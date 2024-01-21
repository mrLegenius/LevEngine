namespace LevEngine
{
    template<typename T, size_t N>
    YAML::Emitter& operator<<(YAML::Emitter& out, const Array<T, N>& array)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq;
        for (const auto arrayElement : array)
        {
            out << arrayElement;
        }
        out << YAML::EndSeq;
        return out;
    }
    
    template <typename T>
    bool TryParse(const YAML::Node& node, T& value)
    {
        if (const auto data = node)
        {
            value = data.as<T>();

            return true;
        }

        return false;
    }

    template <typename T>
    void SerializeList(YAML::Emitter& out, const char* listName, Vector<T> list)
    {
        out << YAML::Key << listName << YAML::Value << YAML::BeginSeq;
        for (const T listElement : list)
        {
            out << YAML::Value << listElement;
        }
        out << YAML::EndSeq;
    }

    template <class T>
    Ref<T> DeserializeAsset(YAML::Node&& node)
    {
        static_assert(std::is_base_of_v<Asset, T>, "T must be Asset");

        if (!node) return nullptr;

        try
        {
            const UUID assetUUID = node.as<std::uint64_t>();
            return AssetDatabase::GetAsset<T>(assetUUID);
        }
        catch (std::exception& e)
        {
            Log::CoreWarning("Failed to deserialize asset. {0}", e.what());
            return nullptr;
        }
    }

    template <class T>
    Ref<T> DeserializeAsset(const YAML::Node& node)
    {
        static_assert(std::is_base_of_v<Asset, T>, "T must be Asset");

        if (!node) return nullptr;

        try
        {
            const UUID assetUUID = node.as<std::uint64_t>();
            return AssetDatabase::GetAsset<T>(assetUUID);
        }
        catch (std::exception& e)
        {
            Log::CoreWarning("Failed to deserialize asset. {0}", e.what());
            return nullptr;
        }
    }
}
