#include "levpch.h"
#include "D3D11Query.h"

namespace LevEngine
{
    D3D11Query::D3D11Query(ID3D11Device2* device, const QueryType queryType, const uint8_t numBuffers)
        : m_QueryType(queryType)
          , m_NumBuffers(numBuffers)
    {
        device->GetImmediateContext2(&m_DeviceContext);
        
        D3D11_QUERY_DESC queryDesc = {};

        switch (m_QueryType)
        {
        case QueryType::Timer:
            queryDesc.Query = D3D11_QUERY_TIMESTAMP;
            break;
        case QueryType::CountSamples:
            queryDesc.Query = D3D11_QUERY_OCCLUSION;
            break;
        case QueryType::CountSamplesPredicate:
            queryDesc.Query = D3D11_QUERY_OCCLUSION_PREDICATE;
            break;
        case QueryType::CountPrimitives:
        case QueryType::CountTransformFeedbackPrimitives:
            queryDesc.Query = D3D11_QUERY_SO_STATISTICS;
            break;
        default:
            LEV_THROW("Unknown Query Type");
            break;
        }

        m_Queries[0].resize(m_NumBuffers);

        for (uint8_t i = 0; i < m_NumBuffers; ++i)
        {
            const auto hr = device->CreateQuery(&queryDesc, &m_Queries[0][i]);
            LEV_CORE_ASSERT(SUCCEEDED(hr), "Failed to create Query")
        }

        // For timer queries, we also need to create the disjoint timer queries.
        if (m_QueryType == QueryType::Timer)
        {
            m_DisjointQueries.resize(m_NumBuffers);
            m_Queries[1].resize(m_NumBuffers);

            D3D11_QUERY_DESC disjointQueryDesc = {};
            disjointQueryDesc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;

            for (uint8_t i = 0; i < m_NumBuffers; ++i)
            {
                {
                    const auto hr = device->CreateQuery(&queryDesc, &m_Queries[1][i]);
                    LEV_CORE_ASSERT(SUCCEEDED(hr), "Failed to create Query")
                }

                {
                    const auto hr = device->CreateQuery(&disjointQueryDesc, &m_DisjointQueries[i]);
                    LEV_CORE_ASSERT(SUCCEEDED(hr), "Failed to create Disjount Query")
                }
            }
        }
    }

    void D3D11Query::Begin(const int64_t frame)
    {
        const int buffer = static_cast<int>(frame - 1L) % m_NumBuffers;
        if (buffer >= 0)
        {
            if (m_QueryType == QueryType::Timer)
            {
                m_DeviceContext->Begin(m_DisjointQueries[buffer].Get());
                m_DeviceContext->End(m_Queries[0][buffer].Get());
            }
            else
            {
                m_DeviceContext->Begin(m_Queries[0][buffer].Get());
            }
        }
    }

    void D3D11Query::End(const int64_t frame)
    {
        const int buffer = static_cast<int>(frame - 1L) % m_NumBuffers;
        if (buffer >= 0)
        {
            if (m_QueryType == QueryType::Timer)
            {
                m_DeviceContext->End(m_Queries[1][buffer].Get());
                m_DeviceContext->End(m_DisjointQueries[buffer].Get());
            }
            else
            {
                m_DeviceContext->End(m_Queries[0][buffer].Get());
            }
        }
    }

    bool D3D11Query::IsQueryResultAvailable(const int64_t frame)
    {
        bool result = false;
        const int buffer = static_cast<int>(frame - 1L) % m_NumBuffers;

        if (buffer >= 0)
        {
            if (m_QueryType == QueryType::Timer)
            {
                result = m_DeviceContext->GetData(m_DisjointQueries[buffer].Get(), nullptr, 0, 0) == S_OK;
            }
            else
            {
                result = m_DeviceContext->GetData(m_Queries[0][buffer].Get(), nullptr, 0, 0) == S_OK;
            }
        }

        return result;
    }

    Query::QueryResult D3D11Query::GetQueryResult(const int64_t frame)
    {
        QueryResult result = {};
        const int buffer = static_cast<int>(frame - 1L) % m_NumBuffers;

        if (buffer >= 0)
        {
            if (m_QueryType == QueryType::Timer)
            {
                while (m_DeviceContext->GetData(m_DisjointQueries[buffer].Get(), nullptr, 0, 0) == S_FALSE)
                {
                    Sleep(1L);
                }
                D3D11_QUERY_DATA_TIMESTAMP_DISJOINT timeStampDisjoint;
                m_DeviceContext->GetData(m_DisjointQueries[buffer].Get(), &timeStampDisjoint,
                                          sizeof(D3D11_QUERY_DATA_TIMESTAMP_DISJOINT), 0);
                
                if (timeStampDisjoint.Disjoint == FALSE)
                {
                    UINT64 beginTime, endTime;
                    if (m_DeviceContext->GetData(m_Queries[0][buffer].Get(), &beginTime, sizeof(UINT64), 0) == S_OK &&
                        m_DeviceContext->GetData(m_Queries[1][buffer].Get(), &endTime, sizeof(UINT64), 0) == S_OK)
                    {
                        result.ElapsedTime = static_cast<double>(endTime - beginTime) / static_cast<double>(timeStampDisjoint.Frequency);
                        result.IsValid = true;
                    }
                }
            }
            else
            {
                // Wait for the results to become available.
                while (m_DeviceContext->GetData(m_Queries[0][buffer].Get(), nullptr, 0, 0))
                {
                    Sleep(1L);
                }

                switch (m_QueryType)
                {
                case QueryType::CountSamples:
                    {
                        UINT64 numSamples = 0;
                        if (m_DeviceContext->GetData(m_Queries[0][buffer].Get(), &numSamples, sizeof(UINT64), 0) ==
                            S_OK)
                        {
                            result.NumSamples = numSamples;
                            result.IsValid = true;
                        }
                    }
                    break;
                case QueryType::CountSamplesPredicate:
                    {
                        BOOL anySamples = FALSE;
                        if (m_DeviceContext->GetData(m_Queries[0][buffer].Get(), &anySamples, sizeof(UINT64), 0) ==
                            S_OK)
                        {
                            result.AnySamples = anySamples == TRUE;
                            result.IsValid = true;
                        }
                    }
                    break;
                case QueryType::CountPrimitives:
                case QueryType::CountTransformFeedbackPrimitives:
                    {
                        D3D11_QUERY_DATA_SO_STATISTICS streamOutStats = {};
                        if (m_DeviceContext->GetData(m_Queries[0][buffer].Get(), &streamOutStats,
                                                      sizeof(D3D11_QUERY_DATA_SO_STATISTICS), 0) == S_OK)
                        {
                            result.PrimitivesGenerated = result.TransformFeedbackPrimitives = streamOutStats.
                                NumPrimitivesWritten;
                            result.IsValid = true;
                        }
                    }
                    break;
                default:
                    break;
                }
            }
        }

        return result;
    }

    uint8_t D3D11Query::GetBufferCount() const { return m_NumBuffers; }
}
