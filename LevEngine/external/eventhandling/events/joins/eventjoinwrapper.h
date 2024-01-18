#pragma once

#include <memory>
#include "../handlers/eventhandlerptr.h"


namespace events {


template<class ...TParams> class IEvent;


namespace joins {


class AbstractEventJoin;


class EventJoinWrapper
{
    public:

        template<class TSome, class ...TParams>
        inline EventJoinWrapper( IEvent<TParams...>& _event, TSome&& handler );

        constexpr EventJoinWrapper() noexcept;
        EventJoinWrapper( EventJoinWrapper&& other ) noexcept;
        EventJoinWrapper( EventJoinWrapper& other ) noexcept;

        EventJoinWrapper& operator=( EventJoinWrapper&& other ) noexcept;
        EventJoinWrapper& operator=( const EventJoinWrapper& other ) noexcept;

        operator bool() const;

        bool isAssigned() const;

        bool isJoined() const;
        bool join();
        bool unjoin();

    private:

        std::shared_ptr<AbstractEventJoin> m_eventJoin;
};


} // joins


using EventJoin = joins::EventJoinWrapper;


} // events
