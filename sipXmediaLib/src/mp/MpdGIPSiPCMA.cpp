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

#ifdef HAVE_GIPS /* [ */

#include "assert.h"
// APPLICATION INCLUDES
#include "mp/MpdGIPSiPCMA.h"
#include "mp/GIPS/GIPS_API.h"
#include "mp/MprDejitter.h"
const MpCodecInfo MpdGIPSiPCMA::smCodecInfo(
         SdpCodec::SDP_CODEC_GIPS_IPCMA, GIPS_API_VERSION, true,
         8000, 0, 1, 160, 64000, 176, 1280, 2240, 160);
MpdGIPSiPCMA::MpdGIPSiPCMA(int payloadType)
   : MpDecoderBase(payloadType, &smCodecInfo),
     pDecoderState(NULL)
{
   osPrintf("MpdGIPSiPCMA::MpdGIPSiPCMA(%d)\n", payloadType);
}

MpdGIPSiPCMA::~MpdGIPSiPCMA()
{
   freeDecode();
}

OsStatus MpdGIPSiPCMA::initDecode()
{
   int res = 0;

   return ((0==res) ? OS_SUCCESS : OS_NO_MEMORY);
}

OsStatus MpdGIPSiPCMA::freeDecode(void)
{
   int res;
   OsStatus ret = OS_DELETED;

   if (NULL != pDecoderState) {
      res = EG711A_GIPS_10MS16B_free(pDecoderState);
      pDecoderState = NULL;
      ret = OS_SUCCESS;
   }
   return ret;
}
#endif /* HAVE_GIPS ] */
