#include "levpch.h"
#include "Job.h"

namespace LevEngine
{
    void Job::Run()
    {
        const auto function = m_Function;
        m_IsRunning = true;
        vgjs::schedule([=]
        {
            vgjs::schedule([=]
            {
                try
                {
                    function();
                }
                catch (std::exception& e)
                {
                    Log::CoreError("Error in parallel job: '{}'", e.what());
                }
            });

            vgjs::continuation([=]
            {
                m_IsRunning = false;
                Log::Debug("Job is finished");
            });
        });
    }

    void Job::Wait() const
    {
        while (m_IsRunning)
            std::this_thread::sleep_for(microseconds(10));
    }
}
