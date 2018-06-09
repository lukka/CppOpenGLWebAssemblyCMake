#pragma once

#include "emulator/emulator.h"
#include "emulator/DeviceInput.h"
#include <exception>
#include <string>
#include <vector>
#include <iostream>

namespace Z80 {

#define A reg[7]
#define B reg[0]
#define C reg[1]
#define D reg[2]
#define E reg[3]
#define F reg[6]
#define H reg[4]
#define L reg[5]
#define c get_Carry()
#define s get_SignFlag()
#define z get_ZeroFlag()
#define h get_HFlag()
#define n get_NFlag()
#define p get_PVFlag()
#define v get_PVFlag()
#define BC get_BC()
#define DE get_DE()
#define HL get_HL()
#define HX get_HX()
#define HY get_HY()
#define LX get_LX()
#define LY get_LY()
#define Get_SZHVC_add_flag(operand, result, carry) (flag_SZHVC_add[operand][result][carry])
#define Get_SZHVC_sub_flag(operand, result, carry) (flag_SZHVC_sub[operand][result][carry])

	class Z80ProcessorImpl;

	enum InterruptMode
    {
        Mode0,
        Mode1,
        Mode2
    };

	class Z80ProcessorImpl  : public Emulator::IProcessor
	{
		typedef void (/*__thiscall*/ Z80ProcessorImpl::*Action)();

		struct Instruction
		{
			enum Prefix
			{
				None,
				CB,
				DD,
				ED,
				FD,
				DDCB,
				FDCB
			};

			enum Register
			{
				B_reg_index,
				C_reg_index,
				D_reg_index,
				E_reg_index,
				H_reg_index,
				L_reg_index,
				F_reg_index,
				A_reg_index
			};

			Prefix prefix;
			uint16_t opcode;
			int length;
			Register _register;
			uint8_t displacement;
			uint8_t immediate;
			uint16_t immediateExtended;
			Action execute;
			int cycles;
		};

        const static uint8_t B_index = 0;
        const static uint8_t C_index = 1;
        const static uint8_t D_index = 2;
        const static uint8_t E_index = 3;
        const static uint8_t H_index = 4;
        const static uint8_t L_index = 5;
        const static uint8_t F_index = 6;
        const static uint8_t A_index = 7;

        const static uint8_t S_mask = 0x80;
        const static uint8_t Z_mask = 0x40;
        const static uint8_t H_mask = 0x10;
        const static uint8_t PV_mask = 0x04;
        const static uint8_t N_mask = 0x02;
        const static uint8_t C_mask = 0x01;

        enum DecodeAction
        {
            None,
            Immediate,
            ImmediateExtended,
            Displacement,
            PrefixedDisplacement,
            DisplacementImmediate,
            DisplacementRegistry,
            Register
        };

        struct OpcodeInfo
        {
            OpcodeInfo(DecodeAction decodeAction, Action interpreterFunc)
            {
                this->decodeAction = decodeAction;
                this->interpreterFunc = interpreterFunc;
            }

            DecodeAction decodeAction;
            Action interpreterFunc;
        };

        static const uint8_t cc_op[];
        static const uint8_t cc_cb[];
        static const uint8_t cc_ed[];
        static const uint8_t cc_xy[];
        static const uint8_t cc_xycb[];

        // extra cycles if jr/jp/call taken and 'interrupt latency' on rst 0-7
        static const uint8_t cc_ex[];
        static const bool parity[];

        static uint8_t flag_SZHVC_add[256][256][2];
        static uint8_t flag_SZHVC_sub[256][256][2];

        static uint8_t flag_SZ[256];
        static uint8_t flag_SZP[256];
        static uint8_t flag_SZ_BIT[256];
        static uint8_t flag_SZHV_dec[256];
        static uint8_t flag_SZHV_inc[256];

        static const uint8_t rstMask[];

	public:
		Z80ProcessorImpl(double clockFrequency,uint8_t ram[], Emulator::IOHandler& ioHandler,
			const std::string tag, Emulator::IMemoryHandler* memoryHandler, Emulator::Cpu& cpu);
		~Z80ProcessorImpl();

		// Emulator::IProcessor
		virtual void Reset();
		virtual int64_t get_ExecutionTimeInCurrentTimeslice() const;
		virtual int64_t Execute(int64_t us);
		virtual void TakeInterrupt();
		virtual void AbortTimeslice();
		virtual Emulator::IDeviceInputLine& get_IrqInputLine() {
			return irqLine;
		}
		virtual Emulator::IDeviceInputLine& get_ResetInputLine() {
			return resetLine;
		}
		virtual Emulator::IDeviceInputLine& get_NmiInputLine() {
			return nmiLine;
		}
		virtual std::vector<Emulator::IDeviceInputLine*> get_InputLines();
		virtual uint32_t get_DefaultIrqVector() const {
			return 0x000000ff;
		}
        virtual void SetProgramCounterListener(ProgramCounterListener &pcListener) {
            _programCounterListener = pcListener;
        }

	private:
		uint8_t ReadMemory(uint16_t addr) const {
           uint8_t value;
            if (nullptr == memoryHandler || !memoryHandler->ReadMemory(addr, value))
            {
                value = ram[addr];
            }
            return value;
		} 
		uint16_t ReadMemory16(uint16_t addr) const {
            // code review: speed up!
           uint8_t low = ReadMemory(addr);
           uint8_t hi = ReadMemory((uint16_t)(addr + 1));
            return (uint16_t)((hi << 8) | low);
		} 
		void WriteMemory(uint16_t addr,uint8_t value) {
            if (nullptr == memoryHandler || !memoryHandler->WriteMemory(addr, value))
            {
                ram[addr] = value;
            }
		} 
		void WriteMemory16(uint16_t addr, uint16_t value) {
            // code review: speed up!
           uint8_t low = (uint8_t)(value & 0xff);
           uint8_t hi = (uint8_t)(value >> 8);
            WriteMemory(addr, low);
            WriteMemory((uint16_t)(addr + 1), hi);
		} 

        void SetAdcOpFlags(uint16_t op1, uint16_t op2, int result)
        {
            this->F = (uint8_t)(
                ((result >> 16) & C_mask) |
                ((result >> 8) & S_mask) |
                (((result ^ op1 ^ op2) >> 8) & H_mask) |
                (((result & 0xffff) == 0) ? Z_mask : 0) |
                (((op2 ^ op1 ^ 0x8000) & (op2 ^ result) & 0x8000) >> 13)
            );
        } 

        void SetAddOpFlags(uint16_t op1, uint16_t op2, int result)
        {
            this->F = (uint8_t)(
                (this->F & (S_mask | Z_mask | PV_mask)) |
                ((result >> 16) & C_mask) |
                (((result ^ op1 ^ op2) >> 8) & H_mask)
            );
        } 

        void SetSbcOpFlags(uint16_t op1, uint16_t op2, int result)
        {
            this->F = (uint8_t)(
                N_mask |
                ((result >> 16) & C_mask) |
                ((0 == (result & 0xffff)) ? Z_mask : 0) |
                (((op1 ^ result ^ op2) >> 8) & H_mask) |
                ((((op2 ^ op1) & (op1 ^ result)) & 0x8000) >> 13)
            );
        }

		Instruction FetchDecode(uint16_t pc);

		void OnNmiLineChanged(Emulator::LineState state) {
            // mark an NMI pending
            if (this->nmiState == Emulator::LineState::Clear && state != Emulator::LineState::Clear) {
				this->isNmiPending = true;
            }
			this->nmiState = state;
		}

		uint8_t get_A() const {
			return reg[7];
		}
		void set_A(uint8_t value) {
			reg[7] = value;
		}
		uint8_t get_B() const {
			return reg[0];
		}
		void set_B(uint8_t value) {
			reg[0] = value;
		}
		uint8_t get_C() const {
			return reg[1];
		}
		void set_C(uint8_t value) {
			reg[1] = value;
		}
		uint8_t get_D() const {
			return reg[2];
		}
		void set_D(uint8_t value) {
			reg[2] = value;
		}
		uint8_t get_E() const {
			return reg[3];
		}
		void set_E(uint8_t value) {
			reg[3] = value;
		}
		uint8_t get_H() const {
			return reg[4];
		}
		void set_H(uint8_t value) {
			reg[4] = value;
		}
		uint8_t get_L() const {
			return reg[5];
		}
		void set_L(uint8_t value) {
			reg[5] = value;
		}
		uint16_t get_BC() const {
			return ((reg[0] << 8) | reg[1]);
		}
		void set_BC(uint16_t value) {
			reg[0] = (value >> 8) & 0xff;
			reg[1] = (value & 0xff);
		}
		uint16_t get_DE() const {
			return ((reg[2] << 8) | reg[3]);
		}
		void set_DE(uint16_t value) {
			reg[2] = (value >> 8) & 0xff;
			reg[3] = (value & 0xff);
		}
		uint16_t get_HL() const {
			return ((reg[4] << 8) | reg[5]);
		}
		void set_HL(uint16_t value) {
			reg[4] = (value >> 8) & 0xff;
			reg[5] = (value & 0xff);
		}
		void set_LX(uint8_t value) {
			IX = (value & 0xff) | (IX & 0xff00);
		}
		void set_HX(uint8_t value) {
			IX = ((value << 8) & 0xff00) | (IX & 0xff);
		}
		void set_LY(uint8_t value) {
			IY = (value & 0xff) | (IY & 0xff00);
		}
		void set_HY(uint8_t value) {
			IY = ((value << 8) & 0xff00) | (IY & 0xff);
		}
		bool get_ZeroFlag() const {
			return 0 != (reg[6] & 0x40);
		};
		void set_ZeroFlag(bool value) {
			if (value) {
				reg[6] |= 0x40;
			}
			else {
				reg[6] &= ~0x40;
			}
		};
		uint8_t get_Carry() const {
			return 0 != (reg[6] & 0x01);
		};
		void set_Carry(bool value) {
			if (value) {
				reg[6] |= 0x01;
			}
			else {
				reg[6] &= ~0x01;
			}
		};
		bool get_SignFlag() const {
			return 0 != (reg[6] & 0x80);
		};
		void set_SignFlag(bool value) {
			if (value) {
				reg[6] |= 0x80;
			}
			else {
				reg[6] &= ~0x80;
			}
		};
		bool get_HFlag() const {
			return 0 != (reg[6] & 0x10);
		};
		void set_HFlag(bool value) {
			if (value) {
				reg[6] |= 0x10;
			}
			else {
				reg[6] &= ~0x10;
			}
		};
		bool get_NFlag() const {
			return 0 != (reg[6] & 0x02);
		};
		void set_NFlag(bool value) {
			if (value) {
				reg[6] |= 0x02;
			}
			else {
				reg[6] &= ~0x02;
			}
		};
		bool get_PVFlag() const {
			return 0 != (reg[6] & 0x04);
		};
		void set_PVFlag(bool value) {
			if (value) {
				reg[6] |= 0x04;
			}
			else {
				reg[6] &= ~0x04;
			}
		};
		uint8_t get_HX() const {
			return (IX >> 8);
		}
		uint8_t get_LX() const {
			return (IX & 0xff);
		}
		uint8_t get_HY() const {
			return (IY >> 8);
		}
		uint8_t get_LY() const {
			return (IY & 0xff);
		}

	private:
        void Noni()
        {
            // do nothing
            //throw new std::exception("invalid z80 instruction");
			return;
        }

        void Adc_a_r()
        {
           uint8_t d = this->reg[(int)this->instr._register];
           uint8_t result = (uint8_t)((this->A + this->c + d) & 0xff);
            this->F = Get_SZHVC_add_flag(this->A, result, this->c);
            this->A = result;
        }

        void Adc_a_n()
        {
           uint8_t result = (uint8_t)((this->A + this->c + this->instr.immediate) & 0xff);
            this->F = Get_SZHVC_add_flag(this->A, result, this->c);
            this->A = result;
        }

        void Adc_a_hx()
        {
           uint8_t result = (uint8_t)((this->A + this->c + this->HX) & 0xff);
            this->F = Get_SZHVC_add_flag(this->A, result, this->c);
            this->A = result;
        }

        void Adc_a_lx()
        {
           uint8_t result = (uint8_t)((this->A + this->c + this->LX) & 0xff);
            this->F = Get_SZHVC_add_flag(this->A, result, this->c);
            this->A = result;
        }

        void Adc_a_hy()
        {
           uint8_t result = (uint8_t)((this->A + this->c + this->HY) & 0xff);
            this->F = Get_SZHVC_add_flag(this->A, result, this->c);
            this->A = result;
        }

        void Adc_a_ly()
        {
           uint8_t result = (uint8_t)((this->A + this->c + this->LY) & 0xff);
            this->F = Get_SZHVC_add_flag(this->A, result, this->c);
            this->A = result;
        }

