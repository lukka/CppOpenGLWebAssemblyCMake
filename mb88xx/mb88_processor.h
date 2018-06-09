#pragma once

#include <cassert>
#include <exception>
#include <string>
#include "emulator/DeviceInput.h"
#include "emulator/Timer.h"
#include "emulator/emulator.h"


#define EA get_EA()

namespace Mb88xx
{
enum Port
{
    MB88_PORTK = 0,  // input only, 4 bits
    MB88_PORTO,      // output only, PLA function output
    MB88_PORTP,      // 4 bits
    MB88_PORTR0,     // R0-R3, 4 bits
    MB88_PORTR1,     // R4-R7, 4 bits
    MB88_PORTR2,     // R8-R11, 4 bits
    MB88_PORTR3,     // R12-R15, 4 bits
    MB88_PORTSI      // SI, 1 bit
};

class Status
{
   public:
    uint8_t PC;      // Program Counter: 6 bits
    uint8_t PA;      // Page Address: 4 bits
    uint16_t SP[4];  // Stack is 4*10 bit addresses deep, but we also use 3 top
                     // bits per address to store flags during irq
    uint8_t SI;      // Stack index: 2 bits
    uint8_t A;       // Accumulator: 4 bits
    uint8_t X;       // Index X: 4 bits
    uint8_t Y;       // Index Y: 4 bits
    bool st;         // State flag: 1 bit
    bool zf;         // Zero flag: 1 bit
    bool cf;         // Carry flag: 1 bit
    bool vf;         // Timer overflow flag: 1 bit
    bool sf;         // Serial Full/Empty flag: 1 bit
    bool nf;         // Interrupt flag: 1 bit

    // Peripheral Control
    uint8_t pio;  // Peripheral enable bits: 8 bits

    // Timer registers
    uint8_t TH;   // Timer High: 4 bits
    uint8_t TL;   // Timer Low: 4 bits
    uint8_t TP;   // Timer Prescale: 6 bits?
    uint8_t ctr;  // current external counter value

    // Serial registers
    uint8_t SB;        // Serial buffer: 4 bits
    uint16_t SBcount;  // number of bits received

    // IRQ handling
    uint8_t interruptPending;

    Status() { Reset(); }

    void Reset()
    {
        // zero registers and flags
        PC = 0;
        PA = 0;
        SP[0] = SP[1] = SP[2] = SP[3] = 0;
        SI = 0;
        A = 0;
        X = 0;
        Y = 0;
        st = true;  // start off with st=1
        zf = false;
        cf = false;
        vf = false;
        sf = false;
        nf = false;
        pio = 0;
        TH = 0;
        TL = 0;
        TP = 0;
        SB = 0;
        SBcount = 0;
        interruptPending = 0;
    }
};

class Processor : public Emulator::IProcessor
{
    typedef void (/*__thiscall*/ Processor::*Action)();

    static const int SERIAL_DISABLE_THRESH =
        1000;  // at this value, we give up driving the serial port
    static const int SERIAL_PRESCALE = 6;
    static const int TIMER_PRESCALE = 32;

    static const int INT_CAUSE_SERIAL = 0x01;
    static const int INT_CAUSE_TIMER = 0x02;
    static const int INT_CAUSE_EXTERNAL = 0x04;

    enum DecodeAction
    {
        None,
        Immediate
    };

    struct Instruction
    {
        uint8_t opcode;
        uint8_t immediate;
        int cycles;
        Action execute;
    };

    /// The program counter listener.
    ProgramCounterListener _programCounterListener;

   public:
    struct OpcodeInfo
    {
        OpcodeInfo(DecodeAction decodeAction, Action interpreterFunc)
            : decodeAction(decodeAction), interpreterFunc(interpreterFunc)
        {
        }

        DecodeAction decodeAction;
        Action interpreterFunc;
    };

   private:
    static OpcodeInfo fetchDecodeTable[];

