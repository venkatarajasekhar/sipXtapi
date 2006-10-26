//  
// Copyright (C) 2006 SIPez LLC. 
// Licensed to SIPfoundry under a Contributor Agreement. 
//
// Copyright (C) 2004-2006 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
//
// Copyright (C) 2004-2006 Pingtel Corp.  All rights reserved.
// Licensed to SIPfoundry under a Contributor Agreement.
//
// $$
///////////////////////////////////////////////////////////////////////////////


#ifndef HAVE_GIPS /* [ */

#include "assert.h"
#include "string.h"

#include "mp/JB/JB_API.h"
#include "mp/MpJitterBuffer.h"
#include "mp/MpSipxDecoders.h" // for G.711 decoder
#include "mp/MpdSipxSpeex.h"   // for Speex decoder
#include "mp/NetInTask.h"      // for definition of RTP packet

static int debugCount = 0;

/* ============================ CREATORS ================================== */

MpJitterBuffer::MpJitterBuffer()
{
   for (int i=0; i<JbPayloadMapSize; i++)
      payloadMap[i] = NULL;

   JbQCount = 0;
   JbQIn = 0;
   JbQOut = 0;

   debugCount = 0;
}

// Destructor
MpJitterBuffer::~MpJitterBuffer()
{
}

/* ============================ MANIPULATORS ============================== */

int MpJitterBuffer::pushPacket(MpRtpBufPtr &rtpPacket)
{
   unsigned decodedSamples; // number of samples, returned from decoder
   UCHAR payloadType;       // RTP packet payload type
   MpDecoderBase* decoder;  // decoder for the packet

   payloadType = rtpPacket->getRtpPayloadType();

   // Ignore illegal payload types
   if (payloadType >= JbPayloadMapSize)
      return 0;

   // Get decoder
   decoder = payloadMap[payloadType];
   if (decoder == NULL)
      return 0; // If we can't decode it, we must ignore it?

   // Decode packet
   decodedSamples = decoder->decode(rtpPacket, JbQueueSize-JbQCount, JbQ+JbQIn);

   // Update buffer state
   JbQCount += decodedSamples;
   JbQIn += decodedSamples;
   // Reset write pointer if we reach end of buffer
   if (JbQIn >= JbQueueSize)
      JbQIn = 0;

   return 0;
}

int MpJitterBuffer::getSamples(MpAudioSample *samplesBuffer, JB_size samplesNumber)
{
   // Check does we have available decoded data
   if (JbQCount != 0) {
      // We could not return more then we have
      samplesNumber = min(samplesNumber,JbQCount);

      memcpy(samplesBuffer, JbQ+JbQOut, samplesNumber * sizeof(MpAudioSample));

      JbQCount -= samplesNumber;
      JbQOut += samplesNumber;
      if (JbQOut >= JbQueueSize)
         JbQOut -= JbQueueSize;
   }

   return samplesNumber;
}

int MpJitterBuffer::setCodepoint(const JB_char* codec, JB_size sampleRate,
   JB_code codepoint)
{
   return 0;
}

int MpJitterBuffer::setCodecList(MpDecoderBase** codecList, int codecCount)
{
	// For every payload type, load in a codec pointer, or a NULL if it isn't there
	for(int i=0; i<codecCount; i++)
   {
		int payloadType = codecList[i]->getPayloadType();
		if(payloadType < JbPayloadMapSize) {
			payloadMap[payloadType] = codecList[i];
		}
	}

   return 0;
}

/* ===================== Jitter Buffer API Functions ====================== */

JB_ret JB_initCodepoint(JB_inst *JB_inst,
                        const JB_char* codec,
                        JB_size sampleRate,
                        JB_code codepoint)
{
   return JB_inst->setCodepoint(codec, sampleRate, codepoint);
}

JB_ret JB_RecIn(JB_inst *JB_inst,
                MpRtpBufPtr &rtpPacket)
{
   return JB_inst->pushPacket(rtpPacket);
}

JB_ret JB_RecOut(JB_inst *JB_inst,
                 MpAudioSample *voiceSamples,
                 JB_size *pLength)
{
   *pLength = JB_inst->getSamples(voiceSamples, *pLength);
   return 0;
}

JB_ret JB_create(JB_inst **pJB)
{
   *pJB = new MpJitterBuffer();
   return 0;
}

JB_ret JB_init(JB_inst *pJB, int fs)
{
   return 0;
}

JB_ret JB_free(JB_inst *pJB)
{
   delete pJB;
   return 0;
}
#endif /* NOT(HAVE_GIPS) ] */
