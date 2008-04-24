
/* Copyright (c) 2006-2007, Stefan Eilemann <eile@equalizergraphics.com> 
   All rights reserved. */

#ifndef EQ_STATEVENT_H
#define EQ_STATEVENT_H

#include <eq/client/channelEvent.h>

namespace eq
{
    class Channel;

    /**
     * Holds one statistics event, used for profiling.
     */
    class ScopedStatistics
    {
    public:
        ScopedStatistics( const StatEvent::Type type, Channel* channel );
        ~ScopedStatistics();

    private:
        StatEvent _event;
        Channel*  _channel;
    };
}

#endif // EQ_STATEVENT_H
