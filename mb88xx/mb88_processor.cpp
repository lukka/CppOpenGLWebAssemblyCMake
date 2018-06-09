#include "mb88_processor.h"

namespace Mb88xx
{
    Processor::OpcodeInfo Processor::fetchDecodeTable[] =
    {
        OpcodeInfo(DecodeAction::None, &Processor::Nop), // 0x00
        OpcodeInfo(DecodeAction::None, &Processor::Out_O), // 0x01
        OpcodeInfo(DecodeAction::None, &Processor::Out_P), // 0x02
        OpcodeInfo(DecodeAction::None, &Processor::Out_R), // 0x03
        OpcodeInfo(DecodeAction::None, &Processor::T_a_y), // 0x04
        OpcodeInfo(DecodeAction::None, &Processor::T_a_th), // 0x05
        OpcodeInfo(DecodeAction::None, &Processor::T_a_tl), // 0x06
        OpcodeInfo(DecodeAction::None, &Processor::T_a_s), // 0x07
        OpcodeInfo(DecodeAction::None, &Processor::Inc_y), // 0x08
        OpcodeInfo(DecodeAction::None, &Processor::Inc_mem), // 0x09
        OpcodeInfo(DecodeAction::None, &Processor::St_inc), // 0x0a
        OpcodeInfo(DecodeAction::None, &Processor::Ex_a_mem), // 0x0b
        OpcodeInfo(DecodeAction::None, &Processor::RotLeft), // 0x0c
        OpcodeInfo(DecodeAction::None, &Processor::Ld_a), // 0x0d
        OpcodeInfo(DecodeAction::None, &Processor::Adc), // 0x0e
        OpcodeInfo(DecodeAction::None, &Processor::And), // 0x0f

        OpcodeInfo(DecodeAction::None, &Processor::Daa), // 0x10
        OpcodeInfo(DecodeAction::None, &Processor::Das), // 0x11
        OpcodeInfo(DecodeAction::None, &Processor::In_k), // 0x12
        OpcodeInfo(DecodeAction::None, &Processor::In_r), // 0x13
        OpcodeInfo(DecodeAction::None, &Processor::T_y_a), // 0x14
        OpcodeInfo(DecodeAction::None, &Processor::T_th_a), // 0x15
        OpcodeInfo(DecodeAction::None, &Processor::T_tl_a), // 0x16
        OpcodeInfo(DecodeAction::None, &Processor::T_s_a), // 0x17
        OpcodeInfo(DecodeAction::None, &Processor::Dec_y), // 0x18
        OpcodeInfo(DecodeAction::None, &Processor::Dec_mem), // 0x19
        OpcodeInfo(DecodeAction::None, &Processor::St_dec), // 0x1a
        OpcodeInfo(DecodeAction::None, &Processor::Ex_a_x), // 0x1b
        OpcodeInfo(DecodeAction::None, &Processor::RotRight), // 0x1c
        OpcodeInfo(DecodeAction::None, &Processor::St_a), // 0x1d
        OpcodeInfo(DecodeAction::None, &Processor::Sbc), // 0x1e
        OpcodeInfo(DecodeAction::None, &Processor::Or), // 0x1f

        OpcodeInfo(DecodeAction::None, &Processor::Set_r), // 0x20
        OpcodeInfo(DecodeAction::None, &Processor::Set_c), // 0x21
        OpcodeInfo(DecodeAction::None, &Processor::Rst_r), // 0x22
        OpcodeInfo(DecodeAction::None, &Processor::Rst_c), // 0x23
        OpcodeInfo(DecodeAction::None, &Processor::Tst_r), // 0x24
        OpcodeInfo(DecodeAction::None, &Processor::Tst_i), // 0x25
        OpcodeInfo(DecodeAction::None, &Processor::Tst_v), // 0x26
        OpcodeInfo(DecodeAction::None, &Processor::Tst_s), // 0x27
        OpcodeInfo(DecodeAction::None, &Processor::Tst_c), // 0x28
        OpcodeInfo(DecodeAction::None, &Processor::Tst_z), // 0x29
        OpcodeInfo(DecodeAction::None, &Processor::St_s), // 0x2a
        OpcodeInfo(DecodeAction::None, &Processor::Ld_s), // 0x2b
        OpcodeInfo(DecodeAction::None, &Processor::Rts), // 0x2c
        OpcodeInfo(DecodeAction::None, &Processor::Neg), // 0x2d
        OpcodeInfo(DecodeAction::None, &Processor::Cp_a_mem), // 0x2e
        OpcodeInfo(DecodeAction::None, &Processor::Xor), // 0x2f

        OpcodeInfo(DecodeAction::None, &Processor::SetBit), // 0x30
        OpcodeInfo(DecodeAction::None, &Processor::SetBit), // 0x31
        OpcodeInfo(DecodeAction::None, &Processor::SetBit), // 0x32
        OpcodeInfo(DecodeAction::None, &Processor::SetBit), // 0x33
        OpcodeInfo(DecodeAction::None, &Processor::RstBit), // 0x34
        OpcodeInfo(DecodeAction::None, &Processor::RstBit), // 0x35
        OpcodeInfo(DecodeAction::None, &Processor::RstBit), // 0x36
        OpcodeInfo(DecodeAction::None, &Processor::RstBit), // 0x37
        OpcodeInfo(DecodeAction::None, &Processor::TstBit), // 0x38
        OpcodeInfo(DecodeAction::None, &Processor::TstBit), // 0x39
        OpcodeInfo(DecodeAction::None, &Processor::TstBit), // 0x3a
        OpcodeInfo(DecodeAction::None, &Processor::TstBit), // 0x3b
        OpcodeInfo(DecodeAction::None, &Processor::Rti), // 0x3c
        OpcodeInfo(DecodeAction::Immediate, &Processor::Jpa), // 0x3d
        OpcodeInfo(DecodeAction::Immediate, &Processor::En), // 0x3e
        OpcodeInfo(DecodeAction::Immediate, &Processor::Dis), // 0x3f

        OpcodeInfo(DecodeAction::None, &Processor::SetBit_d), // 0x40
        OpcodeInfo(DecodeAction::None, &Processor::SetBit_d), // 0x41
        OpcodeInfo(DecodeAction::None, &Processor::SetBit_d), // 0x42
        OpcodeInfo(DecodeAction::None, &Processor::SetBit_d), // 0x43
        OpcodeInfo(DecodeAction::None, &Processor::RstBit_d), // 0x44
        OpcodeInfo(DecodeAction::None, &Processor::RstBit_d), // 0x45
        OpcodeInfo(DecodeAction::None, &Processor::RstBit_d), // 0x46
        OpcodeInfo(DecodeAction::None, &Processor::RstBit_d), // 0x47
        OpcodeInfo(DecodeAction::None, &Processor::TstBit_d), // 0x48
        OpcodeInfo(DecodeAction::None, &Processor::TstBit_d), // 0x49
        OpcodeInfo(DecodeAction::None, &Processor::TstBit_d), // 0x4a
        OpcodeInfo(DecodeAction::None, &Processor::TstBit_d), // 0x4b
        OpcodeInfo(DecodeAction::None, &Processor::TstBit_a), // 0x4c
        OpcodeInfo(DecodeAction::None, &Processor::TstBit_a), // 0x4d
        OpcodeInfo(DecodeAction::None, &Processor::TstBit_a), // 0x4e
        OpcodeInfo(DecodeAction::None, &Processor::TstBit_a), // 0x4f

        OpcodeInfo(DecodeAction::None, &Processor::Ex_a_d), // 0x50
        OpcodeInfo(DecodeAction::None, &Processor::Ex_a_d), // 0x51
        OpcodeInfo(DecodeAction::None, &Processor::Ex_a_d), // 0x52
        OpcodeInfo(DecodeAction::None, &Processor::Ex_a_d), // 0x53
        OpcodeInfo(DecodeAction::None, &Processor::Ex_y_d), // 0x54
        OpcodeInfo(DecodeAction::None, &Processor::Ex_y_d), // 0x55
        OpcodeInfo(DecodeAction::None, &Processor::Ex_y_d), // 0x56
        OpcodeInfo(DecodeAction::None, &Processor::Ex_y_d), // 0x57
        OpcodeInfo(DecodeAction::None, &Processor::Ld_x_imm), // 0x58
        OpcodeInfo(DecodeAction::None, &Processor::Ld_x_imm), // 0x59
        OpcodeInfo(DecodeAction::None, &Processor::Ld_x_imm), // 0x5a
        OpcodeInfo(DecodeAction::None, &Processor::Ld_x_imm), // 0x5b
        OpcodeInfo(DecodeAction::None, &Processor::Ld_x_imm), // 0x5c
        OpcodeInfo(DecodeAction::None, &Processor::Ld_x_imm), // 0x5d
        OpcodeInfo(DecodeAction::None, &Processor::Ld_x_imm), // 0x5e
        OpcodeInfo(DecodeAction::None, &Processor::Ld_x_imm), // 0x5f

        OpcodeInfo(DecodeAction::Immediate, &Processor::Call_imm), // 0x60
        OpcodeInfo(DecodeAction::Immediate, &Processor::Call_imm), // 0x61
        OpcodeInfo(DecodeAction::Immediate, &Processor::Call_imm), // 0x62
        OpcodeInfo(DecodeAction::Immediate, &Processor::Call_imm), // 0x63
        OpcodeInfo(DecodeAction::Immediate, &Processor::Call_imm), // 0x64
        OpcodeInfo(DecodeAction::Immediate, &Processor::Call_imm), // 0x65
        OpcodeInfo(DecodeAction::Immediate, &Processor::Call_imm), // 0x66
        OpcodeInfo(DecodeAction::Immediate, &Processor::Call_imm), // 0x67
        OpcodeInfo(DecodeAction::Immediate, &Processor::Jp_imm), // 0x68
        OpcodeInfo(DecodeAction::Immediate, &Processor::Jp_imm), // 0x69
        OpcodeInfo(DecodeAction::Immediate, &Processor::Jp_imm), // 0x6a
        OpcodeInfo(DecodeAction::Immediate, &Processor::Jp_imm), // 0x6b
        OpcodeInfo(DecodeAction::Immediate, &Processor::Jp_imm), // 0x6c
        OpcodeInfo(DecodeAction::Immediate, &Processor::Jp_imm), // 0x6d
        OpcodeInfo(DecodeAction::Immediate, &Processor::Jp_imm), // 0x6e
        OpcodeInfo(DecodeAction::Immediate, &Processor::Jp_imm), // 0x6f

        OpcodeInfo(DecodeAction::None, &Processor::Add_imm), // 0x70
        OpcodeInfo(DecodeAction::None, &Processor::Add_imm), // 0x71
        OpcodeInfo(DecodeAction::None, &Processor::Add_imm), // 0x72
        OpcodeInfo(DecodeAction::None, &Processor::Add_imm), // 0x73
        OpcodeInfo(DecodeAction::None, &Processor::Add_imm), // 0x74
        OpcodeInfo(DecodeAction::None, &Processor::Add_imm), // 0x75
        OpcodeInfo(DecodeAction::None, &Processor::Add_imm), // 0x76
        OpcodeInfo(DecodeAction::None, &Processor::Add_imm), // 0x77
        OpcodeInfo(DecodeAction::None, &Processor::Add_imm), // 0x78
        OpcodeInfo(DecodeAction::None, &Processor::Add_imm), // 0x79
        OpcodeInfo(DecodeAction::None, &Processor::Add_imm), // 0x7a
        OpcodeInfo(DecodeAction::None, &Processor::Add_imm), // 0x7b
        OpcodeInfo(DecodeAction::None, &Processor::Add_imm), // 0x7c
        OpcodeInfo(DecodeAction::None, &Processor::Add_imm), // 0x7d
        OpcodeInfo(DecodeAction::None, &Processor::Add_imm), // 0x7e
        OpcodeInfo(DecodeAction::None, &Processor::Add_imm), // 0x7f

        OpcodeInfo(DecodeAction::None, &Processor::Ld_y_imm), // 0x80
        OpcodeInfo(DecodeAction::None, &Processor::Ld_y_imm), // 0x81
        OpcodeInfo(DecodeAction::None, &Processor::Ld_y_imm), // 0x82
        OpcodeInfo(DecodeAction::None, &Processor::Ld_y_imm), // 0x83
        OpcodeInfo(DecodeAction::None, &Processor::Ld_y_imm), // 0x84
        OpcodeInfo(DecodeAction::None, &Processor::Ld_y_imm), // 0x85
        OpcodeInfo(DecodeAction::None, &Processor::Ld_y_imm), // 0x86
        OpcodeInfo(DecodeAction::None, &Processor::Ld_y_imm), // 0x87
        OpcodeInfo(DecodeAction::None, &Processor::Ld_y_imm), // 0x88
        OpcodeInfo(DecodeAction::None, &Processor::Ld_y_imm), // 0x89
        OpcodeInfo(DecodeAction::None, &Processor::Ld_y_imm), // 0x8a
        OpcodeInfo(DecodeAction::None, &Processor::Ld_y_imm), // 0x8b
        OpcodeInfo(DecodeAction::None, &Processor::Ld_y_imm), // 0x8c
        OpcodeInfo(DecodeAction::None, &Processor::Ld_y_imm), // 0x8d
        OpcodeInfo(DecodeAction::None, &Processor::Ld_y_imm), // 0x8e
        OpcodeInfo(DecodeAction::None, &Processor::Ld_y_imm), // 0x8f

        OpcodeInfo(DecodeAction::None, &Processor::Ld_a_imm), // 0x90
        OpcodeInfo(DecodeAction::None, &Processor::Ld_a_imm), // 0x91
        OpcodeInfo(DecodeAction::None, &Processor::Ld_a_imm), // 0x92
        OpcodeInfo(DecodeAction::None, &Processor::Ld_a_imm), // 0x93
        OpcodeInfo(DecodeAction::None, &Processor::Ld_a_imm), // 0x94
        OpcodeInfo(DecodeAction::None, &Processor::Ld_a_imm), // 0x95
        OpcodeInfo(DecodeAction::None, &Processor::Ld_a_imm), // 0x96
        OpcodeInfo(DecodeAction::None, &Processor::Ld_a_imm), // 0x97
        OpcodeInfo(DecodeAction::None, &Processor::Ld_a_imm), // 0x98
        OpcodeInfo(DecodeAction::None, &Processor::Ld_a_imm), // 0x99
        OpcodeInfo(DecodeAction::None, &Processor::Ld_a_imm), // 0x9a
        OpcodeInfo(DecodeAction::None, &Processor::Ld_a_imm), // 0x9b
        OpcodeInfo(DecodeAction::None, &Processor::Ld_a_imm), // 0x9c
        OpcodeInfo(DecodeAction::None, &Processor::Ld_a_imm), // 0x9d
        OpcodeInfo(DecodeAction::None, &Processor::Ld_a_imm), // 0x9e
        OpcodeInfo(DecodeAction::None, &Processor::Ld_a_imm), // 0x9f

        OpcodeInfo(DecodeAction::None, &Processor::Cp_y_imm), // 0xa0
        OpcodeInfo(DecodeAction::None, &Processor::Cp_y_imm), // 0xa1
        OpcodeInfo(DecodeAction::None, &Processor::Cp_y_imm), // 0xa2
        OpcodeInfo(DecodeAction::None, &Processor::Cp_y_imm), // 0xa3
        OpcodeInfo(DecodeAction::None, &Processor::Cp_y_imm), // 0xa4
        OpcodeInfo(DecodeAction::None, &Processor::Cp_y_imm), // 0xa5
        OpcodeInfo(DecodeAction::None, &Processor::Cp_y_imm), // 0xa6
        OpcodeInfo(DecodeAction::None, &Processor::Cp_y_imm), // 0xa7
        OpcodeInfo(DecodeAction::None, &Processor::Cp_y_imm), // 0xa8
        OpcodeInfo(DecodeAction::None, &Processor::Cp_y_imm), // 0xa9
        OpcodeInfo(DecodeAction::None, &Processor::Cp_y_imm), // 0xaa
        OpcodeInfo(DecodeAction::None, &Processor::Cp_y_imm), // 0xab
        OpcodeInfo(DecodeAction::None, &Processor::Cp_y_imm), // 0xac
        OpcodeInfo(DecodeAction::None, &Processor::Cp_y_imm), // 0xad
        OpcodeInfo(DecodeAction::None, &Processor::Cp_y_imm), // 0xae
        OpcodeInfo(DecodeAction::None, &Processor::Cp_y_imm), // 0xaf

        OpcodeInfo(DecodeAction::None, &Processor::Cp_a_imm), // 0xb0
        OpcodeInfo(DecodeAction::None, &Processor::Cp_a_imm), // 0xb1
        OpcodeInfo(DecodeAction::None, &Processor::Cp_a_imm), // 0xb2
        OpcodeInfo(DecodeAction::None, &Processor::Cp_a_imm), // 0xb3
        OpcodeInfo(DecodeAction::None, &Processor::Cp_a_imm), // 0xb4
        OpcodeInfo(DecodeAction::None, &Processor::Cp_a_imm), // 0xb5
        OpcodeInfo(DecodeAction::None, &Processor::Cp_a_imm), // 0xb6
        OpcodeInfo(DecodeAction::None, &Processor::Cp_a_imm), // 0xb7
        OpcodeInfo(DecodeAction::None, &Processor::Cp_a_imm), // 0xb8
        OpcodeInfo(DecodeAction::None, &Processor::Cp_a_imm), // 0xb9
        OpcodeInfo(DecodeAction::None, &Processor::Cp_a_imm), // 0xba
        OpcodeInfo(DecodeAction::None, &Processor::Cp_a_imm), // 0xbb
        OpcodeInfo(DecodeAction::None, &Processor::Cp_a_imm), // 0xbc
        OpcodeInfo(DecodeAction::None, &Processor::Cp_a_imm), // 0xbd
        OpcodeInfo(DecodeAction::None, &Processor::Cp_a_imm), // 0xbe
        OpcodeInfo(DecodeAction::None, &Processor::Cp_a_imm), // 0xbf

        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xc0
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xc1
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xc2
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xc3
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xc4
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xc5
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xc6
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xc7
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xc8
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xc9
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xca
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xcb
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xcc
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xcd
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xce
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xcf
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xd0
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xd1
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xd2
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xd3
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xd4
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xd5
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xd6
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xd7
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xd8
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xd9
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xda
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xdb
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xdc
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xdd
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xde
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xdf
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xe0
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xe1
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xe2
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xe3
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xe4
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xe5
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xe6
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xe7
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xe8
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xe9
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xea
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xeb
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xec
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xed
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xee
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xef
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xf0
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xf1
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xf2
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xf3
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xf4
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xf5
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xf6
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xf7
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xf8
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xf9
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xfa
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xfb
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xfc
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xfd
        OpcodeInfo(DecodeAction::None, &Processor::Jmp), // 0xfe
        OpcodeInfo(DecodeAction::None, &Processor::Jmp) // 0xff
    };

