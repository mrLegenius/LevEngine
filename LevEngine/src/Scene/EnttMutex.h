#pragma once

struct EnttMutex
{
    inline static std::recursive_mutex Mutex;
};