        void Adc_a_hl_ind()
        {
           uint8_t val = ReadMemory(this->HL);
           uint8_t result = (uint8_t)((this->A + this->c + val) & 0xff);
            this->F = Get_SZHVC_add_flag(this->A, result, this->c);
            this->A = result;
        }

        void Adc_a_ix_d_ind()
        {
            uint16_t addr = (uint16_t)(this->IX + this->instr.displacement);
           uint8_t val = ReadMemory(addr);
           uint8_t result = (uint8_t)((this->A + this->c + val) & 0xff);
            this->F = Get_SZHVC_add_flag(this->A, result, this->c);
            this->A = result;
        }

        void Adc_a_iy_d_ind()
        {
            uint16_t addr = (uint16_t)(this->IY + this->instr.displacement);
           uint8_t val = ReadMemory(addr);
           uint8_t result = (uint8_t)((this->A + this->c + val) & 0xff);
            this->F = Get_SZHVC_add_flag(this->A, result, this->c);
            this->A = result;
        }

        void Adc_hl_bc()
        {
            int result = this->HL + this->c + this->BC;
            SetAdcOpFlags(this->HL, this->BC, result);
            this->set_HL((uint16_t)(result & 0xffff));
        }

        void Adc_hl_de()
        {
            int result = this->HL + this->c + this->DE;
            SetAdcOpFlags(this->HL, this->DE, result);
            this->set_HL((uint16_t)(result & 0xffff));
        }

        void Adc_hl_hl()
        {
            int result = this->HL + this->c + this->HL;
            SetAdcOpFlags(this->HL, this->HL, result);
            this->set_HL((uint16_t)(result & 0xffff));
        }

        void Adc_hl_sp()
        {
            int result = this->HL + this->c + this->SP;
            SetAdcOpFlags(this->HL, this->SP, result);
            this->set_HL((uint16_t)(result & 0xffff));
        }

        void Add_a_r()
        {
           uint8_t d = this->reg[(int)this->instr._register];
           uint8_t result = (uint8_t)((this->A + d) & 0xff);
            this->F = Get_SZHVC_add_flag(this->A, result, 0);
            this->A = result;
        }

        void Add_a_n()
        {
           uint8_t result = (uint8_t)((this->A + this->instr.immediate) & 0xff);
            this->F = Get_SZHVC_add_flag(this->A, result, 0);
            this->A = result;
        }

        void Add_a_hx()
        {
           uint8_t result = (uint8_t)((this->A + this->HX) & 0xff);
            this->F = Get_SZHVC_add_flag(this->A, result, 0);
            this->A = result;
        }

        void Add_a_lx()
        {
           uint8_t result = (uint8_t)((this->A + this->LX) & 0xff);
            this->F = Get_SZHVC_add_flag(this->A, result, 0);
            this->A = result;
        }

        void Add_a_hy()
        {
           uint8_t result = (uint8_t)((this->A + this->HY) & 0xff);
            this->F = Get_SZHVC_add_flag(this->A, result, 0);
            this->A = result;
        }

        void Add_a_ly()
        {
           uint8_t result = (uint8_t)((this->A + this->LY) & 0xff);
            this->F = Get_SZHVC_add_flag(this->A, result, 0);
            this->A = result;
        }

        void Add_a_hl_ind()
        {
           uint8_t val = ReadMemory(this->HL);
           uint8_t result = (uint8_t)((this->A + val) & 0xff);
            this->F = Get_SZHVC_add_flag(this->A, result, 0);
            this->A = result;
        }

        void Add_a_ix_d_ind()
        {
            uint16_t addr = (uint16_t)(this->IX + this->instr.displacement);
           uint8_t val = ReadMemory(addr);
           uint8_t result = (uint8_t)((this->A + val) & 0xff);
            this->F = Get_SZHVC_add_flag(this->A, result, 0);
            this->A = result;
        }

        void Add_a_iy_d_ind()
        {
            uint16_t addr = (uint16_t)(this->IY + this->instr.displacement);
           uint8_t val = ReadMemory(addr);
           uint8_t result = (uint8_t)((this->A + val) & 0xff);
            this->F = Get_SZHVC_add_flag(this->A, result, 0);
            this->A = result;
        }

        void Add_hl_bc()
        {
            int result = this->HL + this->BC;
            SetAddOpFlags(this->HL, this->BC, result);
            this->set_HL((uint16_t)(result & 0xffff));
        }

        void Add_hl_de()
        {
            int result = this->HL + this->DE;
            SetAddOpFlags(this->HL, this->DE, result);
            this->set_HL((uint16_t)(result & 0xffff));
        }

        void Add_hl_hl()
        {
            int result = this->HL + this->HL;
            SetAddOpFlags(this->HL, this->HL, result);
            this->set_HL((uint16_t)(result & 0xffff));
        }

        void Add_hl_sp()
        {
            int result = this->HL + this->SP;
            SetAddOpFlags(this->HL, this->SP, result);
            this->set_HL((uint16_t)(result & 0xffff));
        }

        void Add_ix_bc()
        {
            int result = this->IX + this->BC;
            SetAddOpFlags(this->IX, this->BC, result);
            this->IX = (uint16_t)(result & 0xffff);
        }

        void Add_ix_de()
        {
            int result = this->IX + this->DE;
            SetAddOpFlags(this->IX, this->DE, result);
            this->IX = (uint16_t)(result & 0xffff);
        }

        void Add_ix_ix()
        {
            int result = this->IX + this->IX;
            SetAddOpFlags(this->IX, this->IX, result);
            this->IX = (uint16_t)(result & 0xffff);
        }

        void Add_ix_sp()
        {
            int result = this->IX + this->SP;
            SetAddOpFlags(this->IX, this->SP, result);
            this->IX = (uint16_t)(result & 0xffff);
        }

        void Add_iy_bc()
        {
            int result = this->IY + this->BC;
            SetAddOpFlags(this->IY, this->BC, result);
            this->IY = (uint16_t)(result & 0xffff);
        }

        void Add_iy_de()
        {
            int result = this->IY + this->DE;
            SetAddOpFlags(this->IY, this->DE, result);
            this->IY = (uint16_t)(result & 0xffff);
        }

        void Add_iy_iy()
        {
            int result = this->IY + this->IY;
            SetAddOpFlags(this->IY, this->IY, result);
            this->IY = (uint16_t)(result & 0xffff);
        }

        void Add_iy_sp()
        {
            int result = this->IY + this->SP;
            SetAddOpFlags(this->IY, this->SP, result);
            this->IY = (uint16_t)(result & 0xffff);
        }

        void And_a_r()
        {
            this->A = (uint8_t)(this->A & this->reg[(int)this->instr._register]);
            this->F = (uint8_t)(flag_SZP[this->A] | H_mask);
        }

        void And_a_n()
        {
            this->A = (uint8_t)(this->A & this->instr.immediate);
            this->F = (uint8_t)(flag_SZP[this->A] | H_mask);
        }

        void And_a_hx()
        {
            this->A = (uint8_t)(this->A & this->HX);
            this->F = (uint8_t)(flag_SZP[this->A] | H_mask);
        }

        void And_a_lx()
        {
            this->A = (uint8_t)(this->A & this->LX);
            this->F = (uint8_t)(flag_SZP[this->A] | H_mask);
        }

        void And_a_hy()
        {
            this->A = (uint8_t)(this->A & this->HY);
            this->F = (uint8_t)(flag_SZP[this->A] | H_mask);
        }

        void And_a_ly()
        {
            this->A = (uint8_t)(this->A & this->LY);
            this->F = (uint8_t)(flag_SZP[this->A] | H_mask);
        }

        void And_a_hl_ind()
        {
           uint8_t val = ReadMemory(this->HL);
            this->A = (uint8_t)(this->A & val);
            this->F = (uint8_t)(flag_SZP[this->A] | H_mask);
        }

        void And_a_ix_d_ind()
        {
            uint16_t addr = (uint16_t)(this->IX + this->instr.displacement);
           uint8_t val = ReadMemory(addr);
            this->A = (uint8_t)(this->A & val);
            this->F = (uint8_t)(flag_SZP[this->A] | H_mask);
        }

        void And_a_iy_d_ind()
        {
            uint16_t addr = (uint16_t)(this->IY + this->instr.displacement);
           uint8_t val = ReadMemory(addr);
            this->A = (uint8_t)(this->A & val);
            this->F = (uint8_t)(flag_SZP[this->A] | H_mask);
        }

        void Bit_bit_r()
        {
           uint8_t val = this->reg[(int)this->instr._register];
            int bit = (this->instr.opcode >> 3) & 0x07;
            this->F = (uint8_t)(this->c | H_mask | flag_SZ_BIT[val & (1 << bit)]);
        }

        void Bit_bit_hl_ind()
        {
           uint8_t val = ReadMemory(this->HL);
            int bit = (this->instr.opcode >> 3) & 0x07;
            this->F = (uint8_t)(this->c | H_mask | flag_SZ_BIT[val & (1 << bit)]);
        }

        void Bit_bit_ix_d_ind()
        {
            uint16_t addr = (uint16_t)(this->IX + this->instr.displacement);
           uint8_t val = ReadMemory(addr);
            int bit = (this->instr.opcode >> 3) & 0x07;
            this->F = (uint8_t)(this->c | H_mask | flag_SZ_BIT[val & (1 << bit)]);
        }

        void Bit_bit_iy_d_ind()
        {
            uint16_t addr = (uint16_t)(this->IY + this->instr.displacement);
           uint8_t val = ReadMemory(addr);
            int bit = (this->instr.opcode >> 3) & 0x07;
            this->F = (uint8_t)(this->c | H_mask | flag_SZ_BIT[val & (1 << bit)]);
        }

        void Call(uint16_t addr)
        {
            WriteMemory(--this->SP, (uint8_t)(this->PC >> 8));
            WriteMemory(--this->SP, (uint8_t)(this->PC & 0xff));
            this->PC = addr;
        }

        void Call_nn()
        {
            Call(this->instr.immediateExtended);
        }

        void Call_c_nn()
        {
            if (this->c != 0)
            {
                Call(this->instr.immediateExtended);
                this->cyclesLeft -= cc_ex[this->instr.opcode];
            }
        }

        void Call_nc_nn()
        {
            if (this->c == 0)
            {
                Call(this->instr.immediateExtended);
                this->cyclesLeft -= cc_ex[this->instr.opcode];
            }
        }

        void Call_m_nn()
        {
            if (this->s)
            {
                Call(this->instr.immediateExtended);
                this->cyclesLeft -= cc_ex[this->instr.opcode];
            }
        }

        void Call_p_nn()
        {
            if (! this->s)
            {
                Call(this->instr.immediateExtended);
                this->cyclesLeft -= cc_ex[this->instr.opcode];
            }
        }

        void Call_z_nn()
        {
            if (this->z)
            {
                Call(this->instr.immediateExtended);
                this->cyclesLeft -= cc_ex[this->instr.opcode];
            }
        }

        void Call_nz_nn()
        {
            if (! this->z)
            {
                Call(this->instr.immediateExtended);
                this->cyclesLeft -= cc_ex[this->instr.opcode];
            }
        }

        void Call_pe_nn()
        {
            if (this->p)
            {
                Call(this->instr.immediateExtended);
                this->cyclesLeft -= cc_ex[this->instr.opcode];
            }
        }

        void Call_po_nn()
        {
            if (!this->p)
            {
                Call(this->instr.immediateExtended);
                this->cyclesLeft -= cc_ex[this->instr.opcode];
            }
        }

        void Ccf()
        {
            this->set_HFlag(this->c != 0);
            this->set_Carry(this->c == 0);
            this->set_NFlag(false);
        }

        void Cp_a_r()
        {
           uint8_t d = this->reg[(int)this->instr._register];
           uint8_t result = (uint8_t)((this->A - d) & 0xff);
            this->F = Get_SZHVC_sub_flag(this->A, result, 0);
        }

        void Cp_a_n()
        {
           uint8_t d = this->instr.immediate;
           uint8_t result = (uint8_t)((this->A - d) & 0xff);
            this->F = Get_SZHVC_sub_flag(this->A, result, 0);
        }

        void Cp_a_hx()
        {
           uint8_t d = this->HX;
           uint8_t result = (uint8_t)((this->A - d) & 0xff);
            this->F = Get_SZHVC_sub_flag(this->A, result, 0);
        }

        void Cp_a_lx()
        {
           uint8_t d = this->LX;
           uint8_t result = (uint8_t)((this->A - d) & 0xff);
            this->F = Get_SZHVC_sub_flag(this->A, result, 0);
        }

        void Cp_a_hy()
        {
           uint8_t d = this->HY;
           uint8_t result = (uint8_t)((this->A - d) & 0xff);
            this->F = Get_SZHVC_sub_flag(this->A, result, 0);
        }

        void Cp_a_ly()
        {
           uint8_t d = this->LY;
           uint8_t result = (uint8_t)((this->A - d) & 0xff);
            this->F = Get_SZHVC_sub_flag(this->A, result, 0);
        }

