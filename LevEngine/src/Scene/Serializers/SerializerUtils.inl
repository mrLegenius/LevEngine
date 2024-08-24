namespace LevEngine
{
    template <typename T>
    void Write(YAML::Emitter& out, String key, T value)
    {
        out << YAML::Key << key << YAML::Value << value;
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