    double clockFrequency;  // Hz
    bool halt;
    Status status;
    uint8_t* programRom;  // 1K
    uint8_t* dataRam;     // 64 uint8_ts
    uint8_t ramMask;
    Emulator::IOHandler& ioHandler;  // 8 uint8_ts
    uint8_t* pla;                    // PLA configuration
    uint8_t plaLength;
    bool aborted;
    int64_t cyclesLeft;
    int64_t cyclesInCurrentTimeslice;
    // Emulator::Cpu& cpu;
    Emulator::IrqDeviceInputLine irqLine;
    Emulator::IrqCallback irqCallback;
    Emulator::CTimer serialTimer;
    Instruction instr;  // for the sake of speed, reuse the same Instruction
    bool disabled;
    // Null instance of DeviceInput.
    Emulator::_NullDeviceInput nullDeviceInput;

    // FILE* fp;

   public:
    Processor(Emulator::Scheduler& scheduler, double clockFrequency,
              uint8_t* programRom, uint16_t programRamLength, uint8_t* dataRam,
              uint8_t dataRamLength, Emulator::IOHandler& ioHandler,
              uint8_t* pla, uint8_t plaLength, const std::string tag,
              Emulator::Cpu& cpu, bool disabled = false,
              Emulator::IrqCallback irqCallback = nullptr);
    ~Processor();

    // IProcessor
    // Execute 'cycles' T-states. Return number of T-states really executed
    virtual void Reset() { status.Reset(); }

    virtual int64_t get_ExecutionTimeInCurrentTimeslice() const
    {
        int64_t cycles = cyclesInCurrentTimeslice - cyclesLeft;
        return (int64_t)(1e6 * cycles / clockFrequency);
    }

    virtual int64_t Execute(int64_t us)  // microseconds
    {
        aborted = false;
        if (disabled)
        {
            return us;
        }

        cyclesInCurrentTimeslice = (int64_t)(clockFrequency * us / 1e6);
        cyclesLeft = cyclesInCurrentTimeslice;

        do
        {
            FetchDecode();

            // fprintf(fp, "\n%3d %3d %3d %3d %3d  ", get_PC() - (instr.cycles -
            // 1), this->instr.opcode, status.A, status.X, status.Y); for (int ii
            // = 0; ii < 64; ii+=4) fprintf(fp, "%02x %02x %02x %02x ",
            // dataRam[ii], dataRam[ii+1], dataRam[ii+2], dataRam[ii+3]);

            Action execInstruction = this->instr.execute;
            //		if (get_PC() - (instr.cycles - 1) == 42 && this->instr.opcode
            //== 13 && status.A == 0)
            //		{
            //				Sleep(0);
            //		}

            ((*this).*execInstruction)();

            if (aborted)
            {
                cyclesLeft = 0;
            }
            else
            {
                cyclesLeft -= instr.cycles;
            }
            UpdatePio(instr.cycles);
        } while (!halt && cyclesLeft > 0);

        return get_ExecutionTimeInCurrentTimeslice();
    }

    virtual void AbortTimeslice() { aborted = true; }

    virtual Emulator::IDeviceInputLine& get_IrqInputLine() { return irqLine; }
    virtual Emulator::IDeviceInputLine& get_ResetInputLine()
    {
        return nullDeviceInput;
    }
    virtual Emulator::IDeviceInputLine& get_NmiInputLine()
    {
        return nullDeviceInput;
    }

    virtual std::vector<Emulator::IDeviceInputLine*> get_InputLines()
    {
        std::vector<Emulator::IDeviceInputLine*> result;
        result.push_back(&irqLine);
        return result;
    }

   protected:
    uint16_t get_PC() const { return (uint16_t)((status.PA << 6) + status.PC); }

    uint8_t get_EA() { return (uint8_t)((status.X << 4) + status.Y); }

    void IncPC()
    {
        status.PC++;
        if (status.PC >= 0x40)
        {
            status.PC = 0;
            status.PA++;
        }
    }

    uint8_t ReadRom(uint16_t addr)
    {
        assert(addr < 0x0400);
        return programRom[addr];
    }

    uint8_t ReadRam(uint8_t addr)
    {
        addr &= ramMask;
        return dataRam[addr];
    }

    void WriteRam(uint8_t addr, uint8_t value)
    {
        addr &= ramMask;
        dataRam[addr] = value;
    }

    uint8_t ReadPort(Port port)
    {
        return ioHandler.Read((uint16_t)(port - Port::MB88_PORTK));
    }

    void WritePort(Port port, uint8_t value)
    {
        ioHandler.Write((uint16_t)(port - Port::MB88_PORTK), value);
    }

