/*
 * Copyright (c) 2007, Wirtualna Polska S.A.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#pragma once

#include <VideoSupport/VideoScaler.h>

#if !defined(VIDEO_SUPPORT_DISABLE_AVCODEC) && !defined(VIDEO_SUPPORT_USE_SWSCALER)

struct AVPicture;
typedef struct ImgReSampleContext ImgReSampleContext;

class AVCodecVideoScaler: public VideoScaler
{
public:

	AVCodecVideoScaler();

	~AVCodecVideoScaler();

	bool Initialize(VideoSurface surface, size_t sourceWidth, size_t sourceHeight, size_t targetWidth, size_t targetHeight);

	bool Process(const void* sourceFrameBytes, size_t sourceFrameByteSize, void* targetFrameBytes, size_t targetFrameByteSize) throw();

private:

	AVCodecVideoScaler(const AVCodecVideoScaler&);
	AVCodecVideoScaler& operator=(const AVCodecVideoScaler&);

	PixelFormat pixelFormat_;
	std::auto_ptr<AVPicture> sourcePicture_;
	std::auto_ptr<AVPicture> targetPicture_;
	ImgReSampleContext* resampleContext_;
	int sourceWidth_;
	int sourceHeight_;
	int targetWidth_;
	int targetHeight_;
};

#endif // VIDEO_SUPPORT_DISABLE_AVCODEC && VIDEO_SUPPORT_USE_SWSCALER