	Processor::Processor(Emulator::Scheduler& scheduler,
						double clockFrequency,
						uint8_t* programRom, uint16_t programRamLength,
						uint8_t* dataRam,uint8_t dataRamLength,
						Emulator::IOHandler& ioHandler,
						uint8_t* pla,uint8_t plaLength,
						const std::string tag,
						Emulator::Cpu& cpu,
						bool disabled,
						Emulator::IrqCallback irqCallback) :
		clockFrequency(clockFrequency),
		halt(false),
		status(),
		programRom(programRom),
		dataRam(dataRam),
		ramMask((uint8_t) (dataRamLength - 1)),
		ioHandler(ioHandler),
		pla(pla),
		plaLength(plaLength),
		aborted(false),
		cyclesLeft(0),
		cyclesInCurrentTimeslice(0),
		irqLine(cpu, [this](Emulator::LineState state) {
			this->OnIrqLineSet(state);
		}, 0x00000000),
		irqCallback(irqCallback),
#pragma warning (disable : 4355) //  'this' : used in base member initializer list
		serialTimer("SerialTimerCallback", scheduler,
			[this](int64_t param, int64_t currentTime) { this->SerialTimerCallback(param); }),
#pragma warning (default : 4355)
		instr(),
		disabled(disabled)
	{
	}

