#pragma once

namespace LevEngine
{
    template<typename F>
    concept parallelJobFunctor = eastl::is_convertible_v<eastl::decay_t<F>, Action<int>>;
	
    class ParallelJob
    {
    public:
        template<typename F>
        requires parallelJobFunctor<F>
        explicit ParallelJob(F&& function) : m_Function(function) { }

        void Schedule(int count);
        void Run(int count) const;
        void Wait() const;

    private:
        Action<int> m_Function{};
        bool m_IsRunning{};
    };
}

