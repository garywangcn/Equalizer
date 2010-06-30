
/* Copyright (c) 2007-2008, Stefan Eilemann <eile@equalizergraphics.com> 
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * - Neither the name of Eyescale Software GmbH nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "configEvent.h"

using namespace std;

namespace eqPixelBench
{
std::ostream& operator << ( std::ostream& os, const ConfigEvent* event )
{
    switch( event->data.type )
    {
        case ConfigEvent::READBACK:
            os  << "readback";
            break;

        case ConfigEvent::ASSEMBLE:
            os  << "assemble";
            break;
        case ConfigEvent::COMPLET_OPERATION:
            os  << "complete Operation";
            break;
        case ConfigEvent::START_LATENCY:
        case ConfigEvent::DESCRIPTION:
            os  << "        ";
            break;
        default:
            os << static_cast< const eq::ConfigEvent* >( event );
            return os;
    }
    
    if ( event->data.type == ConfigEvent::READBACK && event->testCompress )
    {
        os << event->area << "( size GPU : " << event->dataSizeGPU << " bytes ";
        os << "/ size CPU : " << event->dataSizeCPU << " bytes ";
        os << "/ time : " <<  event->msec << "ms )";
    }
    else if ( event->data.type == ConfigEvent::ASSEMBLE && event->testCompress )
    {
        os << event->area << "( size CPU : " << event->dataSizeCPU << " bytes ";
        os << "/ time : " <<  event->msec << "ms )";
    }
    else if ( event->data.type == ConfigEvent::DESCRIPTION )
    {
        os << event->formatType;
    }
    else
    {   os << " \"" << event->data.user.data << "\" " << event->formatType
           << string( 50-strlen( event->formatType ), ' ' ) << event->area << ": ";
    
        if( event->msec < 0.0f )
            os << "error 0x" << hex << static_cast< int >( -event->msec ) << dec;
        else
            os << static_cast< uint32_t >( event->area.x() * event->area.y() / 
                                           event->msec  / 1048.576f )
               << "MPix/sec (" << event->msec << "ms, " << 1000.0f / event->msec
               << "FPS)";
    }
    return os;
}
}
