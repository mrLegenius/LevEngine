#pragma once

namespace LevEngine::Editor
{
    // Writes JSON. Nothing reads it here, because yaml-cpp already parses JSON -- YAML is a superset
    // of it -- so the arguments of a command are read as a YAML::Node and only the answer needs code.
    class JsonWriter
    {
    public:
        JsonWriter& BeginObject();
        JsonWriter& EndObject();

        JsonWriter& BeginArray();
        JsonWriter& EndArray();

        JsonWriter& Key(const String& key);

        JsonWriter& Value(const String& value);
        JsonWriter& Value(const char* value);
        JsonWriter& Value(double value);
        JsonWriter& Value(float value) { return Value(static_cast<double>(value)); }
        JsonWriter& Value(int value) { return Value(static_cast<double>(value)); }
        JsonWriter& Value(unsigned value) { return Value(static_cast<double>(value)); }
        JsonWriter& Value(uint64_t value);
        JsonWriter& Value(bool value);
        JsonWriter& Null();

        template<class T>
        JsonWriter& KeyValue(const String& key, const T& value)
        {
            Key(key);
            Value(value);
            return *this;
        }

        JsonWriter& KeyVector3(const String& key, const Vector3& value);
        JsonWriter& Vector3Value(const Vector3& value);

        [[nodiscard]] String Str() const { return String(m_Buffer.c_str()); }

    private:
        void PrepareValue();
        static std::string Escape(const std::string& text);

        std::string m_Buffer;

        //<--- true while the next thing written is the first member of its container ---<<
        Vector<bool> m_First;
        bool m_ExpectingValue = false;
    };

    //<--- Reading helpers over the argument node. Everything is optional and everything has a
    //default, so a command never fails because a field was left out ---<<
    namespace JsonRead
    {
        bool Has(const YAML::Node& node, const String& key);

        String GetString(const YAML::Node& node, const String& key, const String& fallback = String{});
        float GetFloat(const YAML::Node& node, const String& key, float fallback = 0.0f);
        int GetInt(const YAML::Node& node, const String& key, int fallback = 0);
        bool GetBool(const YAML::Node& node, const String& key, bool fallback = false);
        uint64_t GetUInt64(const YAML::Node& node, const String& key, uint64_t fallback = 0);

        //<--- Accepts [x, y, z] and {"x": .., "y": .., "z": ..} ---<<
        bool TryGetVector3(const YAML::Node& node, const String& key, Vector3& outValue);
    }
}
