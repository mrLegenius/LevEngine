#include "levpch.h"
#include "ParallelJob.h"

namespace LevEngine
{
    void ParallelJob::Run(const int count)
    {
        const auto function = m_Function;
        m_IsRunning = true;
        vgjs::schedule([=]
        {
            for (int i = 0; i < count; i++)
            {
                vgjs::schedule([=]
                {
                    try
                    {
                        function(i);
                    }
                    catch (std::exception& e)
                    {
                        Log::CoreError("Error in parallel job: '{}'", e.what());
                    }
                });
            }

            vgjs::continuation([=]
            {
                m_IsRunning = false;
            });
        });
    }

    void ParallelJob::Wait() const
    {
        while (m_IsRunning)
            std::this_thread::sleep_for(microseconds(10));
    }
}