    void IncrementTimer();
    uint8_t GetPla(int inA, int inB);
    void UpdatePio(int cycles);
    void UpdatePioEnable(uint8_t newPio);
    Processor::Instruction& FetchDecode();
    void OnIrqLineSet(Emulator::LineState state);

   public:
    void SerialTimerCallback(int64_t param);

   private:
    // Instructions

    void Nop()
    {
        // nop ZCS:...
        status.st = true;
    }

    void Adc()
    {
        // adc ZCS:xxx
        uint8_t m = (uint8_t)(ReadRam(EA) + status.A + (status.cf ? 1 : 0));
        status.st = ((m & 0x10) == 0);
        status.cf = !status.st;
        status.A = (uint8_t)(m & 0x0f);
        status.zf = (0 == status.A);
    }

    void Add_imm()
    {
        // ai ZCS:xxx
        uint8_t val = (uint8_t)((instr.opcode & 0x0f) + status.A);
        status.st = ((val & 0x10) == 0);
        status.cf = !status.st;
        status.A = (uint8_t)(val & 0x0f);
        status.zf = (0 == status.A);
    }

    void And()
    {
        // and ZCS:x.x
        status.A &= ReadRam(EA);
        status.zf = (0 == status.A);
        status.st = !status.zf;
    }

    void Call_imm()
    {
        // call imm ZCS:..x
        if (status.st)
        {
            status.SP[status.SI] = get_PC();
            status.SI = (uint8_t)((status.SI + 1) & 0x03);
            status.PC = (uint8_t)(instr.immediate & 0x3f);
            status.PA = (uint8_t)(((instr.opcode & 0x07) << 2) |
                                  (instr.immediate >> 6));
        }
        status.st = true;
    }

    void Cp_a_mem()
    {
        // c ZCS:xxx
        uint8_t val = (uint8_t)(ReadRam(EA) - status.A);
        status.cf = ((val & 0x10) != 0x00);
        val &= 0x0f;
        status.st = (0 != val);
        status.zf = !status.st;
    }

    void Cp_a_imm()
    {
        // ci ZCS:xxx
        uint8_t val = (uint8_t)((instr.opcode & 0x0f) - status.A);
        status.cf = ((val & 0x10) != 0x00);
        val &= 0x0f;
        status.st = (0 != val);
        status.zf = !status.st;
    }

    void Cp_y_imm()
    {
        // cyi ZCS:xxx
        uint8_t val = (uint8_t)((instr.opcode & 0x0f) - status.Y);
        status.cf = ((val & 0x10) != 0x00);
        val &= 0x0f;
        status.st = (0 != val);
        status.zf = !status.st;
    }

    void Daa()
    {
        // daa ZCS:.xx
        if (status.cf || status.A > 9)
        {
            status.A += 6;
        }
        status.st = ((status.A & 0x10) == 0);
        status.cf = !status.st;
        status.A = (uint8_t)(status.A & 0x0f);
    }

    void Das()
    {
        // das ZCS:.xx
        if (status.cf || status.A > 9)
        {
            status.A += 10;
        }
        status.st = ((status.A & 0x10) == 0);
        status.cf = !status.st;
        status.A = (uint8_t)(status.A & 0x0f);
    }

    void Dec_mem()
    {
        // dcm ZCS:x.x
        uint8_t ea = EA;
        uint8_t m = ReadRam(ea);
        status.st = (m != 0x00);
        m = (uint8_t)((m - 1) & 0x0f);
        status.zf = (0 == m);
        WriteRam(ea, m);
    }

    void Dec_y()
    {
        // dcy ZCS:..x
        status.st = (status.Y != 0x00);
        status.Y = (uint8_t)((status.Y - 1) & 0x0f);
        status.zf = (0 == status.Y);
    }

    void Dis()
    {
        // dis imm ZCS:...
        UpdatePioEnable((uint8_t)(status.pio & ~instr.immediate));
        status.st = true;
    }

    void En()
    {
        // en imm ZCS:...
        UpdatePioEnable((uint8_t)(status.pio | instr.immediate));
        status.st = true;
    }