        void Cp_a_hl_ind()
        {
           uint8_t val = ReadMemory(this->HL);
           uint8_t result = (uint8_t)((this->A - val) & 0xff);
            this->F = Get_SZHVC_sub_flag(this->A, result, 0);
        }

        void Cp_a_ix_d_ind()
        {
            uint16_t addr = (uint16_t)(this->IX + this->instr.displacement);
           uint8_t val = ReadMemory(addr);
           uint8_t result = (uint8_t)((this->A - val) & 0xff);
            this->F = Get_SZHVC_sub_flag(this->A, result, 0);
        }

        void Cp_a_iy_d_ind()
        {
            uint16_t addr = (uint16_t)(this->IY + this->instr.displacement);
           uint8_t val = ReadMemory(addr);
           uint8_t result = (uint8_t)((this->A - val) & 0xff);
            this->F = Get_SZHVC_sub_flag(this->A, result, 0);
        }

        // compare and decrement
        void Cpd()
        {
           uint8_t val = ReadMemory(this->HL);
           uint8_t result = (uint8_t)(this->A - val);
            this->Dec_hl();
            this->Dec_bc();
            this->F = (uint8_t)(this->c | flag_SZ[result] | ((this->A ^ val ^ result) & H_mask) |
                N_mask | (this->BC != 0 ? PV_mask : 0));
        }

        // compare and decrement, repeat
        void Cpdr()
        {
           uint8_t val = ReadMemory(this->HL);
           uint8_t result = (uint8_t)(this->A - val);
            this->Dec_hl();
            this->Dec_bc();

            this->F = (uint8_t)(this->c | flag_SZ[result] | ((this->A ^ val ^ result) & H_mask) |
                N_mask | (this->BC != 0 ? PV_mask : 0));

            if (!this->z && (this->BC != 0))
            {
                // repeat
                this->PC -= 2;
                cyclesLeft -= cc_ex[this->instr.opcode];
            }
        }

        // compare and increment
        void Cpi()
        {
           uint8_t val = ReadMemory(this->HL);
           uint8_t result = (uint8_t)(this->A - val);
            this->Inc_hl();
            this->Dec_bc();
            this->F = (uint8_t)(this->c | flag_SZ[result] | ((this->A ^ val ^ result) & H_mask) |
                N_mask | (this->BC != 0 ? PV_mask : 0));
        }

        // compare and increment, repeat
        void Cpir()
        {
           uint8_t val = ReadMemory(this->HL);
           uint8_t result = (uint8_t)(this->A - val);
            this->Inc_hl();
            this->Dec_bc();
            this->F = (uint8_t)(this->c | flag_SZ[result] | ((this->A ^ val ^ result) & H_mask) |
                N_mask | (this->BC != 0 ? PV_mask : 0));
            
            if (!this->z && (this->BC != 0))
            {
                // repeat
                this->PC -= 2;
                cyclesLeft -= cc_ex[this->instr.opcode];
            }
        }

        // complement
        void Cpl()
        {
            this->A ^= 0xff;
            this->set_HFlag(true);
            this->set_NFlag(true);
        }

        // decimal adjust
        void Da_a()
        {
            // Code review!

            int result = this->A;
            int tmp = 0;
            if (this->n)
            {
                tmp = this->F & 0x13; // & (CFLAG|NFLAG|HFLAG)
                if (this->c != 0 || this->A > 0x99)
                {
                    result -= 0x160;
                }

                if (this->h || (this->A & 0x0f) > 9)
                {
                    if ((this->A & 0x0F) > 5)
                    {
                        tmp &= 0xef; // ~HFLAG;
                    }
                    result = (result & 0xFF00) | ((result - 6) & 0xFF); 
                }
            }
            else
            {
                tmp = (this->F & 0x01) | (((this->A & 0x0F) > 9) ? 0x10 : 0x00); // 0x10:HFLAG
                if (((tmp | this->F) & 0x10) != 0)
                {
                    result += 6;
                }

                if ((this->c != 0) || ((result & 0x1F0) > 0x90))
                {
                    result += 0x60;
                }
            }
            
            this->A = (uint8_t)(result & 0xff);
            this->F = (uint8_t)(flag_SZP[this->A] | tmp | ((result >> 8) & 0x01));
        }

        void Dec_r()
        {
            int registerIndex = (this->instr.opcode & 0x38) >> 3;
           uint8_t m = this->reg[registerIndex];
           uint8_t result = (uint8_t)(m - 1);
            this->reg[registerIndex] = result;
            this->F = (uint8_t)(this->c | flag_SZHV_dec[result]);
        }

        void Dec_hl_ind()
        {
           uint8_t val = ReadMemory(this->HL);
           uint8_t result = (uint8_t)(val - 1);
            WriteMemory(this->HL, result);
            this->F = (uint8_t)(this->c | flag_SZHV_dec[result]); 
        }

        void Dec_ix_d_ind()
        {
            uint16_t addr = (uint16_t)(this->IX + this->instr.displacement);
           uint8_t val = ReadMemory(addr);
           uint8_t result = (uint8_t)(val - 1);
            WriteMemory(addr, result);
            this->F = (uint8_t)(this->c | flag_SZHV_dec[result]); 
        }

        void Dec_iy_d_ind()
        {
            uint16_t addr = (uint16_t)(this->IY + this->instr.displacement);
           uint8_t val = ReadMemory(addr);
           uint8_t result = (uint8_t)(val - 1);
            WriteMemory(addr, result);
            this->F = (uint8_t)(this->c | flag_SZHV_dec[result]); 
        }

        void Dec_bc()
        {
			set_BC(get_BC() - 1);
        }

        void Dec_de()
        {
			set_DE(get_DE() - 1);
        }

        void Dec_hl()
        {
			set_HL(get_HL() - 1);
        }

        void Dec_sp()
        {
            this->SP--;
        }

        void Dec_ix()
        {
            this->IX--;
        }

        void Dec_hx()
        {
           uint8_t val = this->HX;
           uint8_t result = (uint8_t)(val - 1);
            this->set_HX(result);
            this->F = (uint8_t)(this->c | flag_SZHV_dec[result]);
        }

        void Dec_lx()
        {
           uint8_t val = this->LX;
           uint8_t result = (uint8_t)(val - 1);
            this->set_LX(result);
            this->F = (uint8_t)(this->c | flag_SZHV_dec[result]);
        }

        void Dec_iy()
        {
            this->IY--;
        }

        void Dec_hy()
        {
           uint8_t val = this->HY;
           uint8_t result = (uint8_t)(val - 1);
            this->set_HY(result);
            this->F = (uint8_t)(this->c | flag_SZHV_dec[result]);
        }

        void Dec_ly()
        {
           uint8_t val = this->LY;
           uint8_t result = (uint8_t)(val - 1);
            this->set_LY(result);
            this->F = (uint8_t)(this->c | flag_SZHV_dec[result]);
        }

        void Di()
        {
            this->IFF1 = this->IFF2 = false;
        }

        void Djnz_d()
        {
            if (--this->B != 0)
            {
                this->PC += (uint16_t)(int8_t)(this->instr.displacement);
                cyclesLeft -= cc_ex[this->instr.opcode];
            }
        }

        void Ei()
        {
            this->IFF1 = this->IFF2 = true;
            this->AfterEI = true;
        }

        void Ex_sp_ind_hl()
        {
           uint8_t low = ReadMemory(this->SP);
           uint8_t hi = ReadMemory((uint16_t)(this->SP + 1));
            WriteMemory(this->SP, this->L);
            WriteMemory((uint16_t)(this->SP + 1), this->H);
            this->L = low;
            this->H = hi;
        }

        void Ex_sp_ind_ix()
        {
            uint16_t mem = ReadMemory16(this->SP);
            WriteMemory16(this->SP, this->IX);
            this->IX = mem;
        }

        void Ex_sp_ind_iy()
        {
            uint16_t mem = ReadMemory16(this->SP);
            WriteMemory16(this->SP, this->IY);
            this->IY = mem;
        }

        void Ex_af1_af2()
        {
            // (AF) <-> (AF')

           uint8_t tmp = this->reg[A_index];
            this->reg[A_index] = this->altReg[A_index];
            this->altReg[A_index] = tmp;

            tmp = this->reg[F_index];
            this->reg[F_index] = this->altReg[F_index];
            this->altReg[F_index] = tmp;
        }

        void Ex_de_hl()
        {
            uint16_t tmp = this->DE;
            this->set_DE(this->HL);
            this->set_HL(tmp);
        }

        void Exx()
        {
            // (BC) <-> (BC'), (DE) <-> (DE'), (HL) <-> (HL')

           uint8_t tmp = this->reg[B_index];
            this->reg[B_index] = this->altReg[B_index];
            this->altReg[B_index] = tmp;

            tmp = this->reg[C_index];
            this->reg[C_index] = this->altReg[C_index];
            this->altReg[C_index] = tmp;

            tmp = this->reg[D_index];
            this->reg[D_index] = this->altReg[D_index];
            this->altReg[D_index] = tmp;

            tmp = this->reg[E_index];
            this->reg[E_index] = this->altReg[E_index];
            this->altReg[E_index] = tmp;

            tmp = this->reg[H_index];
            this->reg[H_index] = this->altReg[H_index];
            this->altReg[H_index] = tmp;

            tmp = this->reg[L_index];
            this->reg[L_index] = this->altReg[L_index];
            this->altReg[L_index] = tmp;
        }

        void Halt()
        {
            this->halt = true;
            this->PC--;
            if ((this->irqLine.get_State() == Emulator::LineState::Clear) && (this->cyclesLeft > 0L))
            {
                int64_t num = (this->cyclesLeft + 3L) / 4L;
                this->R = (uint8_t)(this->R + num);
                this->cyclesLeft -= 4L * num;
            }
        }

        void LeaveHalt()
        {
            if (this->halt)
            {
                this->halt = false;
                this->PC++;
            }
        }

        void Im_0()
        {
            this->interruptMode = InterruptMode::Mode0;
        }

        void Im_1()
        {
            this->interruptMode = InterruptMode::Mode1;
        }

        void Im_2()
        {
            this->interruptMode = InterruptMode::Mode2;
        }

        void In_a_n()
        {
            this->A = ioHandler.Read(this->instr.immediate);
        }

        void In_r_c()
        {
            int registerIndex = (this->instr.opcode & 0x38) >> 3;
           uint8_t val = ioHandler.Read(this->C);
            this->reg[registerIndex] = val;
            this->F = (uint8_t)((this->F & C_mask) | flag_SZP[val]);
        }

        void In_0_c()
        {
           uint8_t val = ioHandler.Read(this->C);
            this->F = (uint8_t)((this->F & C_mask) | flag_SZP[val]);
        }

        void Inc_r()
        {
            int registerIndex = (this->instr.opcode & 0x38) >> 3;
           uint8_t m = this->reg[registerIndex];
           uint8_t result = (uint8_t)(m + 1);
            this->reg[registerIndex] = result;
            this->F = (uint8_t)(this->c | flag_SZHV_inc[result]);
        }

        void Inc_bc()
        {
			set_BC(get_BC() + 1);
        }

        void Inc_de()
        {
			set_DE(get_DE() + 1);
        }

        void Inc_hl()
        {
			set_HL(get_HL() + 1);
        }

        void Inc_sp()
        {
            this->SP++;
        }

        void Inc_ix()
        {
            this->IX++;
        }

        void Inc_hx()
        {
           uint8_t val = this->HX;
           uint8_t result = (uint8_t)(val + 1);
            this->set_HX(result);
            this->F = (uint8_t)(this->c | flag_SZHV_inc[result]);
        }

        void Inc_lx()
        {
           uint8_t val = this->LX;
           uint8_t result = (uint8_t)(val + 1);
            this->set_LX(result);
            this->F = (uint8_t)(this->c | flag_SZHV_inc[result]);
        }

        void Inc_iy()
        {
            this->IY++;
        }

        void Inc_hy()
        {
           uint8_t val = this->HY;
           uint8_t result = (uint8_t)(val + 1);
            this->set_HY(result);
            this->F = (uint8_t)(this->c | flag_SZHV_inc[result]);
        }

        void Inc_ly()
        {
           uint8_t val = this->LY;
           uint8_t result = (uint8_t)(val + 1);
            this->set_LY(result);
            this->F = (uint8_t)(this->c | flag_SZHV_inc[result]);
        }

        void Inc_hl_ind()
        {
           uint8_t val = ReadMemory(this->HL);
           uint8_t result = (uint8_t)(val + 1);
            WriteMemory(this->HL, result);
            this->F = (uint8_t)(this->c | flag_SZHV_inc[result]);
        }

