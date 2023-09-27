#pragma once

namespace LevEngine
{
    template<typename... Args>
    using Action = eastl::function<void(Args...)>;

    template<typename TResult, typename... Args>
    using Func = eastl::function<TResult(Args...)>;
}


