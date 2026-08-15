#include "pch.h"
#include "AgentJson.h"

namespace LevEngine::Editor
{
    JsonWriter& JsonWriter::BeginObject()
    {
        PrepareValue();
        m_Buffer += '{';
        m_First.push_back(true);
        return *this;
    }

    JsonWriter& JsonWriter::EndObject()
    {
        m_Buffer += '}';

        if (!m_First.empty())
            m_First.pop_back();

        return *this;
    }

    JsonWriter& JsonWriter::BeginArray()
    {
        PrepareValue();
        m_Buffer += '[';
        m_First.push_back(true);
        return *this;
    }

    JsonWriter& JsonWriter::EndArray()
    {
        m_Buffer += ']';

        if (!m_First.empty())
            m_First.pop_back();

        return *this;
    }

    JsonWriter& JsonWriter::Key(const String& key)
    {
        if (!m_First.empty())
        {
            if (!m_First.back())
                m_Buffer += ',';

            m_First.back() = false;
        }

        m_Buffer += '"';
        m_Buffer += Escape(key.c_str());
        m_Buffer += "\":";

        m_ExpectingValue = true;

        return *this;
    }

    JsonWriter& JsonWriter::Value(const String& value)
    {
        PrepareValue();
        m_Buffer += '"';
        m_Buffer += Escape(value.c_str());
        m_Buffer += '"';
        return *this;
    }

    JsonWriter& JsonWriter::Value(const char* value) { return Value(String(value ? value : "")); }

    JsonWriter& JsonWriter::Value(const double value)
    {
        PrepareValue();

        //<--- JSON has no infinity and no NaN, and a stray one would make the whole answer
        //unparseable on the other side ---<<
        if (!std::isfinite(value))
        {
            m_Buffer += "null";
            return *this;
        }

        char text[64];
        const int written = snprintf(text, sizeof text, "%.6g", value);
        m_Buffer.append(text, written > 0 ? static_cast<size_t>(written) : 0);

        return *this;
    }

    JsonWriter& JsonWriter::Value(const uint64_t value)
    {
        PrepareValue();

        char text[32];
        const int written = snprintf(text, sizeof text, "%llu", static_cast<unsigned long long>(value));
        m_Buffer.append(text, written > 0 ? static_cast<size_t>(written) : 0);

        return *this;
    }

    JsonWriter& JsonWriter::Value(const bool value)
    {
        PrepareValue();
        m_Buffer += value ? "true" : "false";
        return *this;
    }

    JsonWriter& JsonWriter::Null()
    {
        PrepareValue();
        m_Buffer += "null";
        return *this;
    }

    JsonWriter& JsonWriter::KeyVector3(const String& key, const Vector3& value)
    {
        Key(key);
        return Vector3Value(value);
    }

    JsonWriter& JsonWriter::Vector3Value(const Vector3& value)
    {
        BeginArray();
        Value(value.x);
        Value(value.y);
        Value(value.z);
        return EndArray();
    }

    void JsonWriter::PrepareValue()
    {
        //<--- A value right after a key needs no separator, one inside an array does ---<<
        if (m_ExpectingValue)
        {
            m_ExpectingValue = false;
            return;
        }

        if (m_First.empty()) return;

        if (!m_First.back())
            m_Buffer += ',';

        m_First.back() = false;
    }

    std::string JsonWriter::Escape(const std::string& text)
    {
        std::string result;
        result.reserve(text.size());

        for (const unsigned char character : text)
        {
            switch (character)
            {
            case '"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (character < 0x20)
                {
                    char escaped[8];
                    snprintf(escaped, sizeof escaped, "\\u%04x", character);
                    result += escaped;
                }
                else
                {
                    result += static_cast<char>(character);
                }
                break;
            }
        }

        return result;
    }

    namespace JsonRead
    {
        bool Has(const YAML::Node& node, const String& key)
        {
            return node && node.IsMap() && node[key.c_str()];
        }

        String GetString(const YAML::Node& node, const String& key, const String& fallback)
        {
            if (!Has(node, key)) return fallback;

            try { return node[key.c_str()].as<String>(); }
            catch (...) { return fallback; }
        }

        float GetFloat(const YAML::Node& node, const String& key, const float fallback)
        {
            if (!Has(node, key)) return fallback;

            try { return node[key.c_str()].as<float>(); }
            catch (...) { return fallback; }
        }

        int GetInt(const YAML::Node& node, const String& key, const int fallback)
        {
            if (!Has(node, key)) return fallback;

            try { return node[key.c_str()].as<int>(); }
            catch (...) { return fallback; }
        }

        bool GetBool(const YAML::Node& node, const String& key, const bool fallback)
        {
            if (!Has(node, key)) return fallback;

            try { return node[key.c_str()].as<bool>(); }
            catch (...) { return fallback; }
        }

        uint64_t GetUInt64(const YAML::Node& node, const String& key, const uint64_t fallback)
        {
            if (!Has(node, key)) return fallback;

            try { return node[key.c_str()].as<uint64_t>(); }
            catch (...) { return fallback; }
        }

        bool TryGetVector3(const YAML::Node& node, const String& key, Vector3& outValue)
        {
            if (!Has(node, key)) return false;

            const auto& value = node[key.c_str()];

            try
            {
                if (value.IsSequence() && value.size() >= 3)
                {
                    outValue = Vector3{ value[0].as<float>(), value[1].as<float>(), value[2].as<float>() };
                    return true;
                }

                if (value.IsMap())
                {
                    outValue = Vector3{
                        value["x"] ? value["x"].as<float>() : 0.0f,
                        value["y"] ? value["y"].as<float>() : 0.0f,
                        value["z"] ? value["z"].as<float>() : 0.0f
                    };
                    return true;
                }
            }
            catch (...) { return false; }

            return false;
        }
    }
}
