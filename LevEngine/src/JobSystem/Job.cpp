#include "levpch.h"
#include "Job.h"

namespace LevEngine
{
    void Job::Schedule()
    {
        const auto function = m_Function;
        m_IsRunning = true;
        vgjs::schedule([=]
            {
                vgjs::continuation([=]
                {
                    m_IsRunning = false;
                });

                try
                {
                    function();
                }
                catch (std::exception& e)
                {
                    Log::CoreError("Error in job: '{}'", e.what());
                }
            });
    }

    void Job::Run() const
    {
        m_Function();
    }

    void Job::Wait() const
    {
        while (m_IsRunning)
            std::this_thread::sleep_for(microseconds(10));
    }
}
