
/* Copyright (c) 2007-2009, Stefan Eilemann <eile@equalizergraphics.com> 
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 2.1 as published
 * by the Free Software Foundation.
 *  
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "compoundUpdateInputVisitor.h"

#include "frame.h"
#include "frameData.h"

#include <eq/client/log.h>

using namespace std;
using namespace stde;
using namespace eq::base;

namespace eq
{
namespace server
{
CompoundUpdateInputVisitor::CompoundUpdateInputVisitor(
    const stde::hash_map<std::string, Frame*>& outputFrames )
        : _outputFrames( outputFrames )
{}

VisitorResult CompoundUpdateInputVisitor::visit( Compound* compound )
{
    if( !compound->isActive( ))
        return TRAVERSE_PRUNE;    

    const std::vector< Frame* >& inputFrames = compound->getInputFrames();
    const Channel*               channel     = compound->getChannel();

    if( !compound->testInheritTask( eq::TASK_ASSEMBLE ) || !channel )
        return TRAVERSE_CONTINUE;

    if( inputFrames.empty( ))
    {
        compound->unsetInheritTask( eq::TASK_ASSEMBLE );
        return TRAVERSE_CONTINUE;
    }

    for( vector<Frame*>::const_iterator i = inputFrames.begin(); 
         i != inputFrames.end(); ++i )
    {
        //----- Find corresponding output frame
        Frame*                             frame = *i;
        const std::string&                 name = frame->getName();
        Compound::FrameMap::const_iterator iter =_outputFrames.find( name );

        if( iter == _outputFrames.end())
        {
            EQVERB << "Can't find matching output frame, ignoring input frame "
                   << name << endl;
            frame->unsetData();
            continue;
        }

        //----- Set frame parameters:
        // 1) Frame offset
        Frame*                 outputFrame = iter->second;
        const eq::Viewport&        frameVP = frame->getViewport();
        const eq::PixelViewport& inheritPVP=compound->getInheritPixelViewport();
        eq::PixelViewport         framePVP = inheritPVP.getSubPVP( frameVP );
        Vector2i frameOffset = outputFrame->getMasterData()->getOffset();

        frameOffset += frame->getOffset();
         
        if( channel != compound->getInheritChannel() &&
            compound->getIAttribute( Compound::IATTR_HINT_OFFSET ) != eq::ON )
        {
            // compute delta offset between source and destination, since the
            // channel's native origin (as opposed to destination) is used.
            frameOffset.x() -= framePVP.x;
            frameOffset.y() -= framePVP.y;
        }
        frame->setInheritOffset( frameOffset );

        // 2) zoom
        _updateZoom( compound, frame, outputFrame );

        // 3) TODO input frames are moved using the offset. The pvp signifies
        //    the pixels to be used from the frame data.
        //framePVP.x = static_cast< int32_t >( frameVP.x * inheritPVP.w );
        //framePVP.y = static_cast< int32_t >( frameVP.y * inheritPVP.h );
        //frame->setInheritPixelViewport( framePVP );

        //----- Link input frame to output frame (connects frame data)
        outputFrame->addInputFrame( frame, compound->getInheritEyes( ));

        //----- Commit
        frame->commit();
        EQLOG( eq::LOG_ASSEMBLY )
            << "Input frame  \"" << name << "\" on channel \"" 
            << channel->getName() << "\" id " << frame->getID() << " v"
            << frame->getVersion() << "\" tile pos " << frameOffset << ' ' 
            << frame->getInheritZoom() << endl;
    }

    return TRAVERSE_CONTINUE;
}

void CompoundUpdateInputVisitor::_updateZoom( const Compound* compound,
                                              Frame* frame, 
                                              const Frame* outputFrame )
{
    Zoom zoom = frame->getZoom();
    if( !zoom.isValid( )) // if zoom is not set, inherit from parent
        zoom = compound->getInheritZoom();

    // Zoom difference between output and input
    const FrameData* frameData = outputFrame->getMasterData();
    zoom /= frameData->getZoom();

    frame->setInheritZoom( zoom );
}

}
}