    void Ex_a_d()
    {
        // xd ZCS:x..
        uint8_t m = ReadRam((uint8_t)(instr.opcode & 0x03));
        WriteRam((uint8_t)(instr.opcode & 0x03), status.A);
        status.A = m;
        status.zf = (0 == status.A);
        status.st = true;
    }

    void Ex_a_mem()
    {
        // x ZCS:x..
        uint8_t ea = EA;
        uint8_t m = ReadRam(ea);
        WriteRam(ea, status.A);
        status.A = m;
        status.zf = (0 == status.A);
        status.st = true;
    }

    void Ex_a_x()
    {
        // xx ZCS:x..
        uint8_t m = status.X;
        status.X = status.A;
        status.A = m;
        status.zf = (0 == status.A);
        status.st = true;
    }

    void Ex_y_d()
    {
        // xyd ZCS:x..
        uint8_t m = ReadRam((uint8_t)((instr.opcode & 0x03) + 4));
        WriteRam((uint8_t)((instr.opcode & 0x03) + 4), status.Y);
        status.Y = m;
        status.zf = (0 == status.Y);
        status.st = true;
    }

    void In_k()
    {
        // inK ZCS:x..
        status.A = (uint8_t)(ReadPort(Port::MB88_PORTK) & 0x0f);
        status.zf = (0 == status.A);
        status.st = true;
    }

    void In_r()
    {
        // inK ZCS:x..
        status.A = (uint8_t)(
            ReadPort((Port)(Port::MB88_PORTR0 + (status.Y & 3))) & 0x0f);
        status.zf = (0 == status.A);
        status.st = true;
    }

    void Inc_mem()
    {
        // icm ZCS:x.x
        uint8_t ea = EA;
        uint8_t m = ReadRam(ea);
        status.st = (m < 0x0f);
        m = (uint8_t)((1 + m) & 0x0f);
        status.zf = (0 == m);
        WriteRam(ea, m);
    }

    void Inc_y()
    {
        // icy ZCS:x.x
        status.st = (status.Y < 0x0f);
        status.Y = (uint8_t)((1 + status.Y) & 0x0f);
        status.zf = (0 == status.Y);
    }

    void Jmp()
    {
        // jmp ZCS:..x
        if (status.st)
        {
            status.PC = (uint8_t)(instr.opcode & 0x3f);
        }
        status.st = true;
    }

    void Jpa()
    {
        // jpa imm ZCS:..x
        status.PA = (uint8_t)(instr.immediate & 0x1f);
        status.PC = (uint8_t)(status.A * 4);
        status.st = true;
    }

    void Jp_imm()
    {
        // jpl imm ZCS:..x
        if (status.st)
        {
            status.PC = (uint8_t)(instr.immediate & 0x3f);
            status.PA = (uint8_t)(((instr.opcode & 0x07) << 2) |
                                  (instr.immediate >> 6));
        }
        status.st = true;
    }

    void Ld_a()
    {
        // l ZCS:x..
        status.A = ReadRam(EA);
        status.zf = (0 == status.A);
        status.st = true;
    }

    void Ld_a_imm()
    {
        // li ZCS:x..
        status.A = (uint8_t)(instr.opcode & 0x0f);
        status.zf = (0 == status.A);
        status.st = true;
    }

    void Ld_s()
    {
        // ls ZCS:x..
        status.SB = ReadRam(EA);
        status.zf = (0 == status.SB);
        status.st = true;
    }

    void Ld_x_imm()
    {
        // lxi ZCS:x..
        status.X = (uint8_t)(instr.opcode & 0x07);
        status.zf = (0 == status.X);
        status.st = true;
    }

    void Ld_y_imm()
    {
        // lyi ZCS:x..
        status.Y = (uint8_t)(instr.opcode & 0x0f);
        status.zf = (0 == status.Y);
        status.st = true;
    }

    void Neg()
    {
        // neg ZCS: ..x
        status.A = (uint8_t)(((~status.A) + 1) & 0x0f);
        status.st = (0 != status.A);
    }

    void Or()
    {
        // or ZCS:x.x
        status.A |= ReadRam(EA);
        status.zf = (0 == status.A);
        status.st = !status.zf;
    }

    void Out_O()
    {
        // outO ZCS:...
        WritePort(Port::MB88_PORTO, GetPla(status.A, status.cf ? 1 : 0));
        status.st = true;
    }

