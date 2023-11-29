#pragma once

namespace LevEngine
{
    class Texture;
    
    class Asset
    {
    public:
        explicit Asset(const Path& path, UUID uuid);

        virtual ~Asset() = default;

        [[nodiscard]] String GetName() const { return m_Name; }
        [[nodiscard]] String GetFullName() const { return m_FullName; }
        [[nodiscard]] String GetExtension() const { return m_Extension; }
        [[nodiscard]] Path GetPath() const { return m_Path; }
        [[nodiscard]] UUID GetUUID() const { return m_UUID; }
        [[nodiscard]] bool IsDeserialized() const { return m_Deserialized; }

        [[nodiscard]] String GetAddress() const { return m_Address; }
        void SetAddress(const String& address) { m_Address = address; }

        void Serialize();
        bool Deserialize();
        void SerializeMeta();

        void Rename(const Path& path);

        [[nodiscard]] virtual Ref<Texture> GetIcon() const;

    protected:
        [[nodiscard]] virtual bool WriteDataToFile() const { return true; }
        [[nodiscard]] virtual bool ReadDataFromFile() const { return true; }

        virtual void SerializeData(YAML::Emitter& out) = 0;
        virtual void DeserializeData(YAML::Node& node) = 0;

        virtual void SerializeMeta(YAML::Emitter& out)
        {
        }

        virtual void DeserializeMeta(YAML::Node& node)
        {
        }

        String m_Name;
        String m_FullName;
        String m_Extension;
        Path m_MetaPath;
        Path m_Path;
        UUID m_UUID;
        String m_Address;
        bool m_Deserialized = false;

    private:
        void SerializeData();
        bool DeserializeData();
        bool DeserializeMeta();
    };
}