	Processor::~Processor()
	{
	}

    void Processor::SerialTimerCallback(int64_t param)
    {
        status.SBcount++;

        // if we get too many interrupts with no servicing, disable the timer
        // until somebody does something
        if (status.SBcount >= SERIAL_DISABLE_THRESH)
        {
            serialTimer.Stop();
        }

        // only read if not full; this is needed by the Namco 52xx to ensure that
        // the program can write to S and recover the value even if serial is enabled
        if (!status.sf)
        {
            status.SB = (uint8_t) ((status.SB >> 1) | ((ReadPort(Port::MB88_PORTSI) != 0) ? 8 : 0));

            if (status.SBcount >= 4)
            {
                status.sf = true;
                status.interruptPending |= INT_CAUSE_SERIAL;
            }
        }
    }

	void Processor::IncrementTimer()
    {
        status.TL = (uint8_t) ((status.TL + 1) & 0x0f);
        if (status.TL == 0)
        {
            status.TH = (uint8_t) ((status.TH + 1) & 0x0f);
            if (status.TH == 0)
            {
                status.vf = true;
                status.interruptPending |= INT_CAUSE_TIMER;
            }
        }
    }

   uint8_t Processor::GetPla(int inA, int inB)
    {
       uint8_t index = (uint8_t) (((inB & 1) << 4) | (inA & 0x0f));
        if (nullptr != pla)
        {
            assert(index < plaLength);
            return pla[index];
        }
        else
        {
            return index;
        }
    }