    void Out_P()
    {
        // outP ZCS:...
        WritePort(Port::MB88_PORTP, status.A);
        status.st = true;
    }

    void Out_R()
    {
        // outR ZCS:...
        uint8_t val = status.Y;
        WritePort((Port)(Port::MB88_PORTR0 + (val & 3)), status.A);
        status.st = true;
    }

    void RotLeft()
    {
        // rol ZCS:xxx
        status.A <<= 1;
        status.A |= (uint8_t)(status.cf ? 1 : 0);
        status.st = ((status.A & 0x10) == 0);
        status.cf = !status.st;
        status.A &= 0x0f;
        status.zf = (0 == status.A);
    }

    void RotRight()
    {
        // ror ZCS:xxx
        status.A |= (uint8_t)(status.cf ? 0x10 : 0x00);
        status.st = ((status.A & 0x01) == 0);
        status.cf = !status.st;
        status.A >>= 1;
        status.A &= 0x0f;
        status.zf = (0 == status.A);
    }

    void RstBit()
    {
        // rbit ZCS:...
        uint8_t val = ReadRam(EA);
        WriteRam(EA, (uint8_t)(val & ~(1 << (instr.opcode & 3))));
        status.st = true;
    }

    void RstBit_d()
    {
        // rstD ZCS:...
        uint8_t val =
            (uint8_t)(ReadPort(Port::MB88_PORTR0) & ~(1 << (instr.opcode & 3)));
        WritePort(Port::MB88_PORTR0, val);
        status.st = true;
    }

    void Rst_c()
    {
        // rstc ZCS:.xx
        status.cf = false;
        status.st = true;
    }

    void Rst_r()
    {
        // rstR ZCS:...
        uint8_t val = ReadPort((Port)(Port::MB88_PORTR0 + (status.Y / 4)));
        WritePort((Port)(Port::MB88_PORTR0 + (status.Y / 4)),
                  (uint8_t)(val & ~(1 << (status.Y % 4))));
        status.st = true;
    }

    void Rti()
    {
        // rti ZCS:...
        // return from interrupt - restore address and saved state flags on the
        // top bits of the stack
        status.SI = (uint8_t)((status.SI - 1) & 0x03);
        status.PC = (uint8_t)(status.SP[status.SI] & 0x3f);
        status.PA = (uint8_t)((status.SP[status.SI] >> 6) & 0x1f);
        status.st = 0 != ((status.SP[status.SI] >> 13) & 0x01);
        status.zf = 0 != ((status.SP[status.SI] >> 14) & 0x01);
        status.cf = 0 != ((status.SP[status.SI] >> 15) & 0x01);
    }

    void Rts()
    {
        // rts ZCS:...
        // return from subroutine
        status.SI = (uint8_t)((status.SI - 1) & 3);
        status.PC = (uint8_t)(status.SP[status.SI] & 0x3f);
        status.PA = (uint8_t)((status.SP[status.SI] >> 6) & 0x1f);
        status.st = true;
    }

    void Sbc()
    {
        // sbc ZCS:xxx
        uint8_t m = (uint8_t)(ReadRam(EA) - status.A - (status.cf ? 1 : 0));
        status.st = ((m & 0x10) == 0);
        status.cf = !status.st;
        status.A = (uint8_t)(m & 0x0f);
        status.zf = (0 == status.A);
    }

    void SetBit()
    {
        // sbit ZCS:...
        uint8_t val = ReadRam(EA);
        WriteRam(EA, (uint8_t)(val | (1 << (instr.opcode & 3))));
        status.st = true;
    }

    void SetBit_d()
    {
        // setD ZCS:...
        uint8_t val = (uint8_t)(ReadPort(Port::MB88_PORTR0) |
                                (1 << (instr.opcode & 0x03)));
        WritePort(Port::MB88_PORTR0, val);
        status.st = true;
    }

    void Set_c()
    {
        // setc ZCS:.xx
        status.cf = true;
        status.st = true;
    }

    void Set_r()
    {
        // setR ZCS:...
        uint8_t val = ReadPort((Port)(Port::MB88_PORTR0 + (status.Y / 4)));
        WritePort((Port)(Port::MB88_PORTR0 + (status.Y / 4)),
                  (uint8_t)(val | (1 << (status.Y % 4))));
        status.st = true;
    }

