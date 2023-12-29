#pragma once

namespace LevEngine
{
    template<typename F>
    concept functor = eastl::is_convertible_v<eastl::decay_t<F>, Action<>>;
	
    class Job
    {
    public:
        template<typename F>
        requires functor<F>
        explicit Job(F&& function) : m_Function(function) { }

        void Run();
        void Wait() const;

    private:
        Action<> m_Function{};
        bool m_IsRunning{};
    };
}
