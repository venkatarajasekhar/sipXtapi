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
//
// Copyright (C) 2006-2007 SIPez LLC.
// Licensed to SIPfoundry under a Contributor Agreement.
//
// Copyright (C) 2004-2007 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
//
// Copyright (C) 2004-2006 Pingtel Corp.  All rights reserved.
// Licensed to SIPfoundry under a Contributor Agreement.
//
// $$
///////////////////////////////////////////////////////////////////////////////


#ifndef _MpRtpOutputAudioConnection_h_
#define _MpRtpOutputAudioConnection_h_

// FORWARD DECLARATIONS
class MpFlowGraphBase;
class MpResource;
class MprEncode;

// SYSTEM INCLUDES
// APPLICATION INCLUDES
#include "mp/MpRtpOutputConnection.h"
#include "mediaInterface/IMediaTransportAdapter.h"

// DEFINES
// MACROS
// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STRUCTS
// TYPEDEFS

/**
*  @brief Connection container for audio part of call.
*/
class MpRtpOutputAudioConnection : public MpRtpOutputConnection
{
/* //////////////////////////// PUBLIC //////////////////////////////////// */
public:

/* ============================ CREATORS ================================== */
///@name Creators
//@{

     /// Constructor
   MpRtpOutputAudioConnection(const UtlString& resourceName,
                              MpConnectionID myID);

     /// Destructor
   virtual
   ~MpRtpOutputAudioConnection();

//@}

/* ============================ MANIPULATORS ============================== */
///@name Manipulators
//@{

     /// Process one frame of audio
   UtlBoolean processFrame(void);

     /// Queues a message to start sending RTP and RTCP packets.
   static OsStatus startSendRtp(OsMsgQ& messageQueue,
                                const UtlString& resourceName,
                                IMediaTransportAdapter* pAdapter,
                                SdpCodec* pPrimary,
                                SdpCodec* pDtmf);

     /// Queues a message to stop sending RTP and RTCP packets.
   static OsStatus stopSendRtp(OsMsgQ& messageQueue,
                               const UtlString& resourceName);


   // TODO: make these message and/or meta data in the MpBufs
     /// Start sending DTMF tone.
   void startTone(int toneId);

     /// Stop sending DTMF tone.
   void stopTone(void);

//@}

/* ============================ ACCESSORS ================================= */
///@name Accessors
//@{

//@}

/* ============================ INQUIRY =================================== */
///@name Inquiry
//@{

//@}

/* //////////////////////////// PROTECTED ///////////////////////////////// */
protected:

     /// Handle any resource operation message
   virtual UtlBoolean handleMessage(MpResourceMsg& rMsg);

     /// Perform the enable operation specific to the MpRtpInputAudioConnection
   virtual UtlBoolean handleEnable();

     /// Perform the disable operation specific to the MpRtpInputAudioConnection
   virtual UtlBoolean handleDisable();

     /// Queues a message to start sending RTP and RTCP packets.
   OsStatus handleStartSendRtp(IMediaTransportAdapter* pAdapter,
                               SdpCodec* pPrimary,
                               SdpCodec* pDtmf);

     /// Queues a message to stop sending RTP and RTCP packets.
   OsStatus handleStopSendRtp();

     /// Sets the flowgraph in any child resources of this resource.
   virtual OsStatus setFlowGraph(MpFlowGraphBase* pFlowGraph);
     /**<
     *  @param[in] pFlowGraph - pointer to a flowgraph to store for future use.
     *
     *  @see MpResource::setFlowGraph()
     */

     /// Sets the notification enabled status in any child resources of this resource.
   virtual OsStatus setNotificationsEnabled(UtlBoolean enable);
     /**<
     *  @see MpResource::setNotificationsEnabled()
     */

/* //////////////////////////// PRIVATE /////////////////////////////////// */
private:

     /// Copy constructor (not implemented for this type)
   MpRtpOutputAudioConnection(const MpRtpOutputAudioConnection& rMpRtpOutputAudioConnection);

     /// Assignment operator (not implemented for this type)
   MpRtpOutputAudioConnection& operator=(const MpRtpOutputAudioConnection& rhs);

   MprEncode*         mpEncode;        ///< Outbound component: Encoder
};

/* ============================ INLINE METHODS ============================ */

#endif  // _MpRtpOutputAudioConnection_h_