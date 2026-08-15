#include "pch.h"
#include "AgentServer.h"

#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

namespace LevEngine::Editor
{
    namespace
    {
        //<--- How long a socket waits for the main thread. A camera path can run for a minute and a
        //project load blocks the main thread for far longer than a command normally takes ---<<
        constexpr int k_CallTimeoutSeconds = 300;

        constexpr size_t k_MaxRequestBytes = 4 * 1024 * 1024;

        std::string ReadAll(const SOCKET socket, size_t maxBytes, bool& outFailed)
        {
            std::string request;
            char buffer[8192];

            size_t contentLength = 0;
            size_t headerEnd = std::string::npos;

            outFailed = false;

            while (true)
            {
                if (headerEnd == std::string::npos)
                {
                    headerEnd = request.find("\r\n\r\n");

                    if (headerEnd != std::string::npos)
                    {
                        //<--- Header names are case insensitive, and node sends 'content-length' ---<<
                        std::string headers = request.substr(0, headerEnd);
                        std::string lowered = headers;
                        std::transform(lowered.begin(), lowered.end(), lowered.begin(),
                            [](const unsigned char character) { return static_cast<char>(std::tolower(character)); });

                        const auto position = lowered.find("content-length:");
                        if (position != std::string::npos)
                            contentLength = static_cast<size_t>(std::strtoul(headers.c_str() + position + 15, nullptr, 10));
                    }
                }

                if (headerEnd != std::string::npos && request.size() >= headerEnd + 4 + contentLength)
                    break;

                if (request.size() > maxBytes)
                {
                    outFailed = true;
                    return {};
                }

                const int received = recv(socket, buffer, sizeof buffer, 0);

                if (received <= 0)
                {
                    //<--- A client that closed before the body arrived ---<<
                    outFailed = headerEnd == std::string::npos;
                    break;
                }

                request.append(buffer, received);
            }

            return request;
        }

        void SendResponse(const SOCKET socket, const int status, const std::string& body)
        {
            const char* reason = status == 200 ? "OK"
                : status == 400 ? "Bad Request"
                : status == 404 ? "Not Found"
                : status == 504 ? "Gateway Timeout"
                : "Internal Server Error";

            std::string response = "HTTP/1.1 " + std::to_string(status) + " " + reason + "\r\n";
            response += "Content-Type: application/json\r\n";
            response += "Content-Length: " + std::to_string(body.size()) + "\r\n";
            response += "Connection: close\r\n\r\n";
            response += body;

            size_t sent = 0;
            while (sent < response.size())
            {
                const int written = send(socket, response.data() + sent, static_cast<int>(response.size() - sent), 0);
                if (written <= 0) break;

                sent += written;
            }
        }

        std::string ErrorBody(const std::string& message)
        {
            std::string escaped;
            for (const char character : message)
            {
                if (character == '"' || character == '\\') escaped += '\\';
                escaped += character;
            }

            return "{\"ok\":false,\"error\":\"" + escaped + "\"}";
        }
    }

    AgentServer::~AgentServer()
    {
        Stop();
    }

    bool AgentServer::Start(const uint16_t port)
    {
        if (m_IsRunning) return true;

        WSADATA data;
        if (WSAStartup(MAKEWORD(2, 2), &data) != 0)
        {
            Log::CoreError("Agent server: WSAStartup failed");
            return false;
        }

        const SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (listenSocket == INVALID_SOCKET)
        {
            Log::CoreError("Agent server: could not create a socket");
            WSACleanup();
            return false;
        }

        sockaddr_in address{};
        address.sin_family = AF_INET;
        address.sin_port = htons(port);
        inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);

        if (bind(listenSocket, reinterpret_cast<sockaddr*>(&address), sizeof address) == SOCKET_ERROR)
        {
            Log::CoreWarning("Agent server: port {0} is taken, the editor will run without it", port);
            closesocket(listenSocket);
            WSACleanup();
            return false;
        }

        if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
        {
            Log::CoreError("Agent server: listen failed");
            closesocket(listenSocket);
            WSACleanup();
            return false;
        }