	void Processor::UpdatePio(int cycles)
    {
        /* internal clock enable */
        if (0 != (status.pio & 0x80))
        {
            status.TP = (uint8_t) (status.TP + cycles);
            while (status.TP >= TIMER_PRESCALE)
            {
                status.TP -= TIMER_PRESCALE;
                IncrementTimer();
            }
        }

        /* process pending interrupts */
        if ((status.interruptPending & status.pio) != 0)
        {
            status.SP[status.SI] = get_PC();
            status.SP[status.SI] |= (uint16_t)(status.cf ? 1 << 15 : 0);
            status.SP[status.SI] |= (uint16_t)(status.zf ? 1 << 14 : 0);
            status.SP[status.SI] |= (uint16_t)(status.st ? 1 << 13 : 0);
            status.SI = (uint8_t) ((status.SI + 1) & 3);

            /* the datasheet doesn't mention interrupt vectors but
            the Arabian MCU program expects the following */
            if ((status.interruptPending & status.pio & INT_CAUSE_EXTERNAL) != 0)
            {
                /* if we have a live external source, call the irqcallback */
				if (nullptr != irqCallback) {
					irqCallback();
				}
                status.PC = 0x02;
            }
            else if ((status.interruptPending & status.pio & INT_CAUSE_TIMER) != 0)
            {
                status.PC = 0x04;
            }
            else if ((status.interruptPending & status.pio & INT_CAUSE_SERIAL) != 0)
            {
                status.PC = 0x06;
            }

            status.PA = 0x00;
            status.st = true;
            status.interruptPending = 0;
            cyclesLeft -= 3;
        }
    }

