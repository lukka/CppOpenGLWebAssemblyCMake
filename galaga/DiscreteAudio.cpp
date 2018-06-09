#include "DiscreteAudio.h"
#include "GalagaMachine.h"

#define GALAGA_54XX_DAC_R \
    (1.0 /                \
     (1.0 / RES_K(47) + 1.0 / RES_K(22) + 1.0 / RES_K(10) + 1.0 / RES_K(4.7)))
#define GALAGA_VREF (5.0 * (RES_K(2.2) / (RES_K(3.3) + RES_K(2.2))))

namespace Galaga
{
static const Emulator::OpAmpFilter::Info galaga_chanl1_filt = {
    GALAGA_54XX_DAC_R + RES_K(100),
    0, /* no second input */
    RES_K(22),
    0, /* not used */
    RES_K(220),
    CAP_U(0.001),
    CAP_U(0.001),
    0,           /* not used */
    GALAGA_VREF, /* vRef */
    5,           /* vP */
    0,           /* vN */
};

static const Emulator::OpAmpFilter::Info galaga_chanl2_filt = {
    GALAGA_54XX_DAC_R + RES_K(47),
    0, /* no second input */
    RES_K(10),
    0, /* not used */
    RES_K(150),
    CAP_U(0.01),
    CAP_U(0.01),
    0,           /* not used */
    GALAGA_VREF, /* vRef */
    5,           /* vP */
    0,           /* vN */
};

static const Emulator::OpAmpFilter::Info galaga_chanl3_filt = {
    GALAGA_54XX_DAC_R + RES_K(150),
    0, /* no second input */
    RES_K(22),
    0, /* not used */
    RES_K(470),
    CAP_U(0.01),
    CAP_U(0.01),
    0,           /* not used */
    GALAGA_VREF, /* vRef */
    5,           /* vP */
    0,           /* vN */
};

static const Emulator::Mixer::Desc galaga_final_mixer = {
    DISC_MIXER_IS_OP_AMP,
    {RES_K(33), RES_K(33), RES_K(10)},
    {0}, /* no rNode{} */
    {0}, /* no c{} */
    0,   /* no rI */
    RES_K(3.3),
    0, /* no cF */
    CAP_U(0.1),
    GALAGA_VREF, /* vRef */
    40800,       /* gain */
};

static const Emulator::LadderDAC::Info galaga_54xx_dac = {
    4, /* number of DAC bits */
    {RES_K(47), RES_K(22), RES_K(10), RES_K(4.7)},
    0,
    0,
    0,
    0 /* nothing extra */
};

DiscreteAudio::DiscreteAudio(GalagaMachine& galagaMachine, uint32_t sampleRate)
    : DiscreteDevice(sampleRate),
      _galagaMachine(galagaMachine),
      _mixer(this, galaga_final_mixer),
      _opAmpFilter1(this, galaga_chanl1_filt),
      _opAmpFilter2(this, galaga_chanl2_filt),
      _opAmpFilter3(this, galaga_chanl3_filt),
      _dac1(this, galaga_54xx_dac),
      _dac2(this, galaga_54xx_dac),
      _dac3(this, galaga_54xx_dac),
      _input1(this, Emulator::AudioInput::InitialData(1.0, 0.0, 0)),
      _input2(this, Emulator::AudioInput::InitialData(1.0, 0.0, 0)),
      _input3(this, Emulator::AudioInput::InitialData(1.0, 0.0, 0)),
      _buffer(),
      _lastSampleIndex(0)
{
    initAudioGraph();
}

DiscreteAudio::~DiscreteAudio() {}

std::vector<int>& DiscreteAudio::update(int64_t currentTime)
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

void DiscreteAudio::updateToCurrentTime()
{
    updateToTime(_galagaMachine.get_CurrentTime());
}

int DiscreteAudio::updateToTime(int64_t currentTime)
{
    int64_t currentSampleIndex = currentTime * SampleRate / 1000000;
    int samples = (int)(currentSampleIndex - _lastSampleIndex);
    if (samples <= 0)
    {
        return 0;
    }
    // else if (samples > 1024) {
    //	samples = 1024;
    //}

    auto output = generateSamples(samples);

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

void DiscreteAudio::initAudioGraph()
{
    _mixer.connect({[]() { return 1.0; },  // enabled
                    [this]() { return _opAmpFilter1.getOutput(); },
                    [this]() { return _opAmpFilter2.getOutput(); },
                    [this]() { return _opAmpFilter3.getOutput(); }});

    _opAmpFilter1.connect(
        {[]() { return 1.0; }, [this]() { return _dac1.getOutput(); },
         []() { return 0.0; },
         []() { return DISC_OP_AMP_FILTER_IS_BAND_PASS_1M; }});
    _opAmpFilter2.connect(
        {[]() { return 1.0; }, [this]() { return _dac2.getOutput(); },
         []() { return 0.0; },
         []() { return DISC_OP_AMP_FILTER_IS_BAND_PASS_1M; }});
    _opAmpFilter3.connect(
        {[]() { return 1.0; }, [this]() { return _dac3.getOutput(); },
         []() { return 0.0; },
         []() { return DISC_OP_AMP_FILTER_IS_BAND_PASS_1M; }});

    _dac1.connect(
        {[this]() { return _input1.getOutput(); }, []() { return 4.0; }});
    _dac2.connect(
        {[this]() { return _input2.getOutput(); }, []() { return 4.0; }});
    _dac3.connect(
        {[this]() { return _input3.getOutput(); }, []() { return 4.0; }});

    _input1.reset();
    _input2.reset();
    _input3.reset();
    _dac1.reset();
    _dac2.reset();
    _dac3.reset();
    _opAmpFilter1.reset();
    _opAmpFilter2.reset();
    _opAmpFilter3.reset();
    _mixer.reset();
}

void DiscreteAudio::process(uint32_t samplesPerUpdate,
                            std::vector<int>& samples)
{
    for (uint32_t i = 0; i < samplesPerUpdate; i++)
    {
        _dac1.step();
        _dac2.step();
        _dac3.step();
        _opAmpFilter1.step();
        _opAmpFilter2.step();
        _opAmpFilter3.step();
        _mixer.step();

        samples[i] = static_cast<int>(_mixer.getOutput());
    }
}

std::vector<int> DiscreteAudio::generateSamples(int samples)
{
    std::vector<int> output;
    output.resize(samples);
    process(samples, output);
    return output;
}

}  // namespace Galaga