        void Inc_ix_d_ind()
        {
            uint16_t addr = (uint16_t)(this->IX + this->instr.displacement);
           uint8_t val = ReadMemory(addr);
           uint8_t result = (uint8_t)(val + 1);
            WriteMemory(addr, result);
            this->F = (uint8_t)(this->c | flag_SZHV_inc[result]);
        }

        void Inc_iy_d_ind()
        {
            uint16_t addr = (uint16_t)(this->IY + this->instr.displacement);
           uint8_t val = ReadMemory(addr);
           uint8_t result = (uint8_t)(val + 1);
            WriteMemory(addr, result);
            this->F = (uint8_t)(this->c | flag_SZHV_inc[result]);
        }

        void Ind()
        {
           uint8_t val = ioHandler.Read(this->C);
            this->B--;
            WriteMemory(this->HL, val);
            this->Dec_hl();

            // code review
           uint8_t f = flag_SZ[this->B];
            int t = ((this->C - 1) & 0xff) + val;
            if ((val & S_mask) != 0)
            {
                f |= N_mask;
            }

            if ((t & 0x100) != 0)
            {
                f |= (uint8_t)(H_mask | C_mask);
            }

            this->F = f | (uint8_t)(flag_SZP[(t & 0x07) ^ this->B] & PV_mask);
        }

        void Indr()
        {
           uint8_t val = ioHandler.Read(this->C);
            WriteMemory(this->HL, val);
            this->Dec_hl();
            this->B--;

            // code review
           uint8_t f = flag_SZ[this->B];
            int t = ((this->C - 1) & 0xff) + val;
            if ((val & S_mask) != 0)
            {
                f |= N_mask;
            }

            if ((t & 0x100) != 0)
            {
                f |= (uint8_t)(H_mask | C_mask);
            }

            this->F = f | (uint8_t)(flag_SZP[(t & 0x07) ^ this->B] & PV_mask);
            
            if (this->B != 0)
            {
                // repeat
                this->PC -= 2;
                cyclesLeft -= cc_ex[this->instr.opcode];
            }
        }

        void Ini()
        {
           uint8_t val = ioHandler.Read(this->C);
            WriteMemory(this->HL, val);
            this->B--;
            this->Inc_hl();

            // code review
           uint8_t f = flag_SZ[this->B];
            int t = ((this->C + 1) & 0xff) + val;
            if ((val & C_mask) != 0)
            {
                f |= N_mask;
            }

            if ((t & 0x100) != 0)
            {
                f |= (uint8_t)(H_mask | C_mask);
            }

            this->F = f | (uint8_t)(flag_SZP[(t & 0x07) ^ this->B] & PV_mask);
        }

        void Inir()
        {
           uint8_t val = ioHandler.Read(this->C);
            WriteMemory(this->HL, val);
            this->Inc_hl();
            this->B--;

            // code review
           uint8_t f = flag_SZ[this->B];
            int t = ((this->C + 1) & 0xff) + val;
            if ((val & C_mask) != 0)
            {
                f |= N_mask;
            }

            if ((t & 0x100) != 0)
            {
                f |= (uint8_t)(H_mask | C_mask);
            }

            this->F = f | (uint8_t)(flag_SZP[(t & 0x07) ^ this->B] & PV_mask);
            
            if (this->B != 0)
            {
                // repeat
                this->PC -= 2;
                cyclesLeft -= cc_ex[this->instr.opcode];
            }
        }

        void Jp_nn()
        {
            this->PC = this->instr.immediateExtended;
        }

        void Jp_hl_ind()
        {
            this->PC = this->HL;
        }

        void Jp_ix_ind()
        {
            this->PC = this->IX;
        }

        void Jp_iy_ind()
        {
            this->PC = this->IY;
        }

        void Jp_c_nn()
        {
            if (this->c != 0)
            {
                this->PC = this->instr.immediateExtended;
                cyclesLeft -= cc_ex[this->instr.opcode];
            }
        }

        void Jp_nc_nn()
        {
            if (this->c == 0)
            {
                this->PC = this->instr.immediateExtended;
                cyclesLeft -= cc_ex[this->instr.opcode];
            }
        }

        void Jp_m_nn()
        {
            if (this->s)
            {
                this->PC = this->instr.immediateExtended;
                cyclesLeft -= cc_ex[this->instr.opcode];
            }
        }

        void Jp_p_nn()
        {
            if (! this->s)
            {
                this->PC = this->instr.immediateExtended;
                cyclesLeft -= cc_ex[this->instr.opcode];
            }
        }

        void Jp_z_nn()
        {
            if (this->z)
            {
                this->PC = this->instr.immediateExtended;
                cyclesLeft -= cc_ex[this->instr.opcode];
            }
        }

        void Jp_nz_nn()
        {
            if (! this->z)
            {
                this->PC = this->instr.immediateExtended;
                cyclesLeft -= cc_ex[this->instr.opcode];
            }
        }

        void Jp_pe_nn()
        {
            if (this->p)
            {
                this->PC = this->instr.immediateExtended;
                cyclesLeft -= cc_ex[this->instr.opcode];
            }
        }

        void Jp_po_nn()
        {
            if (! this->p)
            {
                this->PC = this->instr.immediateExtended;
                cyclesLeft -= cc_ex[this->instr.opcode];
            }
        }

        void Jr_d()
        {
            this->PC += (uint16_t)(int8_t)(this->instr.displacement);
        }

        void Jr_c_d()
        {
            if (this->c != 0)
            {
                this->PC += (uint16_t)(int8_t)(this->instr.displacement);
                cyclesLeft -= cc_ex[this->instr.opcode];
            }
        }

        void Jr_nc_d()
        {
            if (this->c == 0)
            {
                this->PC += (uint16_t)(int8_t)(this->instr.displacement);
                cyclesLeft -= cc_ex[this->instr.opcode];
            }
        }

        void Jr_z_d()
        {
            if (this->z)
            {
                this->PC += (uint16_t)(int8_t)(this->instr.displacement);
                cyclesLeft -= cc_ex[this->instr.opcode];
            }
        }

        void Jr_nz_d()
        {
            if (! this->z)
            {
                this->PC += (uint16_t)(int8_t)(this->instr.displacement);
                cyclesLeft -= cc_ex[this->instr.opcode];
            }
        }

        void Ld_i_a()
        {
            this->I = this->A;
        }

        void Ld_mr_a()
        {
            this->R = this->A;
        }

        void Ld_a_i()
        {
            this->A = this->I;
            this->F = (uint8_t)(this->c | flag_SZ[this->A] | (this->IFF2 ? PV_mask : 0));
        }

        void Ld_a_mr()
        {
            this->A = this->R;
            this->F = (uint8_t)(this->c | flag_SZ[this->A] | (this->IFF2 ? PV_mask : 0));
        }

        void Ld_a_r()
        {
            this->A = this->reg[(int)this->instr._register];
        }

        void Ld_a_n()
        {
            this->A = this->instr.immediate;
        }

        void Ld_a_bc_ind()
        {
            this->A = ReadMemory(this->BC);
        }

        void Ld_a_de_ind()
        {
            this->A = ReadMemory(this->DE);
        }

        void Ld_a_hl_ind()
        {
            this->A = ReadMemory(this->HL);
        }

        void Ld_a_ix_d_ind()
        {
            uint16_t addr = (uint16_t)(this->IX + this->instr.displacement);
            this->A = ReadMemory(addr);
        }

        void Ld_a_hx()
        {
            this->A = this->HX;
        }

        void Ld_a_lx()
        {
            this->A = this->LX;
        }

        void Ld_a_iy_d_ind()
        {
            uint16_t addr = (uint16_t)(this->IY + this->instr.displacement);
            this->A = ReadMemory(addr);
        }

        void Ld_a_hy()
        {
            this->A = this->HY;
        }

        void Ld_a_ly()
        {
            this->A = this->LY;
        }

        void Ld_a_nn_ind()
        {
            this->A = ReadMemory(this->instr.immediateExtended);
        }

        void Ld_b_r()
        {
            this->B = this->reg[(int)this->instr._register];
        }

        void Ld_b_n()
        {
            this->B = this->instr.immediate;
        }

        void Ld_b_hl_ind()
        {
            this->B = ReadMemory(this->HL);
        }

        void Ld_b_ix_d_ind()
        {
            uint16_t addr = (uint16_t)(this->IX + this->instr.displacement);
            this->B = ReadMemory(addr);
        }

        void Ld_b_iy_d_ind()
        {
            uint16_t addr = (uint16_t)(this->IY + this->instr.displacement);
            this->B = ReadMemory(addr);
        }

        void Ld_b_hx()
        {
            this->B = this->HX;
        }

        void Ld_b_lx()
        {
            this->B = this->LX;
        }

        void Ld_b_hy()
        {
            this->B = this->HY;
        }

        void Ld_b_ly()
        {
            this->B = this->LY;
        }

        void Ld_c_r()
        {
            this->C = this->reg[(int)this->instr._register];
        }

        void Ld_c_n()
        {
            this->C = this->instr.immediate;
        }

        void Ld_c_hl_ind()
        {
            this->C = ReadMemory(this->HL);
        }

        void Ld_c_ix_d_ind()
        {
            uint16_t addr = (uint16_t)(this->IX + this->instr.displacement);
            this->C = ReadMemory(addr);
        }

        void Ld_c_hx()
        {
            this->C = this->HX;
        }

        void Ld_c_lx()
        {
            this->C = this->LX;
        }

        void Ld_c_iy_d_ind()
        {
            uint16_t addr = (uint16_t)(this->IY + this->instr.displacement);
            this->C = ReadMemory(addr);
        }

        void Ld_c_hy()
        {
            this->C = this->HY;
        }

        void Ld_c_ly()
        {
            this->C = this->LY;
        }

        void Ld_d_r()
        {
            this->D = this->reg[(int)this->instr._register];
        }

        void Ld_d_n()
        {
            this->D = this->instr.immediate;
        }

        void Ld_d_hl_ind()
        {
            this->D = ReadMemory(this->HL);
        }

        void Ld_d_ix_d_ind()
        {
            uint16_t addr = (uint16_t)(this->IX + this->instr.displacement);
            this->D = ReadMemory(addr);
        }

        void Ld_d_hx()
        {
            this->D = this->HX;
        }

        void Ld_d_lx()
        {
            this->D = this->LX;
        }

        void Ld_d_iy_d_ind()
        {
            uint16_t addr = (uint16_t)(this->IY + this->instr.displacement);
            this->D = ReadMemory(addr);
        }

        void Ld_d_hy()
        {
            this->D = this->HY;
        }

        void Ld_d_ly()
        {
            this->D = this->LY;
        }

        void Ld_e_r()
        {
            this->E = this->reg[(int)this->instr._register];
        }

        void Ld_e_n()
        {
            this->E = this->instr.immediate;
        }

        void Ld_e_hl_ind()
        {
            this->E = ReadMemory(this->HL);
        }

        void Ld_e_ix_d_ind()
        {
            uint16_t addr = (uint16_t)(this->IX + this->instr.displacement);
            this->E = ReadMemory(addr);
        }

        void Ld_e_hx()
        {
            this->E = this->HX;
        }

        void Ld_e_lx()
        {
            this->E = this->LX;
        }

        void Ld_e_iy_d_ind()
        {
            uint16_t addr = (uint16_t)(this->IY + this->instr.displacement);
            this->E = ReadMemory(addr);
        }

        void Ld_e_hy()
        {
            this->E = this->HY;
        }

        void Ld_e_ly()
        {
            this->E = this->LY;
        }

        void Ld_h_r()
        {
            this->H = this->reg[(int)this->instr._register];
        }

        void Ld_h_n()
        {
            this->H = this->instr.immediate;
        }

        void Ld_h_hl_ind()
        {
            this->H = ReadMemory(this->HL);
        }

        void Ld_h_ix_d_ind()
        {
            uint16_t addr = (uint16_t)(this->IX + this->instr.displacement);
            this->H = ReadMemory(addr);
        }

        void Ld_h_iy_d_ind()
        {
            uint16_t addr = (uint16_t)(this->IY + this->instr.displacement);
            this->H = ReadMemory(addr);
        }

        void Ld_l_r()
        {
            this->L = this->reg[(int)this->instr._register];
        }

        void Ld_l_n()
        {
            this->L = this->instr.immediate;
        }

        void Ld_l_hl_ind()
        {
            this->L = ReadMemory(this->HL);
        }

        void Ld_l_ix_d_ind()
        {
            uint16_t addr = (uint16_t)(this->IX + this->instr.displacement);
            this->L = ReadMemory(addr);
        }

        void Ld_l_iy_d_ind()
        {
            uint16_t addr = (uint16_t)(this->IY + this->instr.displacement);
            this->L = ReadMemory(addr);
        }

        void Ld_bc_nn_ind()
        {
            this->set_BC(ReadMemory16(this->instr.immediateExtended));
        }

        void Ld_bc_nn()
        {
            this->set_BC(this->instr.immediateExtended);
        }