    void Processor::UpdatePioEnable(uint8_t newPio)
    {
        // if the serial state has changed, configure the timer
        if (0 != ((status.pio ^ newPio) & 0x30))
        {
            if ((newPio & 0x30) == 0)
            {
                serialTimer.Stop();
            }
            else if ((newPio & 0x30) == 0x20)
            {
                serialTimer.Start((int64_t)(1e6 / (clockFrequency / SERIAL_PRESCALE)), false);
            }
            else
            {
                //throw new std::exception("mb88xx: update_pio_enable set serial enable to unsupported value");
				return;
            }
        }

        status.pio = newPio;
    }

    void Processor::OnIrqLineSet(Emulator::LineState state)
    {
        // on falling edge trigger interrupt
        if ((0 != (status.pio & 0x04)) && status.nf && state == Emulator::LineState::Clear)
        {
            status.interruptPending |= INT_CAUSE_EXTERNAL;
        }

        status.nf = (state != Emulator::LineState::Clear) ? 1 : 0;
    }

    Processor::Instruction& Processor::FetchDecode()
    {
        // Callback call with current Program Counter value.
        if(_programCounterListener)
            _programCounterListener(get_PC());
        //Instruction instr = new Instruction(); // for the sake of speed, reuse the same Instruction
        instr.opcode = ReadRom(get_PC());
        IncPC();
        
        // table Look up based on opcode to find action and function pointer
        const OpcodeInfo& opcodeInfo = fetchDecodeTable[instr.opcode];
        DecodeAction decodeAction = opcodeInfo.decodeAction;
        instr.execute = opcodeInfo.interpreterFunc;

        // operand resolution
        switch (decodeAction)
        {
			case DecodeAction::None:
                instr.cycles = 1;
                break;

            case DecodeAction::Immediate:
                instr.immediate = ReadRom(get_PC());
                IncPC();
                instr.cycles = 2;
                break;
        }

        return instr;
    }

}; // Mb88xx