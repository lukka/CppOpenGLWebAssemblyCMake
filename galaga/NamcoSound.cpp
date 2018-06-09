#include "NamcoSound.h"
#include "GalagaMachine.h"

/* quality parameter: internal sample rate is 192 KHz, output is 48 KHz */
#define INTERNAL_RATE 192000

/* 16 bits: sample bits of the stream buffer    */
/* 4 bits:  volume                  */
/* 4 bits:  prom sample bits            */
#define MIXLEVEL (1 << (16 - 4 - 4))

/* stream output level */
#define OUTPUT_LEVEL(n) ((n)*MIXLEVEL / chip->num_voices)

namespace Galaga
{
NamcoSound::NamcoSound(GalagaMachine& galaga)
    : galagaMachine(galaga), _buffer(), _lastSampleIndex(0)
{
    ByteArray prom11d = galaga.ReadRom("prom-1.1d");
    std::copy(begin(prom11d), end(prom11d), rom);

    ByteArray prom25c = galaga.ReadRom("prom-2.5c");
    for (int i = 0; i < 256; i++)
    {
        rom[256 + i] = prom25c[i];
    }

    namco_sound* chip = &_namcoSound;

    /* extract globals from the interface */
    chip->num_voices = 3;
    chip->last_channel = chip->channel_list + chip->num_voices;

    chip->soundregs.resize(0x400);

    /* adjust internal clock */
    chip->namco_clock = 96000;
    int clock_multiple;
    for (clock_multiple = 0; chip->namco_clock < INTERNAL_RATE;
         clock_multiple++)
        chip->namco_clock *= 2;

    chip->f_fracbits = clock_multiple + 15;

    /* build the waveform table */
    BuildDecodedWaveform(chip, rom);

    /* start with sound enabled, many games don't have a sound enable register
     */
    chip->sound_enable = 1;

    /* reset all the voices */
    for (sound_channel* voice = chip->channel_list; voice < chip->last_channel;
         voice++)
    {
        voice->frequency = 0;
        voice->volume = 0;
        voice->waveform_select = 0;
        voice->counter = 0;
    }
}

std::vector<int>& NamcoSound::update(int64_t currentTime)
{
    size_t samplesToGenerate = updateToTime(currentTime);

    if (_buffer.size() < samplesToGenerate)
    {
        for (size_t i = 0; i < samplesToGenerate - _buffer.size(); i++)
        {
            _buffer.push_back(0);
        }
    }

    return _buffer;
}

/* build the decoded waveform table */
// static
void NamcoSound::BuildDecodedWaveform(namco_sound* chip, const uint8_t* rgnbase)
{
    chip->wavedata = rgnbase;

    int size = 32 * 8; /* 32 samples, 8 waveforms */

    chip->waveforms.resize(size * MAX_VOLUME);
    int16_t* p = &chip->waveforms[0];
    for (int v = 0; v < MAX_VOLUME; v++)
    {
        chip->waveform[v] = p;
        p += size;
    }

    /* We need waveform data. It fails if region is not specified. */
    for (int offset = 0; offset < 256; offset++)
    {
        UpdateNamcoWaveform(chip, offset, chip->wavedata[offset]);
    }
}

/* update the decoded waveform data */
// static
void NamcoSound::UpdateNamcoWaveform(namco_sound* chip, int offset,
                                     uint8_t data)
{
    /* use only low 4 bits */
    for (int v = 0; v < MAX_VOLUME; v++)
    {
        chip->waveform[v][offset] = OUTPUT_LEVEL(((data & 0x0f) - 8) * v);
    }
}

void NamcoSound::WriteMemory(uint16_t offset, uint8_t data)
{
    namco_sound* chip = &_namcoSound;
    sound_channel* voice;
    int ch;

    data &= 0x0f;
    if (chip->soundregs[offset] == data) return;

    updateToCurrentTime();

    /* set the register */
    chip->soundregs[offset] = data;

    if (offset < 0x10)
        ch = (offset - 5) / 5;
    else if (offset == 0x10)
        ch = 0;
    else
        ch = (offset - 0x11) / 5;

    if (ch >= chip->num_voices) return;

    /* recompute the voice parameters */
    voice = chip->channel_list + ch;
    switch (offset - ch * 5)
    {
        case 0x05:
            voice->waveform_select = data & 7;
            break;

        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13:
        case 0x14:
            /* the frequency has 20 bits */
            /* the first voice has extra frequency bits */
            voice->frequency = (ch == 0) ? chip->soundregs[0x10] : 0;
            voice->frequency += (chip->soundregs[ch * 5 + 0x11] << 4);
            voice->frequency += (chip->soundregs[ch * 5 + 0x12] << 8);
            voice->frequency += (chip->soundregs[ch * 5 + 0x13] << 12);
            voice->frequency +=
                (chip->soundregs[ch * 5 + 0x14] << 16); /* always 0 */
            break;

        case 0x15:
            voice->volume = data;
            break;
    }
}

void NamcoSound::updateToCurrentTime()
{
    updateToTime(galagaMachine.get_CurrentTime());
}

int NamcoSound::updateToTime(int64_t currentTime)
{
    int64_t currentSampleIndex = currentTime * (SampleRate / 4) / 1000000;
    currentSampleIndex *= 4;

    int samples = (int)(currentSampleIndex - _lastSampleIndex);
    if (samples <= 0)
    {
        return 0;
    }
    // else if (samples > 4096) {
    //	samples = 4096;
    //}

    auto output = UpdateMono(samples);
    if ((int)output.size() < samples)
    {
        for (size_t i = 0; i < samples - output.size(); i++)
        {
            _buffer.push_back(0);
        }
    }
    _buffer.insert(_buffer.end(), output.begin(), output.end());
    _lastSampleIndex = currentSampleIndex;

    return samples;
}

std::vector<int> NamcoSound::UpdateMono(int samples)
{
    namco_sound* chip = &_namcoSound;
    sound_channel* voice;

    std::vector<int> output;
    output.resize(samples);  // reset values to 0

    /* if no sound, we're done */
    if (chip->sound_enable == 0)
    {
        return output;
    }

    /* loop over each voice and add its contribution */
    for (voice = chip->channel_list; voice < chip->last_channel; voice++)
    {
        int v = voice->volume;

        /* only update if we have non-zero volume and frequency */
        if (v && voice->frequency)
        {
            const int16_t* w = &chip->waveform[v][voice->waveform_select * 32];

            /* generate sound into buffer and update the counter for this voice
             */
            voice->counter = UpdateOne(chip, &output[0], samples, w,
                                       voice->counter, voice->frequency);
        }
    }

    return output;
}

}  // namespace Galaga
