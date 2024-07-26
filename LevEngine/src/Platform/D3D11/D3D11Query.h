#pragma once
#include <d3d11.h>
#include <wrl/client.h>

#include "Renderer/Query/Query.h"

namespace LevEngine
{
    class D3D11Query final : public Query
    {
    public:
        D3D11Query(QueryType queryType, uint8_t numBuffers);
        
        void Begin(int64_t frame) override;
        void End(int64_t frame) override;
        bool IsQueryResultAvailable(int64_t frame) override;
        [[nodiscard]] QueryResult GetQueryResult(int64_t frame) override;
        [[nodiscard]] uint8_t GetBufferCount() const override;

    private:
        typedef std::vector<Microsoft::WRL::ComPtr<ID3D11Query>> QueryBuffer;
        QueryBuffer m_DisjointQueries;
        // For timer queries, we need 2 sets of buffered queries.
        QueryBuffer m_Queries[2];

        QueryType m_QueryType;
        // How many queries will be used to prevent stalling the GPU.
        uint8_t m_NumBuffers;
    };
}
