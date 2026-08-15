#pragma once

namespace LevEngine::Editor
{
    // A loopback HTTP server the editor answers commands on.
    //
    // HTTP because everything can speak it with no library: the bridge that Claude Code talks to is
    // twenty lines of node, and a browser or curl can drive the editor by hand while debugging.
    //
    // The socket thread never touches the engine. It parses a request, hands it to the main thread as
    // a Call and blocks until that thread answers, which is the only way a command can safely read a
    // registry or move a camera.
    class AgentServer
    {
    public:
        struct Call
        {
            String Tool;
            //<--- The raw JSON body, parsed on the main thread ---<<
            String Arguments;

            String Response;
            bool IsCompleted = false;

            std::mutex Mutex;
            std::condition_variable Completed;
        };

        ~AgentServer();

        //<--- Binds to 127.0.0.1 only, so nothing outside the machine can drive the editor ---<<
        bool Start(uint16_t port);
        void Stop();

        [[nodiscard]] bool IsRunning() const { return m_IsRunning; }
        [[nodiscard]] uint16_t GetPort() const { return m_Port; }

        //<--- Main thread: the next command to run, or nullptr ---<<
        Ref<Call> TryTakePendingCall();

        //<--- Main thread: hands the answer back to whichever socket is waiting for it ---<<
        static void Complete(const Ref<Call>& call, const String& response);

    private:
        void Run();
        void HandleConnection(uintptr_t socket);

        std::thread m_Thread;

        std::atomic<bool> m_IsRunning{ false };
        std::atomic<bool> m_IsStopping{ false };

        uint16_t m_Port = 0;

        //<--- SOCKET is an unsigned integer, kept as one so no Windows header leaks into a header ---<<
        std::atomic<uintptr_t> m_ListenSocket{ ~static_cast<uintptr_t>(0) };

        std::mutex m_QueueMutex;
        Queue<Ref<Call>> m_Queue;
    };
}
