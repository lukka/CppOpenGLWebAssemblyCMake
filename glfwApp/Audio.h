#pragma once
#include <memory>
#include <vector>
#if defined(__APPLE__) || defined(_MSC_VER)
#include <al.h>
#include <alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif
#include <iostream>
#include <map>
#include "RingBuffer.h"

#define OPENAL_TEST_ERROR(_msg)				\
	do{int error = alGetError();			\
	if (error != AL_NO_ERROR) {				\
		std::cerr<<_msg<<" "<<error<<"\n";	\
		exit(-1);							\
	}} while(false)

struct Audio
{
	Audio();
	~Audio();
	void RenderAudio(int const *samples, size_t size);

private:
	static const size_t STREAMING_BUFFER_SIZE = 960;// 480 16bit samples
	static const size_t MAX_BUFFER_COUNT = 50;// OpenAL buffers created.
	static const size_t FREQ = 48000;// Samples per second.

	ALCdevice *_device;// OopenAL playback device.
	ALCcontext *_context;// OpenAL context.
	ALuint _source;// id of the OpenAL source.
	ALuint _buffers[MAX_BUFFER_COUNT];// ids of the OpenAL created buffers.
	std::vector<std::unique_ptr<uint8_t[]>> _rawBuffers;// Storage of raw samples to play.
	size_t _lastWritten = 0;// 0 ... STREAMING_BUFFER_SIZE - 2
	std::map<ALuint, size_t> _buffersMap;// OpenAL id to _rawBuffers's index map.
	RingBuffer<ALuint, MAX_BUFFER_COUNT> _buffersToFill;// Audio buffers available for filling and enqueing.
};
