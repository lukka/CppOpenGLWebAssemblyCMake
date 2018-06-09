#pragma once
#include <vector>
#include "emulator/DiscreteSoundNode.h"
#include "emulator/Timer.h"
#include "emulator/emulator.h"


/* 8 voices max */
#define MAX_VOICES 8

#define MAX_VOLUME 16

/* a position of waveform sample */
#define WAVEFORM_POSITION(n) (((n) >> chip->f_fracbits) & 0x1f)

namespace Galaga
{
class GalagaMachine;

class NamcoSound
{
    /* this structure defines the parameters for a channel */
    struct sound_channel
    {
        uint32_t frequency;
        uint32_t counter;
        int32_t volume;
        int32_t waveform_select;
    };

    struct namco_sound
    {
        /* data about the sound system */
        sound_channel channel_list[MAX_VOICES];
        sound_channel* last_channel;
        std::vector<uint8_t> soundregs;
        const uint8_t* wavedata;
        std::vector<int16_t> waveforms;

        /* global sound parameters */
        int32_t num_voices;
        int32_t sound_enable;
        int namco_clock;
        int f_fracbits;

        /* decoded waveform table */
        int16_t* waveform[MAX_VOLUME];
    };

   public:
    NamcoSound(GalagaMachine& galagaMachine);

    std::vector<int>& update(int64_t currentTime);

    void WriteMemory(uint16_t offset, uint8_t data);
    std::vector<int> UpdateMono(int samples);

   private:
    static void BuildDecodedWaveform(namco_sound* chip, const uint8_t* rgnbase);
    static void UpdateNamcoWaveform(namco_sound* chip, int offset,
                                    uint8_t data);

    void updateToCurrentTime();
    int updateToTime(int64_t time);

    /* generate sound by oversampling */
    inline uint32_t UpdateOne(namco_sound* chip, stream_sample_t* buffer,
                              int length, const int16_t* wave, uint32_t counter,
                              uint32_t freq)
    {
        while (length-- > 0)
        {
            *buffer++ += wave[WAVEFORM_POSITION(counter)];
            counter += freq;
        }

        return counter;
    }

   private:
    static const int SampleRate = 192000;

    GalagaMachine& galagaMachine;
    static const uint32_t romLength = 512;
    uint8_t rom[romLength];
    namco_sound _namcoSound;

    std::vector<int> _buffer;
    int64_t _lastSampleIndex;
};

}  // namespace Galaga
