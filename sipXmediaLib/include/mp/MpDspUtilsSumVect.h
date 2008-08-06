//
// Copyright (C) 2007 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
//
// Copyright (C) 2007 SIPez LLC.
// Licensed to SIPfoundry under a Contributor Agreement.
//
// $$
//////////////////////////////////////////////////////////////////////////////

// Author: Alexander Chemeris <Alexander DOT Chemeris AT SIPez DOT com>

#ifndef _MpDspUtilsSumVect_h_
#define _MpDspUtilsSumVect_h_

/**
*  @file
*
*  DO NOT INCLUDE THIS FILE DIRECTLY! This files is designed to be included
*  to <mp/MpDspUtils.h> and should not be used outside of it.
*/

/* ============================ INLINE METHODS ============================ */

#ifdef MP_FIXED_POINT // [

OsStatus MpDspUtils::add_I(const int16_t *pSrc1, int32_t *pSrc2Dst, int dataLength)
{
   for (int i=0; i<dataLength; i++)
   {
      add_I(pSrc2Dst[i], (int32_t)pSrc1[i]);
   }
   return OS_SUCCESS;
}

OsStatus MpDspUtils::add_IGain(const int16_t *pSrc1, int32_t *pSrc2Dst, int dataLength, unsigned src1ScaleFactor)
{
   for (int i=0; i<dataLength; i++)
   {
      add_I(pSrc2Dst[i], ((int32_t)pSrc1[i])<<src1ScaleFactor);
   }

   return OS_SUCCESS;
}

OsStatus MpDspUtils::add_IAtt(const int16_t *pSrc1, int32_t *pSrc2Dst, int dataLength, unsigned src1ScaleFactor)
{
   for (int i=0; i<dataLength; i++)
   {
      add_I(pSrc2Dst[i], (int32_t)pSrc1[i]>>src1ScaleFactor);
   }

   return OS_SUCCESS;
}

OsStatus MpDspUtils::add(const int32_t *pSrc1, const int32_t *pSrc2, int32_t *pDst, int dataLength)
{
   for (int i=0; i<dataLength; i++)
   {
      pDst[i] = add(pSrc1[i], pSrc2[i]);
   }
   return OS_SUCCESS;
}

OsStatus MpDspUtils::addMul_I(const int16_t *pSrc1, int16_t val, int32_t *pSrc2Dst, int dataLength)
{
   for (int i=0; i<dataLength; i++)
   {
      add_I(pSrc2Dst[i], pSrc1[i]*val);
   }

   return OS_SUCCESS;
}

OsStatus MpDspUtils::mul(const int16_t *pSrc, const int16_t val, int32_t *pDst, int dataLength)
{
   for (int i=0; i<dataLength; i++)
   {
      pDst[i] = pSrc[i]*val;
   }

   return OS_SUCCESS;
}

#else  // MP_FIXED_POINT ][

OsStatus MpDspUtils::add_I(const int16_t *pSrc1, float *pSrc2Dst, int dataLength)
{
   for (int i=0; i<dataLength; i++)
   {
      add_I(pSrc2Dst[i], pSrc1[i]);
   }

   return OS_SUCCESS;
}

OsStatus MpDspUtils::add(const float *pSrc1, const float *pSrc2, float *pDst, int dataLength)
{
   for (int i=0; i<dataLength; i++)
   {
      pDst[i] = add(pSrc1[i], pSrc2[i]);
   }

   return OS_SUCCESS;
}

OsStatus MpDspUtils::addMul_I(const int16_t *pSrc1, float val, float *pSrc2Dst, int dataLength)
{
   for (int i=0; i<dataLength; i++)
   {
      add_I(pSrc2Dst[i], pSrc1[i]*val);
   }

   return OS_SUCCESS;
}

OsStatus MpDspUtils::mul(const int16_t *pSrc, const float val, float *pDst, int dataLength)
{
   for (int i=0; i<dataLength; i++)
   {
      pDst[i] = pSrc[i]*val;
   }

   return OS_SUCCESS;
}

#endif // MP_FIXED_POINT ]

int MpDspUtils::maxAbs(const int16_t *pSrc, int dataLength)
{
   int startValue = pSrc[0];
   if (startValue < 0)
      startValue = -startValue;

   for (int i = 1; i < dataLength; i++)
   {
      startValue = maximum(startValue, 
         ((int)pSrc[i] > 0) ? (int)pSrc[i] : -((int)pSrc[i]));
   }
   return startValue;
}

#endif  // _MpDspUtilsSumVect_h_