        m_ListenSocket = static_cast<uintptr_t>(listenSocket);
        m_Port = port;
        m_IsStopping = false;
        m_IsRunning = true;

        m_Thread = std::thread{ [this] { Run(); } };

        Log::CoreInfo("Agent server listening on http://127.0.0.1:{0}", port);

        return true;
    }

    void AgentServer::Stop()
    {
        if (!m_IsRunning) return;

        m_IsStopping = true;

        const auto listenSocket = m_ListenSocket.exchange(~static_cast<uintptr_t>(0));
        if (listenSocket != ~static_cast<uintptr_t>(0))
            closesocket(static_cast<SOCKET>(listenSocket));

        if (m_Thread.joinable())
            m_Thread.join();

        //<--- Whoever is still waiting for an answer will never get one now ---<<
        {
            std::lock_guard lock{ m_QueueMutex };

            while (!m_Queue.empty())
            {
                Complete(m_Queue.front(), R"({"ok":false,"error":"editor is shutting down"})");
                m_Queue.pop();
            }
        }

        m_IsRunning = false;

        WSACleanup();
    }

    void AgentServer::Run()
    {
        while (!m_IsStopping)
        {
            const auto listenSocket = m_ListenSocket.load();
            if (listenSocket == ~static_cast<uintptr_t>(0)) break;

            const SOCKET client = accept(static_cast<SOCKET>(listenSocket), nullptr, nullptr);

            if (client == INVALID_SOCKET)
            {
                //<--- Stop closes the listening socket, which is what wakes this up ---<<
                if (m_IsStopping) break;

                continue;
            }

            HandleConnection(static_cast<uintptr_t>(client));
        }
    }

    void AgentServer::HandleConnection(const uintptr_t socketHandle)
    {
        const auto client = static_cast<SOCKET>(socketHandle);

        bool failed = false;
        const std::string request = ReadAll(client, k_MaxRequestBytes, failed);

        if (failed || request.empty())
        {
            closesocket(client);
            return;
        }

        const auto headerEnd = request.find("\r\n\r\n");
        const std::string body = headerEnd == std::string::npos ? std::string{} : request.substr(headerEnd + 4);

        //<--- "POST /editor_stats HTTP/1.1": the path is the tool, the body is its arguments ---<<
        const auto pathStart = request.find(' ');
        const auto pathEnd = pathStart == std::string::npos ? std::string::npos : request.find(' ', pathStart + 1);

        std::string path = pathStart != std::string::npos && pathEnd != std::string::npos
            ? request.substr(pathStart + 1, pathEnd - pathStart - 1)
            : std::string{};

        if (!path.empty() && path.front() == '/')
            path.erase(path.begin());

        if (const auto query = path.find('?'); query != std::string::npos)
            path.erase(query);

        if (path.empty())
        {
            SendResponse(client, 400, ErrorBody("no tool in the request path, use POST /<tool>"));
            closesocket(client);
            return;
        }

        const auto call = CreateRef<Call>();
        call->Tool = String(path.c_str());
        call->Arguments = String(body.c_str());

        {
            std::lock_guard lock{ m_QueueMutex };
            m_Queue.push(call);
        }

        std::unique_lock lock{ call->Mutex };
        const bool answered = call->Completed.wait_for(lock, std::chrono::seconds(k_CallTimeoutSeconds),
            [&call] { return call->IsCompleted; });

        if (answered)
            SendResponse(client, 200, std::string(call->Response.c_str()));
        else
            SendResponse(client, 504, ErrorBody("the editor did not answer in time"));

        lock.unlock();

        closesocket(client);
    }

    Ref<AgentServer::Call> AgentServer::TryTakePendingCall()
    {
        std::lock_guard lock{ m_QueueMutex };

        if (m_Queue.empty()) return nullptr;

        auto call = m_Queue.front();
        m_Queue.pop();

        return call;
    }

    void AgentServer::Complete(const Ref<Call>& call, const String& response)
    {
        if (!call) return;

        {
            std::lock_guard lock{ call->Mutex };
            call->Response = response;
            call->IsCompleted = true;
        }

        call->Completed.notify_all();
    }
}
