#pragma once

namespace LevEngine
{
    template<typename F>
    concept jobFunctor = eastl::is_convertible_v<eastl::decay_t<F>, Action<>>;
	
    class Job
    {
    public:
        template<typename F>
        requires jobFunctor<F>
        explicit Job(F&& function) : m_Function(function) { }

        void Schedule();
        void Run() const;
        void Wait() const;

    private:
        Action<> m_Function{};
        bool m_IsRunning{};
    };
}