        void Ld_de_nn_ind()
        {
            this->set_DE(ReadMemory16(this->instr.immediateExtended));
        }

        void Ld_de_nn()
        {
            this->set_DE(this->instr.immediateExtended);
        }

        void Ld_hl_nn_ind()
        {
            this->set_HL(ReadMemory16(this->instr.immediateExtended));
        }

        void Ld_hl_nn()
        {
            this->set_HL(this->instr.immediateExtended);
        }

        void Ld_sp_nn_ind()
        {
            this->SP = ReadMemory16(this->instr.immediateExtended);
        }

        void Ld_sp_hl()
        {
            this->SP = this->HL;
        }

        void Ld_sp_ix()
        {
            this->SP = this->IX;
        }

        void Ld_sp_iy()
        {
            this->SP = this->IY;
        }

        void Ld_sp_nn()
        {
            this->SP = this->instr.immediateExtended;
        }

        void Ld_hx_n()
        {
            this->set_HX(this->instr.immediate);
        }

        void Ld_hx_a()
        {
            this->set_HX(this->A);
        }

        void Ld_hx_b()
        {
            this->set_HX(this->B);
        }

        void Ld_hx_c()
        {
            this->set_HX(this->C);
        }

        void Ld_hx_d()
        {
            this->set_HX(this->D);
        }

        void Ld_hx_e()
        {
            this->set_HX(this->E);
        }

        void Ld_hx_hx()
        {
            // do nothing
        }

        void Ld_hx_lx()
        {
            this->set_HX(this->LX);
        }

        void Ld_lx_n()
        {
            this->set_LX(this->instr.immediate);
        }

        void Ld_lx_a()
        {
            this->set_LX(this->A);
        }

        void Ld_lx_b()
        {
            this->set_LX(this->B);
        }

        void Ld_lx_c()
        {
            this->set_LX(this->C);
        }

        void Ld_lx_d()
        {
            this->set_LX(this->D);
        }

        void Ld_lx_e()
        {
            this->set_LX(this->E);
        }

        void Ld_lx_hx()
        {
            this->set_LX(this->HX);
        }

        void Ld_lx_lx()
        {
            // do nothing
        }

        void Ld_ix_nn_ind()
        {
            this->IX = ReadMemory16(this->instr.immediateExtended);
        }

        void Ld_ix_nn()
        {
            this->IX = this->instr.immediateExtended;
        }

        void Ld_hy_n()
        {
            this->set_HY(this->instr.immediate);
        }

        void Ld_hy_a()
        {
            this->set_HY(this->A);
        }

        void Ld_hy_b()
        {
            this->set_HY(this->B);
        }

        void Ld_hy_c()
        {
            this->set_HY(this->C);
        }

        void Ld_hy_d()
        {
            this->set_HY(this->D);
        }

        void Ld_hy_e()
        {
            this->set_HY(this->E);
        }

        void Ld_hy_hy()
        {
            // do nothing
        }

        void Ld_hy_ly()
        {
            this->set_HY(this->LY);
        }

        void Ld_ly_n()
        {
            this->set_LY(this->instr.immediate);
        }

        void Ld_ly_a()
        {
            this->set_LY(this->A);
        }

        void Ld_ly_b()
        {
            this->set_LY(this->B);
        }

        void Ld_ly_c()
        {
            this->set_LY(this->C);
        }

        void Ld_ly_d()
        {
            this->set_LY(this->D);
        }

        void Ld_ly_e()
        {
            this->set_LY(this->E);
        }

        void Ld_ly_hy()
        {
            this->set_LY(this->HY);
        }

        void Ld_ly_ly()
        {
            // do nothing
        }

        void Ld_iy_nn_ind()
        {
            this->IY = ReadMemory16(this->instr.immediateExtended);
        }

        void Ld_iy_nn()
        {
            this->IY = this->instr.immediateExtended;
        }

        void Ld_hl_ind_r()
        {
            WriteMemory(this->HL, this->reg[(int)this->instr._register]);
        }

        void Ld_hl_ind_n()
        {
            WriteMemory(this->HL, this->instr.immediate);
        }

        void Ld_bc_ind_a()
        {
            WriteMemory(this->BC, this->A);
        }

        void Ld_de_ind_a()
        {
            WriteMemory(this->DE, this->A);
        }

        void Ld_nn_ind_a()
        {
            WriteMemory(this->instr.immediateExtended, this->A);
        }

        void Ld_nn_ind_bc()
        {
            WriteMemory16(this->instr.immediateExtended, this->BC);
        }

        void Ld_nn_ind_de()
        {
            WriteMemory16(this->instr.immediateExtended, this->DE);
        }

        void Ld_nn_ind_hl()
        {
            WriteMemory16(this->instr.immediateExtended, this->HL);
        }

        void Ld_nn_ind_sp()
        {
            WriteMemory16(this->instr.immediateExtended, this->SP);
        }

        void Ld_nn_ind_ix()
        {
            WriteMemory16(this->instr.immediateExtended, this->IX);
        }

        void Ld_nn_ind_iy()
        {
            WriteMemory16(this->instr.immediateExtended, this->IY);
        }

        void Ld_ix_d_ind_r()
        {
            uint16_t addr = (uint16_t)(this->IX + this->instr.displacement);
            WriteMemory(addr, this->reg[(int)this->instr._register]);
        }

        void Ld_ix_d_ind_n()
        {
            uint16_t addr = (uint16_t)(this->IX + this->instr.displacement);
            WriteMemory(addr, this->instr.immediate);
        }

        void Ld_iy_d_ind_r()
        {
            uint16_t addr = (uint16_t)(this->IY + this->instr.displacement);
            WriteMemory(addr, this->reg[(int)this->instr._register]);
        }

        void Ld_iy_d_ind_n()
        {
            uint16_t addr = (uint16_t)(this->IY + this->instr.displacement);
            WriteMemory(addr, this->instr.immediate);
        }

        void Ldd()
        {
           uint8_t val = ReadMemory(this->HL);
            WriteMemory(this->DE, val);
            this->Dec_de();
            this->Dec_hl();
            this->Dec_bc();
            this->set_HFlag(false);
            this->set_NFlag(false);
            this->set_PVFlag(this->BC != 0);
        }

        void Lddr()
        {
            WriteMemory(this->DE, ReadMemory(this->HL));
            this->Dec_de();
            this->Dec_hl();
            this->Dec_bc();

            this->set_HFlag(false);
            this->set_NFlag(false);
            this->set_PVFlag(false);

            if (this->BC != 0)
            {
                // repeat
                this->PC -= 2;
                cyclesLeft -= cc_ex[this->instr.opcode];
            }
        }

        void Ldi()
        {
            WriteMemory(this->DE, ReadMemory(this->HL));
            this->Inc_de();
            this->Inc_hl();
            this->Dec_bc();
            this->set_HFlag(false);
            this->set_NFlag(false);
            this->set_PVFlag(this->BC != 0);
        }

        void Ldir()
        {
            WriteMemory(this->DE, ReadMemory(this->HL));
            this->Inc_de();
            this->Inc_hl();
            this->Dec_bc();
            this->set_HFlag(false);
            this->set_NFlag(false);
            this->set_PVFlag(false);

            if (this->BC != 0)
            {
                // repeat
                this->PC -= 2;
                cyclesLeft -= cc_ex[this->instr.opcode];
            }
        }

        void Neg()
        {
           uint8_t val = this->A;
            this->A = (uint8_t)(-val);
            this->F = (uint8_t)(Get_SZHVC_sub_flag(0, this->A, 0) | N_mask);
        }

        void Nop()
        {
            // do nothing
        }

        void Or_a_r()
        {
            this->A = (uint8_t)(this->A | this->reg[(int)this->instr._register]);
            this->F = flag_SZP[this->A];
        }

        void Or_a_n()
        {
            this->A = (uint8_t)(this->A | this->instr.immediate);
            this->F = flag_SZP[this->A];
        }

        void Or_a_hx()
        {
            this->A = (uint8_t)(this->A | this->HX);
            this->F = flag_SZP[this->A];
        }

        void Or_a_lx()
        {
            this->A = (uint8_t)(this->A | this->LX);
            this->F = flag_SZP[this->A];
        }

        void Or_a_hy()
        {
            this->A = (uint8_t)(this->A | this->HY);
            this->F = flag_SZP[this->A];
        }

        void Or_a_ly()
        {
            this->A = (uint8_t)(this->A | this->LY);
            this->F = flag_SZP[this->A];
        }

        void Or_a_hl_ind()
        {
            this->A = (uint8_t)(this->A | ReadMemory(this->HL));
            this->F = flag_SZP[this->A];
        }

        void Or_a_ix_d_ind()
        {
            uint16_t addr = (uint16_t)(this->IX + this->instr.displacement);
           uint8_t val = ReadMemory(addr);
            this->A = (uint8_t)(this->A | val);
            this->F = flag_SZP[this->A];
        }

        void Or_a_iy_d_ind()
        {
            uint16_t addr = (uint16_t)(this->IY + this->instr.displacement);
           uint8_t val = ReadMemory(addr);
            this->A = (uint8_t)(this->A | val);
            this->F = flag_SZP[this->A];
        }

        void Out_n_a()
        {
            ioHandler.Write(this->instr.immediate, this->A);
        }

        void Out_c_r()
        {
            int registerIndex = (this->instr.opcode & 0x38) >> 3;
            ioHandler.Write(this->C, this->reg[registerIndex]);
        }

        void Out_c_0()
        {
            ioHandler.Write(this->C, 0);
        }

        void Outd()
        {
           uint8_t val = ReadMemory(this->HL);
            this->B--;
            ioHandler.Write(this->C, val);
            this->Dec_hl();

            // code review
            this->F = flag_SZ[this->B];
            int t = this->L + val;
            if ((val & S_mask) != 0)
            {
                this->F |= N_mask;
            }

            if ((t & 0x100) != 0)
            {
                this->F |= (uint8_t)(H_mask | C_mask);
            }

            this->F |= (uint8_t)(flag_SZP[(t & 0x07) ^ this->B] & PV_mask);
        }

        void Otdr()
        {
           uint8_t val = ReadMemory(this->HL);
            this->B--;
            ioHandler.Write(this->C, val);
            this->Dec_hl();

            // code review
            this->F = flag_SZ[this->B];
            int t = this->L + val;
            if ((val & S_mask) != 0)
            {
                this->F |= N_mask;
            }

            if ((t & 0x100) != 0)
            {
                this->F |= (uint8_t)(H_mask | C_mask);
            }

            this->F |= (uint8_t)(flag_SZP[(t & 0x07) ^ this->B] & PV_mask);

            if (this->B != 0)
            {
                // repeat
                this->PC -= 2;
                cyclesLeft -= cc_ex[this->instr.opcode];
            }
        }

        void Outi()
        {
           uint8_t val = ReadMemory(this->HL);
            this->B--;
            ioHandler.Write(this->C, val);
            this->Inc_hl();

            // code review
            this->F = flag_SZ[this->B];
            int t = this->L + val;
            if ((val & S_mask) != 0)
            {
                this->F |= N_mask;
            }

            if ((t & 0x100) != 0)
            {
                this->F |= (uint8_t)(H_mask | C_mask);
            }

            this->F |= (uint8_t)(flag_SZP[(t & 0x07) ^ this->B] & PV_mask);
        }

        void Otir()
        {
           uint8_t val = ReadMemory(this->HL);
            this->B--;
            ioHandler.Write(this->C, val);
            this->Inc_hl();

            // code review
            this->F = flag_SZ[this->B];
            int t = this->L + val;
            if ((val & S_mask) != 0)
            {
                this->F |= N_mask;
            }

            if ((t & 0x100) != 0)
            {
                this->F |= (uint8_t)(H_mask | C_mask);
            }

            this->F |= (uint8_t)(flag_SZP[(t & 0x07) ^ this->B] & PV_mask);

            if (this->B != 0)
            {
                // repeat
                this->PC -= 2;
                cyclesLeft -= cc_ex[this->instr.opcode];
            }
        }

        void Pop_af()
        {
            this->F = ReadMemory(this->SP++);
            this->A = ReadMemory(this->SP++);
        }

        void Pop_bc()
        {
            this->set_BC(ReadMemory16(this->SP));
            this->SP += 2;
        }

        void Pop_de()
        {
            this->set_DE(ReadMemory16(this->SP));
            this->SP += 2;
        }

        void Pop_hl()
        {
            this->set_HL(ReadMemory16(this->SP));
            this->SP += 2;
        }

        void Pop_ix()
        {
            this->IX = ReadMemory16(this->SP);
            this->SP += 2;
        }

        void Pop_iy()
        {
            this->IY = ReadMemory16(this->SP);
            this->SP += 2;
        }

        void Push_af()
        {
            WriteMemory(--this->SP, this->A);
            WriteMemory(--this->SP, this->F);
        }

