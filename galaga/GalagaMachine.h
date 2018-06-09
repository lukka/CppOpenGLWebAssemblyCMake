#pragma once

#if defined(EMSCRIPTEN)
#include <emscripten/bind.h>
#endif

#include <functional>
#include <map>
#include <string>
#include <vector>
#include "Audio.h"
#include "DiscreteAudio.h"
#include "Frame.h"
#include "GalagaIOHandler.h"
#include "GalagaRoms.h"
#include "NamcoSound.h"
#include "ScreenDevice.h"
#include "emulator/Timer.h"
#include "emulator/emulator.h"


namespace Galaga
{
class Cpu2;
class Cpu3;
class MainCpu;
class Namco06;
class ScreenDevice;
class IN0LPort;
class IN0HPort;
class IN1LPort;
class IN1HPort;
class DSWAPort;
class DSWBPort;

class GalagaMachine
{
   public:
    static const double ClockFrequency;
    static const uint32_t AudioSampleRate;

    enum CabinetType
    {
        Upright,
        Cocktail
    };

   private:
    static const uint32_t Quantum = 167;  // us

    enum Difficulty
    {
        Medium = 0,
        Hard,
        Hardest,
        Easy,
    };

    enum Coinage
    {
        FreePlay = 0,
        TwoCoinsThreeCredits,
        ThreeCoinsOneCredit,
        OneCoinTwoCredits,
        FourCoinsOneCredit,
        OneCoinThreeCredits,
        TwoCoinsOneCredit,
        OneCoinOneCredit
    };

    enum InitialLives
    {
        Two = 0,
        Four,
        Three,
        Five
    };

    enum BonusLives
    {
        None = 0,
        OnetoFour_30K_100K_Every100K__Five_30K_and_100K_only,
        OnetoFour_20K_70K_Every70K__Five_30K_120K_Every120K,
        OnetoFour_20K_and_60K_only__Five_30K_and_150K_only,
        OnetoFour_20K_60K_Every60K__Five_20K_100K_Every100K,
        OnetoFour_30K_120K_Every120K__Five_30K_and_120K_only,
        OnetoFour_20K_80K_Every80K__Five_30K_150K_Every150K,
        OnetoFour_30K_and_80K_only__Five_30K_only,
    };

   public:
    GalagaMachine();
    virtual ~GalagaMachine();

    const ByteArray &ReadRom(const std::string &rom);
    Emulator::IInputPort *GetPortByTag(const std::string &tag);

    void Out0PortWrite(uint8_t data);
    void Out1PortWrite(uint8_t data);

    uint8_t ReadDsw(int offset);
    void WriteLatch(int offset, uint8_t value);

    void OnVblankStartCallback();

    std::vector<int> GetAudio() { return audio->GetAudio(); }

    void Run(double us, Frame &videoFrame, AudioFrame &audioFrame);

    void set_InsertCoin(bool value);
    void set_Start1Player(bool value);
    void set_Start2Player(bool value);
    void set_MoveLeft(bool value);
    void set_MoveRight(bool value);
    void set_Button1(bool value);

    Emulator::AudioInput &getAudioInput(int index)
    {
        return discreteAudio->getAudioInput(index);
    }
    NamcoSound &getNamcoSound() { return *namcoSound; }
    long get_CurrentTime() { return scheduler.get_CurrentTime(); }

    void Draw(Frame &frame);
    void GetAudioSamples(AudioFrame &audioFrame);

    void SetProgramCounterListener(ProgramCounterListener &pcListener);

   private:
    void OnCpu3InterruptCallback(int64_t param);
    void RegisterVblankCallback(Action callback);
    void Reset();
    void ResetLatch();

   public:
    Emulator::Scheduler scheduler;
    Namco06 *namco06xx;
    ScreenDevice *screenDevice;
    DiscreteAudio *discreteAudio;
    NamcoSound *namcoSound;

   private:
    GalagaRoms romManager;
    uint8_t *ram;
    GalagaIOHandler ioHandler;
    MainCpu *mainCpu;
    Cpu2 *cpu2;
    Cpu3 *cpu3;
    Emulator::CTimer *cpu3InterruptCallbackTimer;
    Audio *audio;
    std::vector<Action> vblankCallbacks;
    int customMod;
    friend class Namco06;

    // ports
    typedef std::map<std::string, Emulator::IInputPort *> MachinePorts;
    MachinePorts machinePorts;
    IN0LPort *in0lPort;
    IN0HPort *in0hPort;
    IN1LPort *in1lPort;
    IN1HPort *in1hPort;
    DSWAPort *dswaPort;
    DSWBPort *dswbPort;

   public:
    // settings
    bool twoPlayersGame;
    Difficulty difficulty;
    bool demoSound;
    bool freeze;
    bool rackTest;
    CabinetType cabinetType;
    Coinage coinage;
    InitialLives initialLives;
    BonusLives bonusLives;
};

}  // namespace Galaga