    void St_a()
    {
        // st ZCS:x..
        WriteRam(EA, status.A);
        status.st = true;
    }

    void St_dec()
    {
        // stdc ZCS:x.x
        WriteRam(EA, status.A);
        status.st = (status.Y != 0x00);
        status.Y = (uint8_t)((status.Y - 1) & 0x0f);
        status.zf = (0 == status.Y);
    }

    void St_inc()
    {
        // stic ZCS:x.x
        WriteRam(EA, status.A);
        status.st = (status.Y < 0x0f);
        status.Y = (uint8_t)((1 + status.Y) & 0x0f);
        status.zf = (0 == status.Y);
    }

    void St_s()
    {
        // sts ZCS:x..
        WriteRam(EA, status.SB);
        status.zf = (0 == status.SB);
        status.st = true;
    }

    void T_a_s()
    {
        // tas ZCS:...
        status.SB = status.A;
        status.st = true;
    }

    void T_a_th()
    {
        // tath ZCS:...
        status.TH = status.A;
        status.st = true;
    }

    void T_a_tl()
    {
        // tath ZCS:...
        status.TL = status.A;
        status.st = true;
    }

    void T_a_y()
    {
        // tay ZCS:...
        status.Y = status.A;
        status.st = true;
    }

    void T_s_a()
    {
        // tsa ZCS:x..
        status.A = status.SB;
        status.zf = (0 == status.A);
        status.st = true;
    }

    void T_th_a()
    {
        // ttha ZCS:x..
        status.A = status.TH;
        status.zf = (0 == status.A);
        status.st = true;
    }

    void T_tl_a()
    {
        // ttla ZCS:x..
        status.A = status.TL;
        status.zf = (0 == status.A);
        status.st = true;
    }

    void T_y_a()
    {
        // tya ZCS:x..
        status.A = status.Y;
        status.zf = (0 == status.A);
        status.st = true;
    }

    void TstBit()
    {
        // tbit ZCS:...
        uint8_t val = ReadRam(EA);
        status.st = (val & (1 << (instr.opcode & 3))) == 0;
    }

    void TstBit_a()
    {
        // tba ZCS:..x
        status.st = (status.A & (1 << (instr.opcode & 3))) == 0;
    }

    void TstBit_d()
    {
        // tstD ZCS:..x
        uint8_t val = ReadPort(Port::MB88_PORTR2);
        status.st = (val & (1 << (instr.opcode & 3))) == 0;
    }

    void Tst_c()
    {
        // tstc ZCS:..x
        status.st = !status.cf;
    }

    void Tst_i()
    {
        // tsti ZCS:..x
        status.st = !status.nf;
    }

    void Tst_r()
    {
        // tstr ZCS:..x
        uint8_t val = ReadPort((Port)(Port::MB88_PORTR0 + (status.Y / 4)));
        status.st = (val & (1 << (status.Y % 4))) != 0;
    }

    void Tst_s()
    {
        // tsts ZCS:..x
        uint8_t val = ReadPort((Port)(Port::MB88_PORTR0 + (status.Y / 4)));
        status.st = (val & (1 << (status.Y % 4))) != 0;
        status.st = !status.sf;
        if (status.sf)
        {
            // re-enable the timer if we disabled it previously
            if (status.SBcount >= SERIAL_DISABLE_THRESH)
            {
                // todo
                // status.serial->adjust(attotime::from_hz(status.device->clock()
                // / SERIAL_PRESCALE), 0,
                //    attotime::from_hz(status.device->clock() /
                //    SERIAL_PRESCALE));
            }
            status.SBcount = 0;
        }
        status.sf = false;
    }

    void Tst_v()
    {
        // tstr tstv ZCS:..x
        status.st = !status.vf;
        status.vf = false;
    }

    void Tst_z()
    {
        // tstz ZCS:..x
        status.st = !status.zf;
    }

    void Xor()
    {
        // eor ZCS:x.x
        status.A ^= ReadRam(EA);
        status.zf = (0 == status.A);
        status.st = !status.zf;
    }

    void SetProgramCounterListener(ProgramCounterListener& pcListener)
    {
        _programCounterListener = pcListener;
    }
};

}  // namespace Mb88xx