        void Push_bc()
        {
            this->SP -= 2;
            WriteMemory16(this->SP, this->BC);
        }

        void Push_de()
        {
            this->SP -= 2;
            WriteMemory16(this->SP, this->DE);
        }

        void Push_hl()
        {
            this->SP -= 2;
            WriteMemory16(this->SP, this->HL);
        }

        void Push_ix()
        {
            this->SP -= 2;
            WriteMemory16(this->SP, this->IX);
        }

        void Push_iy()
        {
            this->SP -= 2;
            WriteMemory16(this->SP, this->IY);
        }

        void Push_pc()
        {
            this->SP -= 2;
            WriteMemory16(this->SP, this->PC);
        }

        void Res_bit_r()
        {
            int bit = (this->instr.opcode >> 3) & 0x07;
            this->reg[(int)this->instr._register] &= rstMask[bit];
        }

        void Res_bit_hl_ind()
        {
            int bit = (this->instr.opcode >> 3) & 0x07;
           uint8_t val = ReadMemory(this->HL);
            WriteMemory(this->HL, (uint8_t)(val & rstMask[bit]));
        }

       uint8_t Res_bit_ix()
        {
            uint16_t addr = (uint16_t)(this->IX + this->instr.displacement);
           uint8_t val = ReadMemory(addr);
            int bit = (this->instr.opcode >> 3) & 0x07;
           uint8_t result = (uint8_t)(val & rstMask[bit]);
            WriteMemory(addr, result);
            return result;
        }

       uint8_t Res_bit_iy()
        {
            uint16_t addr = (uint16_t)(this->IY + this->instr.displacement);
           uint8_t val = ReadMemory(addr);
            int bit = (this->instr.opcode >> 3) & 0x07;
           uint8_t result = (uint8_t)(val & rstMask[bit]);
            WriteMemory(addr, result);
            return result;
        }

        void Res_bit_ix_d_ind()
        {
            Res_bit_ix();
        }

        void Res_bit_ix_d_ind_Ld_a()
        {
            this->A = Res_bit_ix();
        }

        void Res_bit_ix_d_ind_Ld_b()
        {
            this->B = Res_bit_ix();
        }

        void Res_bit_ix_d_ind_Ld_c()
        {
            this->C = Res_bit_ix();
        }

        void Res_bit_ix_d_ind_Ld_d()
        {
            this->D = Res_bit_ix();
        }

        void Res_bit_ix_d_ind_Ld_e()
        {
            this->E = Res_bit_ix();
        }

        void Res_bit_ix_d_ind_Ld_h()
        {
            this->H = Res_bit_ix();
        }

        void Res_bit_ix_d_ind_Ld_l()
        {
            this->L = Res_bit_ix();
        }

        void Res_bit_iy_d_ind()
        {
            Res_bit_iy();
        }

        void Res_bit_iy_d_ind_Ld_a()
        {
            this->A = Res_bit_iy();
        }

        void Res_bit_iy_d_ind_Ld_b()
        {
            this->B = Res_bit_iy();
        }

        void Res_bit_iy_d_ind_Ld_c()
        {
            this->C = Res_bit_iy();
        }

        void Res_bit_iy_d_ind_Ld_d()
        {
            this->D = Res_bit_iy();
        }

        void Res_bit_iy_d_ind_Ld_e()
        {
            this->E = Res_bit_iy();
        }

        void Res_bit_iy_d_ind_Ld_h()
        {
            this->H = Res_bit_iy();
        }

        void Res_bit_iy_d_ind_Ld_l()
        {
            this->L = Res_bit_iy();
        }

        void Ret()
        {
            this->PC = ReadMemory16(this->SP);
            this->SP += 2;
        }

        void Ret_c()
        {
            if (this->c != 0)
            {
                Ret();
                cyclesLeft -= cc_ex[this->instr.opcode];
            }
        }

        void Ret_nc()
        {
            if (this->c == 0)
            {
                Ret();
                cyclesLeft -= cc_ex[this->instr.opcode];
            }
        }

        void Ret_m()
        {
            if (this->s)
            {
                Ret();
                cyclesLeft -= cc_ex[this->instr.opcode];
            }
        }

        void Ret_p()
        {
            if (! this->s)
            {
                Ret();
                cyclesLeft -= cc_ex[this->instr.opcode];
            }
        }

        void Ret_z()
        {
            if (this->z)
            {
                Ret();
                cyclesLeft -= cc_ex[this->instr.opcode];
            }
        }

        void Ret_nz()
        {
            if (! this->z)
            {
                Ret();
                cyclesLeft -= cc_ex[this->instr.opcode];
            }
        }

        void Ret_pe()
        {
            if (this->p)
            {
                Ret();
                cyclesLeft -= cc_ex[this->instr.opcode];
            }
        }

        void Ret_po()
        {
            if (! this->p)
            {
                Ret();
                cyclesLeft -= cc_ex[this->instr.opcode];
            }
        }

        void Reti()
        {
            // Return from interrupt
            Ret();

            // according to http://www.msxnet.org/tech/z80-documented.pdf
            this->IFF1 = this->IFF2;
        }

        void Retn()
        {
            // Return from non maskable interrupt
            Ret();

            this->IFF1 = this->IFF2;
        }

        void Rl_a()
        {
           uint8_t newCarry = (uint8_t)((this->A & 0x80) >> 7);
            this->A = (uint8_t)(((this->A & 0x7f) << 1) | this->c);
            this->F = (uint8_t)((this->F & (S_mask | Z_mask | PV_mask)) | newCarry);
        }

        void Rl_r()
        {
           uint8_t val = this->reg[(int)this->instr._register];
           uint8_t newCarry = (uint8_t)((val & 0x80) >> 7);
           uint8_t result = (uint8_t)(((val & 0x7f) << 1) | this->c);
            this->reg[(int)this->instr._register] = result;
            this->F = (uint8_t)(flag_SZP[result] | newCarry);
        }

        void Rl_hl_ind()
        {
           uint8_t val = ReadMemory(this->HL);
           uint8_t newCarry = (uint8_t)((val & 0x80) >> 7);
           uint8_t result = (uint8_t)(((val & 0x7f) << 1) | this->c);
            WriteMemory(this->HL, result);
            this->F = (uint8_t)(flag_SZP[result] | newCarry);
        }

       uint8_t Rl_ind(uint16_t addr)
        {
           uint8_t val = ReadMemory(addr);
           uint8_t newCarry = (uint8_t)((val & 0x80) >> 7);
           uint8_t result = (uint8_t)(((val & 0x7f) << 1) | this->c);
            WriteMemory(addr, result);
            this->F = (uint8_t)(flag_SZP[result] | newCarry);
            return result;
        }

