#include "Audio.h"
#include <algorithm>
#include <cassert>

Audio::Audio()
{
    ALCchar const* defaultDeviceName =
        alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
    _device = alcOpenDevice(defaultDeviceName);
    assert(_device && "Cannot initialize OpenAL!");
    _context = alcCreateContext(_device, NULL);
    assert(_context && "Cannot create OpenAL context!");
    bool contextSet = alcMakeContextCurrent(_context);
    assert(contextSet && "Cannot set OpenAL context!");
    alGenBuffers(MAX_BUFFER_COUNT, _buffers);
    OPENAL_TEST_ERROR("buffers not created");

    alListener3f(AL_POSITION, 0.f, 0.f, 1.f);
    OPENAL_TEST_ERROR("pos");
    alListener3f(AL_VELOCITY, 0.f, 0.f, 0.f);
    OPENAL_TEST_ERROR("vel");
    ALfloat listenerOri[] = {0.f, 0.f, 1.f, 0.f, 1.f, 0.f};
    alListenerfv(AL_ORIENTATION, listenerOri);
    OPENAL_TEST_ERROR("ori");

    alGenSources(1, &_source);
    OPENAL_TEST_ERROR("source");

    alSourcef(_source, AL_PITCH, 1);
    alSourcef(_source, AL_GAIN, 1);
    alSource3f(_source, AL_POSITION, 0, 0, 0);
    alSource3f(_source, AL_VELOCITY, 0, 0, 0);
    alSourcei(_source, AL_LOOPING, AL_FALSE);
    OPENAL_TEST_ERROR("source attrs");

    _rawBuffers.resize(MAX_BUFFER_COUNT);
    for (size_t i = 0; i < _rawBuffers.size(); i++)
    {
        _rawBuffers[i] = std::make_unique<uint8_t[]>(STREAMING_BUFFER_SIZE);
        alBufferData(_buffers[i], AL_FORMAT_MONO16, _rawBuffers[i].get(), 16,
                     FREQ);
        OPENAL_TEST_ERROR("openal setup albufferdata " << i);
        _buffersMap[_buffers[i]] = i;
    }
    alSourceQueueBuffers(_source, MAX_BUFFER_COUNT, _buffers);
    OPENAL_TEST_ERROR("openal setup alsourcequeuebuffers!");
    alSourcePlay(_source);

    OPENAL_TEST_ERROR("openal setup error!");
}

Audio::~Audio()
{
    alDeleteSources(1, &_source);
    alDeleteBuffers(MAX_BUFFER_COUNT, _buffers);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(_context);
    alcCloseDevice(_device);
}

bool isPlaying(ALuint source)
{
    ALenum state;
    alGetSourcei(source, AL_SOURCE_STATE, &state);
    return (state == AL_PLAYING);
}

void Audio::RenderAudio(int const *samples, size_t sampleCount)
{
    ALint buffersProcessedCount;
    ALuint buffers[MAX_BUFFER_COUNT];
    alGetSourcei(_source, AL_BUFFERS_PROCESSED, &buffersProcessedCount);
    alSourceUnqueueBuffers(_source, buffersProcessedCount, buffers);
    for(size_t i = 0; i < buffersProcessedCount; i++)
    {
        _buffersToFill.push_back(buffers[i]);
    }

    std::vector<ALuint> buffersToPlay;
    buffersToPlay.reserve(MAX_BUFFER_COUNT);
    if (_buffersToFill.size() > 0)
    {
        ALuint b = _buffersToFill.front();
        size_t bufferIdx = _buffersMap[b];

        for (size_t i = 0; i < sampleCount; i++)
        {
            int16_t clamped = std::clamp(samples[i], -32768, 32767);
            if (_lastWritten >= STREAMING_BUFFER_SIZE)
            {
                _lastWritten = 0;
                _buffersToFill.pop_front();
                alBufferData(b, AL_FORMAT_MONO16, _rawBuffers[bufferIdx].get(),
                             STREAMING_BUFFER_SIZE, FREQ);
                OPENAL_TEST_ERROR("buffer data");
                buffersToPlay.push_back(b);

                if(_buffersToFill.size())
                {
                    b = _buffersToFill.front();
                    bufferIdx = _buffersMap[b];
                }
                else
                    break;
            }
            else
            {
                _rawBuffers[bufferIdx][_lastWritten] = clamped & 0xff;
                _rawBuffers[bufferIdx][_lastWritten + 1] = (clamped >> 8) & 0xff;
                _lastWritten += 2;
            }
        }
    }

#if defined(DEBUG)
    //std::cout << "freed:" << _buffersToFill.size()
    //     << " enqueued:" << buffersToPlay.size() << "\n";
#endif
    if (buffersToPlay.size() > 0)
        alSourceQueueBuffers(_source, buffersToPlay.size(), &buffersToPlay[0]);
    OPENAL_TEST_ERROR("enqueue buffers");

    if (!isPlaying(_source))
    {
        alSourcePlay(_source);
        OPENAL_TEST_ERROR("play");
    }
}
