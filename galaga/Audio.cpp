#include "Audio.h"
#include "GalagaMachine.h"

namespace Galaga
{
Audio::Audio(GalagaMachine& galagaMachine)
    : _galagaMachine(galagaMachine), _buffer()
{
    _timer = new Emulator::CTimer(
        "UpdateSound", galagaMachine.scheduler,
        [this](int64_t, int64_t currentTime) { update(currentTime); });
    _timer->Start(/*21334*/ 20000, true);  // 50HZ = 20ms
}

Audio::~Audio() { delete _timer; }

void Audio::update(int64_t currentTime)
{
    auto& namcoSamples = _galagaMachine.namcoSound->update(currentTime);
    auto& discreteSamples = _galagaMachine.discreteAudio->update(currentTime);

    size_t size = std::max(namcoSamples.size() / 4, discreteSamples.size());

    // assert(namcoSamples.size() == 4 * discreteSamples.size());
    if (discreteSamples.size() < size)
    {
        discreteSamples.resize(size);
    }
    if (namcoSamples.size() < size * 4)
    {
        namcoSamples.resize(size * 4);
    }

    std::vector<int> samples;
    samples.resize(size);
    for (size_t i = 0; i < namcoSamples.size(); i++)
    {
        samples[i / 4] += namcoSamples[i];
    }

    for (size_t i = 0; i < size; i++)
    {
        samples[i] =
            (samples[i] + 7) / 8 + (((discreteSamples[i] * 230) / 256) + 1) / 2;
    }

    _buffer.insert(_buffer.end(), samples.begin(), samples.end());

    namcoSamples.clear();
    discreteSamples.clear();
}
}  // namespace Galaga