        void Rl_ix_d_ind()
        {
            Rl_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Rl_ix_d_ind_Ld_a()
        {
            this->A = Rl_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Rl_ix_d_ind_Ld_b()
        {
            this->B = Rl_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Rl_ix_d_ind_Ld_c()
        {
            this->C = Rl_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Rl_ix_d_ind_Ld_d()
        {
            this->D = Rl_ind((uint16_t)(this->IX + this->instr.displacement));
        }
        void Rl_ix_d_ind_Ld_e()
        {
            this->E = Rl_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Rl_ix_d_ind_Ld_h()
        {
            this->H = Rl_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Rl_ix_d_ind_Ld_l()
        {
            this->L = Rl_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Rl_iy_d_ind()
        {
            Rl_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Rl_iy_d_ind_Ld_a()
        {
            this->A = Rl_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Rl_iy_d_ind_Ld_b()
        {
            this->B = Rl_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Rl_iy_d_ind_Ld_c()
        {
            this->C = Rl_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Rl_iy_d_ind_Ld_d()
        {
            this->D = Rl_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Rl_iy_d_ind_Ld_e()
        {
            this->E = Rl_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Rl_iy_d_ind_Ld_h()
        {
            this->H = Rl_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Rl_iy_d_ind_Ld_l()
        {
            this->L = Rl_ind((uint16_t)(this->IY + this->instr.displacement));
        }

       uint8_t Rlc_ind(uint16_t addr)
        {
           uint8_t val = ReadMemory(addr);
           uint8_t newCarry = (uint8_t)((val & 0x80) >> 7);
           uint8_t result = (uint8_t)(((val & 0x7f) << 1) | newCarry);
            WriteMemory(addr, result);
            this->F = (uint8_t)(flag_SZP[result] | newCarry);
            return result;
        }

        void Rlc_ix_d_ind()
        {
            Rlc_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Rlc_ix_d_ind_Ld_a()
        {
            this->A = Rlc_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Rlc_ix_d_ind_Ld_b()
        {
            this->B = Rlc_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Rlc_ix_d_ind_Ld_c()
        {
            this->C = Rlc_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Rlc_ix_d_ind_Ld_d()
        {
            this->D = Rlc_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Rlc_ix_d_ind_Ld_e()
        {
            this->E = Rlc_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Rlc_ix_d_ind_Ld_h()
        {
            this->H = Rlc_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Rlc_ix_d_ind_Ld_l()
        {
            this->L = Rlc_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Rlc_iy_d_ind()
        {
            Rlc_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Rlc_iy_d_ind_Ld_a()
        {
            this->A = Rlc_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Rlc_iy_d_ind_Ld_b()
        {
            this->B = Rlc_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Rlc_iy_d_ind_Ld_c()
        {
            this->C = Rlc_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Rlc_iy_d_ind_Ld_d()
        {
            this->D = Rlc_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Rlc_iy_d_ind_Ld_e()
        {
            this->E = Rlc_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Rlc_iy_d_ind_Ld_h()
        {
            this->H = Rlc_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Rlc_iy_d_ind_Ld_l()
        {
            this->L = Rlc_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Rlc_a()
        {
           uint8_t newCarry = (uint8_t)((this->A & 0x80) >> 7);
            this->A = (uint8_t)(((this->A & 0x7f) << 1) | newCarry);
            this->F = (uint8_t)((this->F & (S_mask | Z_mask | PV_mask)) | newCarry);
        }

        void Rlc_r()
        {
           uint8_t val = this->reg[(int)this->instr._register];
           uint8_t newCarry = (uint8_t)((val & 0x80) >> 7);
           uint8_t result = (uint8_t)(((val & 0x7f) << 1) | newCarry);
            this->reg[(int)this->instr._register] = result;
            this->F = (uint8_t)(flag_SZP[result] | newCarry);
        }

        void Rlc_hl_ind()
        {
           uint8_t val = ReadMemory(this->HL);
           uint8_t newCarry = (uint8_t)((val & 0x80) >> 7);
           uint8_t result = (uint8_t)(((val & 0x7f) << 1) | newCarry);
            WriteMemory(this->HL, result);
            this->F = (uint8_t)(flag_SZP[result] | newCarry);
        }

        void Rld_a_hl()
        {
           uint8_t val = ReadMemory(this->HL);
           uint8_t a = this->A;
            WriteMemory(this->HL, (uint8_t)(((val & 0x0f) << 4) | (a & 0x0f)));
            this->A = (uint8_t)((a & 0xf0) | (val >> 4));
            this->F = (uint8_t)(flag_SZP[this->A] | this->c);
        }

        void Rr_a()
        {
           uint8_t newCarry = (uint8_t)(this->A & 0x01);
            this->A = (uint8_t)((this->A >> 1) | (this->c << 7));
            this->F = (uint8_t)((this->F & (S_mask | Z_mask | PV_mask)) | newCarry);
        }

        void Rr_r()
        {
           uint8_t val = this->reg[(int)this->instr._register];
           uint8_t newCarry = (uint8_t)(val & 0x01);
           uint8_t result = (uint8_t)((val >> 1) | (this->c << 7));
            this->reg[(int)this->instr._register] = result;
            this->F = (uint8_t)(flag_SZP[result] | newCarry);
        }

        void Rr_hl_ind()
        {
           uint8_t val = ReadMemory(this->HL);
           uint8_t newCarry = (uint8_t)(val & 0x01);
           uint8_t result = (uint8_t)((val >> 1) | (this->c << 7));
            WriteMemory(this->HL, result);
            this->F = (uint8_t)(flag_SZP[result] | newCarry);
        }

       uint8_t Rr_ind(uint16_t addr)
        {
           uint8_t val = ReadMemory(addr);
           uint8_t newCarry = (uint8_t)(val & 0x01);
           uint8_t result = (uint8_t)((val >> 1) | (this->c << 7));
            WriteMemory(addr, result);
            this->F = (uint8_t)(flag_SZP[result] | newCarry);
            return result;
        }

        void Rr_ix_d_ind()
        {
            Rr_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Rr_ix_d_ind_Ld_a()
        {
            this->A = Rr_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Rr_ix_d_ind_Ld_b()
        {
            this->B = Rr_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Rr_ix_d_ind_Ld_c()
        {
            this->C = Rr_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Rr_ix_d_ind_Ld_d()
        {
            this->D = Rr_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Rr_ix_d_ind_Ld_e()
        {
            this->E = Rr_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Rr_ix_d_ind_Ld_h()
        {
            this->H = Rr_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Rr_ix_d_ind_Ld_l()
        {
            this->L = Rr_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Rr_iy_d_ind()
        {
            Rr_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Rr_iy_d_ind_Ld_a()
        {
            this->A = Rr_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Rr_iy_d_ind_Ld_b()
        {
            this->B = Rr_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Rr_iy_d_ind_Ld_c()
        {
            this->C = Rr_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Rr_iy_d_ind_Ld_d()
        {
            this->D = Rr_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Rr_iy_d_ind_Ld_e()
        {
            this->E = Rr_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Rr_iy_d_ind_Ld_h()
        {
            this->H = Rr_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Rr_iy_d_ind_Ld_l()
        {
            this->L = Rr_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Rrc_a()
        {
           uint8_t newCarry = (uint8_t)(this->A & 0x01);
            this->A = (uint8_t)((this->A >> 1) | (newCarry << 7));
            this->F = (uint8_t)((this->F & (S_mask | Z_mask | PV_mask)) | newCarry);
        }

        void Rrc_r()
        {
           uint8_t val = this->reg[(int)this->instr._register];
           uint8_t newCarry = (uint8_t)(val & 0x01);
           uint8_t result = (uint8_t)((val >> 1) | (newCarry << 7));
            this->reg[(int)this->instr._register] = result;
            this->F = (uint8_t)(flag_SZP[result] | newCarry);
        }

        void Rrc_hl_ind()
        {
           uint8_t val = ReadMemory(this->HL);
           uint8_t newCarry = (uint8_t)(val & 0x01);
           uint8_t result = (uint8_t)((val >> 1) | (newCarry << 7));
            WriteMemory(this->HL, result);
            this->F = (uint8_t)(flag_SZP[result] | newCarry);
        }

       uint8_t Rrc_ind(uint16_t addr)
        {
           uint8_t val = ReadMemory(addr);
           uint8_t newCarry = (uint8_t)(val & 0x01);
           uint8_t result = (uint8_t)((val >> 1) | (newCarry << 7));
            WriteMemory(addr, result);
            this->F = (uint8_t)(flag_SZP[result] | newCarry);
            return result;
        }

        void Rrc_ix_d_ind()
        {
            Rrc_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Rrc_ix_d_ind_Ld_a()
        {
            this->A = Rrc_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Rrc_ix_d_ind_Ld_b()
        {
            this->B = Rrc_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Rrc_ix_d_ind_Ld_c()
        {
            this->C = Rrc_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Rrc_ix_d_ind_Ld_d()
        {
            this->D = Rrc_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Rrc_ix_d_ind_Ld_e()
        {
            this->E = Rrc_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Rrc_ix_d_ind_Ld_h()
        {
            this->H = Rrc_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Rrc_ix_d_ind_Ld_l()
        {
            this->L = Rrc_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Rrc_iy_d_ind()
        {
            Rrc_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Rrc_iy_d_ind_Ld_a()
        {
            this->A = Rrc_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Rrc_iy_d_ind_Ld_b()
        {
            this->B = Rrc_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Rrc_iy_d_ind_Ld_c()
        {
            this->C = Rrc_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Rrc_iy_d_ind_Ld_d()
        {
            this->D = Rrc_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Rrc_iy_d_ind_Ld_e()
        {
            this->E = Rrc_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Rrc_iy_d_ind_Ld_h()
        {
            this->H = Rrc_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Rrc_iy_d_ind_Ld_l()
        {
            this->L = Rrc_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Rrd_a_hl()
        {
           uint8_t val = ReadMemory(this->HL);
           uint8_t a = this->A;
            WriteMemory(this->HL, (uint8_t)(((a & 0x0f) << 4) | (val >> 4)));
            this->A = (uint8_t)((a & 0xf0) | (val & 0x0f));
            this->F = (uint8_t)(flag_SZP[this->A] | this->c);
        }

        void Rst()
        {
           uint8_t pc = (uint8_t)(this->instr.opcode & 0x38);
            WriteMemory(--this->SP, (uint8_t)(this->PC >> 8));
            WriteMemory(--this->SP, (uint8_t)(this->PC & 0xff));
            this->PC = pc; // jump to page-0 address
        }

        void Sbc_a_r()
        {
           uint8_t d = this->reg[(int)this->instr._register];
           uint8_t result = (uint8_t)((this->A - d - this->c) & 0xff);
            this->F = Get_SZHVC_sub_flag(this->A, result, this->c);
            this->A = result;
        }

        void Sbc_a_n()
        {
           uint8_t d = this->instr.immediate;
           uint8_t result = (uint8_t)((this->A - d - this->c) & 0xff);
            this->F = Get_SZHVC_sub_flag(this->A, result, this->c);
            this->A = result;
        }

        void Sbc_a_hx()
        {
           uint8_t d = this->HX;
           uint8_t result = (uint8_t)((this->A - d - this->c) & 0xff);
            this->F = Get_SZHVC_sub_flag(this->A, result, this->c);
            this->A = result;
        }

        void Sbc_a_lx()
        {
           uint8_t d = this->LX;
           uint8_t result = (uint8_t)((this->A - d - this->c) & 0xff);
            this->F = Get_SZHVC_sub_flag(this->A, result, this->c);
            this->A = result;
        }

        void Sbc_a_hy()
        {
           uint8_t d = this->HY;
           uint8_t result = (uint8_t)((this->A - d - this->c) & 0xff);
            this->F = Get_SZHVC_sub_flag(this->A, result, this->c);
            this->A = result;
        }

        void Sbc_a_ly()
        {
           uint8_t d = this->LY;
           uint8_t result = (uint8_t)((this->A - d - this->c) & 0xff);
            this->F = Get_SZHVC_sub_flag(this->A, result, this->c);
            this->A = result;
        }

        void Sbc_a_hl_ind()
        {
           uint8_t val = ReadMemory(this->HL);
           uint8_t result = (uint8_t)((this->A - val - this->c) & 0xff);
            this->F = Get_SZHVC_sub_flag(this->A, result, this->c);
            this->A = result;
        }

        void Sbc_a_ix_d_ind()
        {
            uint16_t addr = (uint16_t)(this->IX + this->instr.displacement);
           uint8_t val = ReadMemory(addr);
           uint8_t result = (uint8_t)((this->A - val - this->c) & 0xff);
            this->F = Get_SZHVC_sub_flag(this->A, result, this->c);
            this->A = result;
        }

        void Sbc_a_iy_d_ind()
        {
            uint16_t addr = (uint16_t)(this->IY + this->instr.displacement);
           uint8_t val = ReadMemory(addr);
           uint8_t result = (uint8_t)((this->A - val - this->c) & 0xff);
            this->F = Get_SZHVC_sub_flag(this->A, result, this->c);
            this->A = result;
        }

        void Sbc_hl_bc()
        {
            uint16_t d = this->BC;
            int result = this->HL - d - this->c;
            SetSbcOpFlags(this->HL, d, result);
            this->set_HL(result & 0xffff);
        }

        void Sbc_hl_de()
        {
            uint16_t d = this->DE;
            int result = this->HL - d - this->c;
            SetSbcOpFlags(this->HL, d, result);
            this->set_HL(result & 0xffff);
        }

        void Sbc_hl_hl()
        {
            uint16_t d = this->HL;
            int result = this->HL - d - this->c;
            SetSbcOpFlags(this->HL, d, result);
            this->set_HL(result & 0xffff);
        }

        void Sbc_hl_sp()
        {
            uint16_t d = this->SP;
            int result = this->HL - d - this->c;
            SetSbcOpFlags(this->HL, d, result);
            this->set_HL(result & 0xffff);
        }

        void Scf()
        {
            this->F = (uint8_t)((this->F & (S_mask | Z_mask | PV_mask)) | C_mask);
        }

        void Set_bit_r()
        {
            int bit = (this->instr.opcode >> 3) & 0x07;
            this->reg[(int)this->instr._register] |= (uint8_t)(1 << bit);
        }

        void Set_bit_hl_ind()
        {
            int bit = (this->instr.opcode >> 3) & 0x07;
            WriteMemory(this->HL, (uint8_t)(ReadMemory(this->HL) | (1 << bit)));
        }

       uint8_t Set_bit_ix()
        {
            uint16_t addr = (uint16_t)(this->IX + this->instr.displacement);
            int bit = (this->instr.opcode >> 3) & 0x07;
           uint8_t val = ReadMemory(addr);
           uint8_t result = (uint8_t)(val | (uint8_t)(1 << bit));
            WriteMemory(addr, result);
            return result;
        }

       uint8_t Set_bit_iy()
        {
            uint16_t addr = (uint16_t)(this->IY + this->instr.displacement);
            int bit = (this->instr.opcode >> 3) & 0x07;
           uint8_t val = ReadMemory(addr);
           uint8_t result = (uint8_t)(val | (uint8_t)(1 << bit));
            WriteMemory(addr, result);
            return result;
        }

        void Set_bit_ix_d_ind()
        {
            Set_bit_ix();
        }

        void Set_bit_ix_d_ind_Ld_a()
        {
            this->A = Set_bit_ix();
        }

        void Set_bit_ix_d_ind_Ld_b()
        {
            this->B = Set_bit_ix();
        }

        void Set_bit_ix_d_ind_Ld_c()
        {
            this->C = Set_bit_ix();
        }

        void Set_bit_ix_d_ind_Ld_d()
        {
            this->D = Set_bit_ix();
        }

        void Set_bit_ix_d_ind_Ld_e()
        {
            this->E = Set_bit_ix();
        }

        void Set_bit_ix_d_ind_Ld_h()
        {
            this->H = Set_bit_ix();
        }

        void Set_bit_ix_d_ind_Ld_l()
        {
            this->L = Set_bit_ix();
        }

        void Set_bit_iy_d_ind()
        {
            Set_bit_iy();
        }

        void Set_bit_iy_d_ind_Ld_a()
        {
            this->A = Set_bit_iy();
        }

        void Set_bit_iy_d_ind_Ld_b()
        {
            this->B = Set_bit_iy();
        }

        void Set_bit_iy_d_ind_Ld_c()
        {
            this->C = Set_bit_iy();
        }

        void Set_bit_iy_d_ind_Ld_d()
        {
            this->D = Set_bit_iy();
        }

        void Set_bit_iy_d_ind_Ld_e()
        {
            this->E = Set_bit_iy();
        }

        void Set_bit_iy_d_ind_Ld_h()
        {
            this->H = Set_bit_iy();
        }

        void Set_bit_iy_d_ind_Ld_l()
        {
            this->L = Set_bit_iy();
        }

        void Sla_r()
        {
           uint8_t val = this->reg[(int)this->instr._register];
           uint8_t newCarry = (uint8_t)((val & 0x80) >> 7);
           uint8_t result = (uint8_t)((val & 0x7f) << 1);
            this->reg[(int)this->instr._register] = result;
            this->F = (uint8_t)(flag_SZP[result] | newCarry);
        }

        void Sla_hl_ind()
        {
           uint8_t val = ReadMemory(this->HL);
           uint8_t newCarry = (uint8_t)((val & 0x80) >> 7);
           uint8_t result = (uint8_t)((val & 0x7f) << 1);
            WriteMemory(this->HL, result);
            this->F = (uint8_t)(flag_SZP[result] | newCarry);
        }

       uint8_t Sla_ind(uint16_t addr)
        {
           uint8_t val = ReadMemory(addr);
           uint8_t newCarry = (uint8_t)((val & 0x80) >> 7);
           uint8_t result = (uint8_t)((val & 0x7f) << 1);
            WriteMemory(addr, result);
            this->F = (uint8_t)(flag_SZP[result] | newCarry);
            return result;
        }

        void Sla_ix_d_ind()
        {
            Sla_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Sla_ix_d_ind_Ld_a()
        {
            this->A = Sla_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Sla_ix_d_ind_Ld_b()
        {
            this->B = Sla_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Sla_ix_d_ind_Ld_c()
        {
            this->C = Sla_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Sla_ix_d_ind_Ld_d()
        {
            this->D = Sla_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Sla_ix_d_ind_Ld_e()
        {
            this->E = Sla_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Sla_ix_d_ind_Ld_h()
        {
            this->H = Sla_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Sla_ix_d_ind_Ld_l()
        {
            this->L = Sla_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Sla_iy_d_ind()
        {
            Sla_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Sla_iy_d_ind_Ld_a()
        {
            this->A = Sla_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Sla_iy_d_ind_Ld_b()
        {
            this->B = Sla_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Sla_iy_d_ind_Ld_c()
        {
            this->C = Sla_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Sla_iy_d_ind_Ld_d()
        {
            this->D = Sla_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Sla_iy_d_ind_Ld_e()
        {
            this->E = Sla_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Sla_iy_d_ind_Ld_h()
        {
            this->H = Sla_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Sla_iy_d_ind_Ld_l()
        {
            this->L = Sla_ind((uint16_t)(this->IY + this->instr.displacement));
        }
        
        void Sra_r()
        {
           uint8_t val = this->reg[(int)this->instr._register];
           uint8_t newCarry = (uint8_t)(val & 0x01);
           uint8_t result = (uint8_t)((val >> 1) | (val & 0x80));
            this->reg[(int)this->instr._register] = result;
            this->F = (uint8_t)(flag_SZP[result] | newCarry);
        }

        void Sra_hl_ind()
        {
           uint8_t val = ReadMemory(this->HL);
           uint8_t newCarry = (uint8_t)(val & 0x01);
           uint8_t result = (uint8_t)((val >> 1) | (val & 0x80));
            WriteMemory(this->HL, result);
            this->F = (uint8_t)(flag_SZP[result] | newCarry);
        }

       uint8_t Sra_ind(uint16_t addr)
        {
           uint8_t val = ReadMemory(addr);
           uint8_t newCarry = (uint8_t)(val & 0x01);
           uint8_t result = (uint8_t)((val >> 1) | (val & 0x80));
            WriteMemory(addr, result);
            this->F = (uint8_t)(flag_SZP[result] | newCarry);
            return result;
        }

        void Sra_ix_d_ind()
        {
            Sra_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Sra_ix_d_ind_Ld_a()
        {
            this->A = Sra_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Sra_ix_d_ind_Ld_b()
        {
            this->B = Sra_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Sra_ix_d_ind_Ld_c()
        {
            this->C = Sra_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Sra_ix_d_ind_Ld_d()
        {
            this->D = Sra_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Sra_ix_d_ind_Ld_e()
        {
            this->E = Sra_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Sra_ix_d_ind_Ld_h()
        {
            this->H = Sra_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Sra_ix_d_ind_Ld_l()
        {
            this->L = Sra_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Sra_iy_d_ind()
        {
            Sra_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Sra_iy_d_ind_Ld_a()
        {
            this->A = Sra_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Sra_iy_d_ind_Ld_b()
        {
            this->B = Sra_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Sra_iy_d_ind_Ld_c()
        {
            this->C = Sra_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Sra_iy_d_ind_Ld_d()
        {
            this->D = Sra_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Sra_iy_d_ind_Ld_e()
        {
            this->E = Sra_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Sra_iy_d_ind_Ld_h()
        {
            this->H = Sra_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Sra_iy_d_ind_Ld_l()
        {
            this->L = Sra_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Sll_r()
        {
           uint8_t val = this->reg[(int)this->instr._register];
           uint8_t newCarry = (uint8_t)((val & 0x80) >> 7);
           uint8_t result = (uint8_t)(((val & 0x7f) << 1) | 0x01); // undocumented
            this->reg[(int)this->instr._register] = result;
            this->F = (uint8_t)(flag_SZP[result] | newCarry);
        }

        void Sll_hl_ind()
        {
           uint8_t val = ReadMemory(this->HL);
           uint8_t newCarry = (uint8_t)((val & 0x80) >> 7);
           uint8_t result = (uint8_t)(((val & 0x7f) << 1) | 0x01); // undocumented
            WriteMemory(this->HL, result);
            this->F = (uint8_t)(flag_SZP[result] | newCarry);
        }

       uint8_t Sll_ind(uint16_t addr)
        {
           uint8_t val = ReadMemory(addr);
           uint8_t newCarry = (uint8_t)((val & 0x80) >> 7);
           uint8_t result = (uint8_t)(((val & 0x7f) << 1) | 0x01); // undocumented
            WriteMemory(addr, result);
            this->F = (uint8_t)(flag_SZP[result] | newCarry);
            return result;
        }

        void Sll_ix_d_ind()
        {
            Sll_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Sll_ix_d_ind_Ld_a()
        {
            this->A = Sll_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Sll_ix_d_ind_Ld_b()
        {
            this->B = Sll_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Sll_ix_d_ind_Ld_c()
        {
            this->C = Sll_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Sll_ix_d_ind_Ld_d()
        {
            this->D = Sll_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Sll_ix_d_ind_Ld_e()
        {
            this->E = Sll_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Sll_ix_d_ind_Ld_h()
        {
            this->H = Sll_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Sll_ix_d_ind_Ld_l()
        {
            this->L = Sll_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Sll_iy_d_ind()
        {
            Sll_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Sll_iy_d_ind_Ld_a()
        {
            this->A = Sll_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Sll_iy_d_ind_Ld_b()
        {
            this->B = Sll_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Sll_iy_d_ind_Ld_c()
        {
            this->C = Sll_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Sll_iy_d_ind_Ld_d()
        {
            this->D = Sll_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Sll_iy_d_ind_Ld_e()
        {
            this->E = Sll_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Sll_iy_d_ind_Ld_h()
        {
            this->H = Sll_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Sll_iy_d_ind_Ld_l()
        {
            this->L = Sll_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Srl_r()
        {
           uint8_t val = this->reg[(int)this->instr._register];
           uint8_t newCarry = (uint8_t)(val & 0x01);
           uint8_t result = (uint8_t)(val >> 1);
            this->reg[(int)this->instr._register] = result;
            this->F = (uint8_t)(flag_SZP[result] | newCarry);
        }

        void Srl_hl_ind()
        {
           uint8_t val = ReadMemory(this->HL);
           uint8_t newCarry = (uint8_t)(val & 0x01);
           uint8_t result = (uint8_t)(val >> 1);
            WriteMemory(this->HL, result);
            this->F = (uint8_t)(flag_SZP[result] | newCarry);
        }

       uint8_t Srl_ind(uint16_t addr)
        {
           uint8_t val = ReadMemory(addr);
           uint8_t newCarry = (uint8_t)(val & 0x01);
           uint8_t result = (uint8_t)(val >> 1);
            WriteMemory(addr, result);
            this->F = (uint8_t)(flag_SZP[result] | newCarry);
            return result;
        }

        void Srl_ix_d_ind()
        {
            Srl_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Srl_ix_d_ind_Ld_a()
        {
            this->A = Srl_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Srl_ix_d_ind_Ld_b()
        {
            this->B = Srl_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Srl_ix_d_ind_Ld_c()
        {
            this->C = Srl_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Srl_ix_d_ind_Ld_d()
        {
            this->D = Srl_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Srl_ix_d_ind_Ld_e()
        {
            this->E = Srl_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Srl_ix_d_ind_Ld_h()
        {
            this->H = Srl_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Srl_ix_d_ind_Ld_l()
        {
            this->L = Srl_ind((uint16_t)(this->IX + this->instr.displacement));
        }

        void Srl_iy_d_ind()
        {
            Srl_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Srl_iy_d_ind_Ld_a()
        {
            this->A = Srl_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Srl_iy_d_ind_Ld_b()
        {
            this->B = Srl_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Srl_iy_d_ind_Ld_c()
        {
            this->C = Srl_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Srl_iy_d_ind_Ld_d()
        {
            this->D = Srl_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Srl_iy_d_ind_Ld_e()
        {
            this->E = Srl_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Srl_iy_d_ind_Ld_h()
        {
            this->H = Srl_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Srl_iy_d_ind_Ld_l()
        {
            this->L = Srl_ind((uint16_t)(this->IY + this->instr.displacement));
        }

        void Sub_a_r()
        {
           uint8_t d = this->reg[(int)this->instr._register];
           uint8_t result = (uint8_t)((this->A - d) & 0xff);
            this->F = (uint8_t)(Get_SZHVC_sub_flag(this->A, result, 0) | N_mask);
            this->A = result;
        }

        void Sub_a_n()
        {
           uint8_t result = (uint8_t)((this->A - this->instr.immediate) & 0xff);
            this->F = (uint8_t)(Get_SZHVC_sub_flag(this->A, result, 0) | N_mask);
            this->A = result;
        }

        void Sub_a_hx()
        {
           uint8_t result = (uint8_t)((this->A - this->HX) & 0xff);
            this->F = (uint8_t)(Get_SZHVC_sub_flag(this->A, result, 0) | N_mask);
            this->A = result;
        }

        void Sub_a_lx()
        {
           uint8_t result = (uint8_t)((this->A - this->LX) & 0xff);
            this->F = (uint8_t)(Get_SZHVC_sub_flag(this->A, result, 0) | N_mask);
            this->A = result;
        }

        void Sub_a_hy()
        {
           uint8_t result = (uint8_t)((this->A - this->HY) & 0xff);
            this->F = (uint8_t)(Get_SZHVC_sub_flag(this->A, result, 0) | N_mask);
            this->A = result;
        }

        void Sub_a_ly()
        {
           uint8_t result = (uint8_t)((this->A - this->LY) & 0xff);
            this->F = (uint8_t)(Get_SZHVC_sub_flag(this->A, result, 0) | N_mask);
            this->A = result;
        }

        void Sub_a_hl_ind()
        {
           uint8_t val = ReadMemory(this->HL);
           uint8_t result = (uint8_t)((this->A - val) & 0xff);
            this->F = (uint8_t)(Get_SZHVC_sub_flag(this->A, result, 0) | N_mask);
            this->A = result;
        }

        void Sub_a_ix_d_ind()
        {
            uint16_t addr = (uint16_t)(this->IX + this->instr.displacement);
           uint8_t val = ReadMemory(addr);
           uint8_t result = (uint8_t)((this->A - val) & 0xff);
            this->F = (uint8_t)(Get_SZHVC_sub_flag(this->A, result, 0) | N_mask);
            this->A = result;
        }

        void Sub_a_iy_d_ind()
        {
            uint16_t addr = (uint16_t)(this->IY + this->instr.displacement);
           uint8_t val = ReadMemory(addr);
           uint8_t result = (uint8_t)((this->A - val) & 0xff);
            this->F = (uint8_t)(Get_SZHVC_sub_flag(this->A, result, 0) | N_mask);
            this->A = result;
        }

        void Xor_a_r()
        {
            this->A = (uint8_t)(this->A ^ this->reg[(int)this->instr._register]);
            this->F = flag_SZP[this->A];
        }

        void Xor_a_n()
        {
            this->A = (uint8_t)(this->A ^ this->instr.immediate);
            this->F = flag_SZP[this->A];
        }

        void Xor_a_hx()
        {
            this->A = (uint8_t)(this->A ^ this->HX);
            this->F = flag_SZP[this->A];
        }

        void Xor_a_lx()
        {
            this->A = (uint8_t)(this->A ^ this->LX);
            this->F = flag_SZP[this->A];
        }

        void Xor_a_hy()
        {
            this->A = (uint8_t)(this->A ^ this->HY);
            this->F = flag_SZP[this->A];
        }

        void Xor_a_ly()
        {
            this->A = (uint8_t)(this->A ^ this->LY);
            this->F = flag_SZP[this->A];
        }

        void Xor_a_hl_ind()
        {
           uint8_t val = ReadMemory(this->HL);
            this->A = (uint8_t)(this->A ^ val);
            this->F = flag_SZP[this->A];
        }

        void Xor_a_ix_d_ind()
        {
            uint16_t addr = (uint16_t)(this->IX + this->instr.displacement);
           uint8_t val = ReadMemory(addr);
            this->A = (uint8_t)(this->A ^ val);
            this->F = flag_SZP[this->A];
        }

        void Xor_a_iy_d_ind()
        {
            uint16_t addr = (uint16_t)(this->IY + this->instr.displacement);
           uint8_t val = ReadMemory(addr);
            this->A = (uint8_t)(this->A ^ val);
            this->F = flag_SZP[this->A];
        }

	private:
		void InitFlagArrays();
		void ResetStatus();

        static OpcodeInfo fetchDecodeTable[7][256];

		// processor status
        uint16_t IX;
        uint16_t IY;
        uint16_t SP;
        uint16_t PC;

       uint8_t reg[8];
       uint8_t altReg[8];
       uint8_t I;
       uint8_t R;

        bool IFF1;
        bool IFF2;
        InterruptMode interruptMode;
        Emulator::LineState nmiState;
        bool isNmiPending;
        uint32_t irqVector;
        bool AfterEI;

		Emulator::IrqDeviceInputLine irqLine;
		Emulator::NmiDeviceInputLine nmiLine;
		Emulator::HaltDeviceInputLine haltLine;
		Emulator::ResetDeviceInputLine resetLine;

		//
		//Emulator::Cpu& cpu;
		std::string tag;
        double clockFrequency; // Hz
        bool halt;
        uint8_t* ram; // 64K
        Emulator::IOHandler& ioHandler;
        Emulator::IMemoryHandler* memoryHandler;
		bool aborted;
        int64_t cyclesLeft;
		int64_t cyclesInCurrentTimeslice;
		Instruction instr; // for the sake of speed, reuse the same Instruction
        ProgramCounterListener _programCounterListener;
	};

} // Z80