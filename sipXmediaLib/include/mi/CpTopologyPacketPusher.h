// Copyright 2008 AOL LLC.
// Licensed to SIPfoundry under a Contributor Agreement.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA. 
// Copyright (C) 2008 AOL LLC.
// Licensed to SIPfoundry under the LGPL license.
//
// $$
/////////////////////////////////////////////////////////////////////////////

#ifndef _CpTopologyPacketPusher_h_
#define _CpTopologyPacketPusher_h_

// SYSTEM INCLUDES
//#include <...>
// APPLICATION INCLUDES
#include "tapi/sipXtapi.h"
#include "mp/MpTopologyGraph.h"
#include "mediaInterface/IMediaPacketPusher.h"
// DEFINES
// MACROS
// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STRUCTS
// TYPEDEFS

class CpTopologyPacketPusher : public IMediaPacketPusher
{
/* //////////////////////////// PUBLIC //////////////////////////////////// */
public:
    CpTopologyPacketPusher(MpTopologyGraph* const pGraph);
    void pushRtpPacket(const int channel, SIPX_MEDIA_TYPE mediaType, const char* buffer, size_t len);
    void pushRtcpPacket(const int channel, SIPX_MEDIA_TYPE mediaType, const char* buffer, size_t len);
/* //////////////////////////// PROTECTED ///////////////////////////////// */
/* //////////////////////////// PRIVATE /////////////////////////////////// */
private:
    MpTopologyGraph* mpGraph;
};

#endif 