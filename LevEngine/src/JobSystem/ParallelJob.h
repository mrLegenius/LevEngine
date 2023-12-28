#pragma once

namespace LevEngine
{
    template<typename F>
    concept functor = eastl::is_convertible_v<eastl::decay_t<F>, Action<int>>;
	
    class ParallelJob
    {
    public:
        template<typename F>
        requires functor<F>
        explicit ParallelJob(F&& function) : m_Function(function) { }

        void Run(int count);
        void Wait() const;

    private:
        Action<int> m_Function{};
        bool m_IsRunning{};
    };
}

