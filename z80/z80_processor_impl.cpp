#include "z80_processor_impl.h"

namespace Z80 {

    const uint8_t Z80ProcessorImpl::cc_op[] =
    {
        4,10, 7, 6, 4, 4, 7, 4, 4,11, 7, 6, 4, 4, 7, 4,
        8,10, 7, 6, 4, 4, 7, 4,12,11, 7, 6, 4, 4, 7, 4,
        7,10,16, 6, 4, 4, 7, 4, 7,11,16, 6, 4, 4, 7, 4,
        7,10,13, 6,11,11,10, 4, 7,11,13, 6, 4, 4, 7, 4,
        4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
        4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
        4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
        7, 7, 7, 7, 7, 7, 4, 7, 4, 4, 4, 4, 4, 4, 7, 4,
        4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
        4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
        4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
        4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
        5,10,10,10,10,11, 7,11, 5,10,10, 0,10,17, 7,11,
        5,10,10,11,10,11, 7,11, 5, 4,10,11,10, 0, 7,11,
        5,10,10,19,10,11, 7,11, 5, 4,10, 4,10, 0, 7,11,
        5,10,10, 4,10,11, 7,11, 5, 6,10, 4,10, 0, 7,11
    };

    const uint8_t Z80ProcessorImpl::cc_cb[] =
    {
        8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8,
        8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8,
        8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8,
        8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8,
        8, 8, 8, 8, 8, 8,12, 8, 8, 8, 8, 8, 8, 8,12, 8,
        8, 8, 8, 8, 8, 8,12, 8, 8, 8, 8, 8, 8, 8,12, 8,
        8, 8, 8, 8, 8, 8,12, 8, 8, 8, 8, 8, 8, 8,12, 8,
        8, 8, 8, 8, 8, 8,12, 8, 8, 8, 8, 8, 8, 8,12, 8,
        8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8,
        8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8,
        8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8,
        8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8,
        8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8,
        8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8,
        8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8,
        8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8
    };

    const uint8_t Z80ProcessorImpl::cc_ed[] =
    {
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        12,12,15,20, 8,14, 8, 9,12,12,15,20, 8,14, 8, 9,
        12,12,15,20, 8,14, 8, 9,12,12,15,20, 8,14, 8, 9,
        12,12,15,20, 8,14, 8,18,12,12,15,20, 8,14, 8,18,
        12,12,15,20, 8,14, 8, 8,12,12,15,20, 8,14, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        16,16,16,16, 8, 8, 8, 8,16,16,16,16, 8, 8, 8, 8,
        16,16,16,16, 8, 8, 8, 8,16,16,16,16, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8
    };

    const uint8_t Z80ProcessorImpl::cc_xy[] =
    {
        4, 4, 4, 4, 4, 4, 4, 4, 4,15, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4,15, 4, 4, 4, 4, 4, 4,
        4,14,20,10, 9, 9,11, 4, 4,15,20,10, 9, 9,11, 4,
        4, 4, 4, 4,23,23,19, 4, 4,15, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 9, 9,19, 4, 4, 4, 4, 4, 9, 9,19, 4,
        4, 4, 4, 4, 9, 9,19, 4, 4, 4, 4, 4, 9, 9,19, 4,
        9, 9, 9, 9, 9, 9,19, 9, 9, 9, 9, 9, 9, 9,19, 9,
        19,19,19,19,19,19, 4,19, 4, 4, 4, 4, 9, 9,19, 4,
        4, 4, 4, 4, 9, 9,19, 4, 4, 4, 4, 4, 9, 9,19, 4,
        4, 4, 4, 4, 9, 9,19, 4, 4, 4, 4, 4, 9, 9,19, 4,
        4, 4, 4, 4, 9, 9,19, 4, 4, 4, 4, 4, 9, 9,19, 4,
        4, 4, 4, 4, 9, 9,19, 4, 4, 4, 4, 4, 9, 9,19, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        4,14, 4,23, 4,15, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 4, 4,10, 4, 4, 4, 4, 4, 4
    };

    const uint8_t Z80ProcessorImpl::cc_xycb[] =
    {
        23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,
        23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,
        23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,
        23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,
        20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,
        20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,
        20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,
        20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,
        23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,
        23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,
        23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,
        23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,
        23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,
        23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,
        23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,
        23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23
    };

    // extra cycles if jr/jp/call taken and 'interrupt latency' on rst 0-7
    const uint8_t Z80ProcessorImpl::cc_ex[] =
    {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* DJNZ */
            5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0,	/* JR NZ/JR Z */
            5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0,	/* JR NC/JR C */
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0,	/* LDIR/CPIR/INIR/OTIR LDDR/CPDR/INDR/OTDR */
            6, 0, 0, 0, 7, 0, 0, 2, 6, 0, 0, 0, 7, 0, 0, 2,
            6, 0, 0, 0, 7, 0, 0, 2, 6, 0, 0, 0, 7, 0, 0, 2,
            6, 0, 0, 0, 7, 0, 0, 2, 6, 0, 0, 0, 7, 0, 0, 2,
            6, 0, 0, 0, 7, 0, 0, 2, 6, 0, 0, 0, 7, 0, 0, 2
    };

    const bool Z80ProcessorImpl::parity[] =
    {
        true,  false, false, true,  false, true,  true,  false, false, true,  true,  false, true,  false, false, true,
        false, true,  true,  false, true,  false, false, true,  true,  false, false, true,  false, true,  true,  false,
        false, true,  true,  false, true,  false, false, true,  true,  false, false, true,  false, true,  true,  false,
        true,  false, false, true,  false, true,  true,  false, false, true,  true,  false, true,  false, false, true,
        false, true,  true,  false, true,  false, false, true,  true,  false, false, true,  false, true,  true,  false,
        true,  false, false, true,  false, true,  true,  false, false, true,  true,  false, true,  false, false, true,
        true,  false, false, true,  false, true,  true,  false, false, true,  true,  false, true,  false, false, true,
        false, true,  true,  false, true,  false, false, true,  true,  false, false, true,  false, true,  true,  false,
        false, true,  true,  false, true,  false, false, true,  true,  false, false, true,  false, true,  true,  false,
        true,  false, false, true,  false, true,  true,  false, false, true,  true,  false, true,  false, false, true,
        true,  false, false, true,  false, true,  true,  false, false, true,  true,  false, true,  false, false, true,
        false, true,  true,  false, true,  false, false, true,  true,  false, false, true,  false, true,  true,  false,
        true,  false, false, true,  false, true,  true,  false, false, true,  true,  false, true,  false, false, true,
        false, true,  true,  false, true,  false, false, true,  true,  false, false, true,  false, true,  true,  false,
        false, true,  true,  false, true,  false, false, true,  true,  false, false, true,  false, true,  true,  false,
        true,  false, false, true,  false, true,  true,  false, false, true,  true,  false, true,  false, false, true
    };

    uint8_t Z80ProcessorImpl::flag_SZHVC_add[256][256][2];
    uint8_t Z80ProcessorImpl::flag_SZHVC_sub[256][256][2];

    uint8_t Z80ProcessorImpl::flag_SZ[256];
    uint8_t Z80ProcessorImpl::flag_SZP[256];
    uint8_t Z80ProcessorImpl::flag_SZ_BIT[256];
    uint8_t Z80ProcessorImpl::flag_SZHV_dec[256];
    uint8_t Z80ProcessorImpl::flag_SZHV_inc[256];

	const uint8_t Z80ProcessorImpl::rstMask[] =
    {
        0xfe, 0xfd, 0xfb, 0xf7, 0xef, 0xdf, 0xbf, 0x7f
    };

	// static
    Z80ProcessorImpl::OpcodeInfo Z80ProcessorImpl::fetchDecodeTable[7][256] =
    {
        // no prefix
        {
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Nop), // 0x00
            OpcodeInfo(DecodeAction::ImmediateExtended, &Z80ProcessorImpl::Ld_bc_nn), // 0x01
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_bc_ind_a), // 0x02
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Inc_bc), // 0x03
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Inc_r), // 0x04
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Dec_r), // 0x05
            OpcodeInfo(DecodeAction::Immediate, &Z80ProcessorImpl::Ld_b_n), // 0x06
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Rlc_a), // 0x07
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ex_af1_af2), // 0x08
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Add_hl_bc), // 0x09
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_a_bc_ind), // 0x0a
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Dec_bc), // 0x0b
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Inc_r), // 0x0c
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Dec_r), // 0x0d
            OpcodeInfo(DecodeAction::Immediate, &Z80ProcessorImpl::Ld_c_n), // 0x0e
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Rrc_a), // 0x0f

            OpcodeInfo(DecodeAction::Displacement, &Z80ProcessorImpl::Djnz_d), // 0x10
            OpcodeInfo(DecodeAction::ImmediateExtended, &Z80ProcessorImpl::Ld_de_nn), // 0x11
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_de_ind_a), // 0x12
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Inc_de), // 0x13
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Inc_r), // 0x14
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Dec_r), // 0x15
            OpcodeInfo(DecodeAction::Immediate, &Z80ProcessorImpl::Ld_d_n), // 0x16
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Rl_a), // 0x17
            OpcodeInfo(DecodeAction::Displacement, &Z80ProcessorImpl::Jr_d), // 0x18
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Add_hl_de), // 0x19
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_a_de_ind), // 0x1a
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Dec_de), // 0x1b
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Inc_r), // 0x1c
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Dec_r), // 0x1d
            OpcodeInfo(DecodeAction::Immediate, &Z80ProcessorImpl::Ld_e_n), // 0x1e
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Rr_a), // 0x1f

            OpcodeInfo(DecodeAction::Displacement, &Z80ProcessorImpl::Jr_nz_d), // 0x20
            OpcodeInfo(DecodeAction::ImmediateExtended, &Z80ProcessorImpl::Ld_hl_nn), // 0x21
            OpcodeInfo(DecodeAction::ImmediateExtended, &Z80ProcessorImpl::Ld_nn_ind_hl), // 0x22
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Inc_hl), // 0x23
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Inc_r), // 0x24
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Dec_r), // 0x25
            OpcodeInfo(DecodeAction::Immediate, &Z80ProcessorImpl::Ld_h_n), // 0x26
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Da_a), // 0x27
            OpcodeInfo(DecodeAction::Displacement, &Z80ProcessorImpl::Jr_z_d), // 0x28
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Add_hl_hl), // 0x29
            OpcodeInfo(DecodeAction::ImmediateExtended, &Z80ProcessorImpl::Ld_hl_nn_ind), // 0x2a
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Dec_hl), // 0x2b
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Inc_r), // 0x2c
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Dec_r), // 0x2d
            OpcodeInfo(DecodeAction::Immediate, &Z80ProcessorImpl::Ld_l_n), // 0x2e
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Cpl), // 0x2f

            OpcodeInfo(DecodeAction::Displacement, &Z80ProcessorImpl::Jr_nc_d), // 0x30
            OpcodeInfo(DecodeAction::ImmediateExtended, &Z80ProcessorImpl::Ld_sp_nn), // 0x31
            OpcodeInfo(DecodeAction::ImmediateExtended, &Z80ProcessorImpl::Ld_nn_ind_a), // 0x32
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Inc_sp), // 0x33
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Inc_hl_ind), // 0x34
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Dec_hl_ind), // 0x35
            OpcodeInfo(DecodeAction::Immediate, &Z80ProcessorImpl::Ld_hl_ind_n), // 0x36
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Scf), // 0x37
            OpcodeInfo(DecodeAction::Displacement, &Z80ProcessorImpl::Jr_c_d), // 0x38
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Add_hl_sp), // 0x39
            OpcodeInfo(DecodeAction::ImmediateExtended, &Z80ProcessorImpl::Ld_a_nn_ind), // 0x3a
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Dec_sp), // 0x3b
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Inc_r), // 0x3c
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Dec_r), // 0x3d
            OpcodeInfo(DecodeAction::Immediate, &Z80ProcessorImpl::Ld_a_n), // 0x3e
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ccf), // 0x3f

            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_b_r), // 0x40
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_b_r), // 0x41
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_b_r), // 0x42
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_b_r), // 0x43
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_b_r), // 0x44
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_b_r), // 0x45
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_b_hl_ind), // 0x46
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_b_r), // 0x47
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_c_r), // 0x48
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_c_r), // 0x49
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_c_r), // 0x4a
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_c_r), // 0x4b
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_c_r), // 0x4c
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_c_r), // 0x4d
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_c_hl_ind), // 0x4e
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_c_r), // 0x4f

            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_d_r), // 0x50
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_d_r), // 0x51
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_d_r), // 0x52
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_d_r), // 0x53
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_d_r), // 0x54
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_d_r), // 0x55
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_d_hl_ind), // 0x56
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_d_r), // 0x57
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_e_r), // 0x58
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_e_r), // 0x59
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_e_r), // 0x5a
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_e_r), // 0x5b
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_e_r), // 0x5c
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_e_r), // 0x5d
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_e_hl_ind), // 0x5e
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_e_r), // 0x5f

            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_h_r), // 0x60
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_h_r), // 0x61
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_h_r), // 0x62
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_h_r), // 0x63
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_h_r), // 0x64
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_h_r), // 0x65
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_h_hl_ind), // 0x66
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_h_r), // 0x67
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_l_r), // 0x68
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_l_r), // 0x69
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_l_r), // 0x6a
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_l_r), // 0x6b
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_l_r), // 0x6c
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_l_r), // 0x6d
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_l_hl_ind), // 0x6e
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_l_r), // 0x6f

            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_hl_ind_r), // 0x70
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_hl_ind_r), // 0x71
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_hl_ind_r), // 0x72
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_hl_ind_r), // 0x73
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_hl_ind_r), // 0x74
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_hl_ind_r), // 0x75
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Halt), // 0x76
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_hl_ind_r), // 0x77
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_a_r), // 0x78
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_a_r), // 0x79
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_a_r), // 0x7a
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_a_r), // 0x7b
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_a_r), // 0x7c
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_a_r), // 0x7d
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_a_hl_ind), // 0x7e
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Ld_a_r), // 0x7f

            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Add_a_r), // 0x80
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Add_a_r), // 0x81
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Add_a_r), // 0x82
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Add_a_r), // 0x83
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Add_a_r), // 0x84
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Add_a_r), // 0x85
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Add_a_hl_ind), // 0x86
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Add_a_r), // 0x87
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Adc_a_r), // 0x88
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Adc_a_r), // 0x89
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Adc_a_r), // 0x8a
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Adc_a_r), // 0x8b
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Adc_a_r), // 0x8c
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Adc_a_r), // 0x8d
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Adc_a_hl_ind), // 0x8e
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Adc_a_r), // 0x8f

            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Sub_a_r), // 0x90
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Sub_a_r), // 0x91
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Sub_a_r), // 0x92
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Sub_a_r), // 0x93
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Sub_a_r), // 0x94
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Sub_a_r), // 0x95
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Sub_a_hl_ind), // 0x96
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Sub_a_r), // 0x97
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Sbc_a_r), // 0x98
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Sbc_a_r), // 0x99
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Sbc_a_r), // 0x9a
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Sbc_a_r), // 0x9b
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Sbc_a_r), // 0x9c
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Sbc_a_r), // 0x9d
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Sbc_a_hl_ind), // 0x9e
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Sbc_a_r), // 0x9f

            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::And_a_r), // 0xa0
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::And_a_r), // 0xa1
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::And_a_r), // 0xa2
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::And_a_r), // 0xa3
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::And_a_r), // 0xa4
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::And_a_r), // 0xa5
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::And_a_hl_ind), // 0xa6
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::And_a_r), // 0xa7
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Xor_a_r), // 0xa8
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Xor_a_r), // 0xa9
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Xor_a_r), // 0xaa
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Xor_a_r), // 0xab
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Xor_a_r), // 0xac
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Xor_a_r), // 0xad
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Xor_a_hl_ind), // 0xae
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Xor_a_r), // 0xaf

            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Or_a_r), // 0xb0
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Or_a_r), // 0xb1
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Or_a_r), // 0xb2
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Or_a_r), // 0xb3
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Or_a_r), // 0xb4
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Or_a_r), // 0xb5
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Or_a_hl_ind), // 0xb6
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Or_a_r), // 0xb7
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Cp_a_r), // 0xb8
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Cp_a_r), // 0xb9
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Cp_a_r), // 0xba
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Cp_a_r), // 0xbb
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Cp_a_r), // 0xbc
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Cp_a_r), // 0xbd
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Cp_a_hl_ind), // 0xbe
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Cp_a_r), // 0xbf

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ret_nz), // 0xc0
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Pop_bc), // 0xc1
            OpcodeInfo(DecodeAction::ImmediateExtended, &Z80ProcessorImpl::Jp_nz_nn), // 0xc2
            OpcodeInfo(DecodeAction::ImmediateExtended, &Z80ProcessorImpl::Jp_nn), // 0xc3
            OpcodeInfo(DecodeAction::ImmediateExtended, &Z80ProcessorImpl::Call_nz_nn), // 0xc4
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Push_bc), // 0xc5
            OpcodeInfo(DecodeAction::Immediate, &Z80ProcessorImpl::Add_a_n), // 0xc6
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Rst), // 0xc7
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ret_z), // 0xc8
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ret), // 0xc9
            OpcodeInfo(DecodeAction::ImmediateExtended, &Z80ProcessorImpl::Jp_z_nn), // 0xca
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xcb
            OpcodeInfo(DecodeAction::ImmediateExtended, &Z80ProcessorImpl::Call_z_nn), // 0xcc
            OpcodeInfo(DecodeAction::ImmediateExtended, &Z80ProcessorImpl::Call_nn), // 0xcd
            OpcodeInfo(DecodeAction::Immediate, &Z80ProcessorImpl::Adc_a_n), // 0xce
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Rst), // 0xcf

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ret_nc), // 0xd0
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Pop_de), // 0xd1
            OpcodeInfo(DecodeAction::ImmediateExtended, &Z80ProcessorImpl::Jp_nc_nn), // 0xd2
            OpcodeInfo(DecodeAction::Immediate, &Z80ProcessorImpl::Out_n_a), // 0xd3
            OpcodeInfo(DecodeAction::ImmediateExtended, &Z80ProcessorImpl::Call_nc_nn), // 0xd4
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Push_de), // 0xd5
            OpcodeInfo(DecodeAction::Immediate, &Z80ProcessorImpl::Sub_a_n), // 0xd6
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Rst), // 0xd7
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ret_c), // 0xd8
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Exx), // 0xd9
            OpcodeInfo(DecodeAction::ImmediateExtended, &Z80ProcessorImpl::Jp_c_nn), // 0xda
            OpcodeInfo(DecodeAction::Immediate, &Z80ProcessorImpl::In_a_n), // 0xdb
            OpcodeInfo(DecodeAction::ImmediateExtended, &Z80ProcessorImpl::Call_c_nn), // 0xdc
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xdd
            OpcodeInfo(DecodeAction::Immediate, &Z80ProcessorImpl::Sbc_a_n), // 0xde
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Rst), // 0xdf

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ret_po), // 0xe0
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Pop_hl), // 0xe1
            OpcodeInfo(DecodeAction::ImmediateExtended, &Z80ProcessorImpl::Jp_po_nn), // 0xe2
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ex_sp_ind_hl), // 0xe3
            OpcodeInfo(DecodeAction::ImmediateExtended, &Z80ProcessorImpl::Call_po_nn), // 0xe4
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Push_hl), // 0xe5
            OpcodeInfo(DecodeAction::Immediate, &Z80ProcessorImpl::And_a_n), // 0xe6
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Rst), // 0xe7
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ret_pe), // 0xe8
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Jp_hl_ind), // 0xe9
            OpcodeInfo(DecodeAction::ImmediateExtended, &Z80ProcessorImpl::Jp_pe_nn), // 0xea
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ex_de_hl), // 0xeb
            OpcodeInfo(DecodeAction::ImmediateExtended, &Z80ProcessorImpl::Call_pe_nn), // 0xec
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xed
            OpcodeInfo(DecodeAction::Immediate, &Z80ProcessorImpl::Xor_a_n), // 0xee
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Rst), // 0xef

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ret_p), // 0xf0
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Pop_af), // 0xf1
            OpcodeInfo(DecodeAction::ImmediateExtended, &Z80ProcessorImpl::Jp_p_nn), // 0xf2
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Di), // 0xf3
            OpcodeInfo(DecodeAction::ImmediateExtended, &Z80ProcessorImpl::Call_p_nn), // 0xf4
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Push_af), // 0xf5
            OpcodeInfo(DecodeAction::Immediate, &Z80ProcessorImpl::Or_a_n), // 0xf6
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Rst), // 0xf7
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ret_m), // 0xf8
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_sp_hl), // 0xf9
            OpcodeInfo(DecodeAction::ImmediateExtended, &Z80ProcessorImpl::Jp_m_nn), // 0xfa
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ei), // 0xfb
            OpcodeInfo(DecodeAction::ImmediateExtended, &Z80ProcessorImpl::Call_m_nn), // 0xfc
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xfd
            OpcodeInfo(DecodeAction::Immediate, &Z80ProcessorImpl::Cp_a_n), // 0xfe
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Rst), // 0xff
        },

        // CB prefix
        {
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Rlc_r), // 0x00
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Rlc_r), // 0x01
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Rlc_r), // 0x02
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Rlc_r), // 0x03
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Rlc_r), // 0x04
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Rlc_r), // 0x05
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Rlc_hl_ind), // 0x06
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Rlc_r), // 0x07
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Rrc_r), // 0x08
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Rrc_r), // 0x09
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Rrc_r), // 0x0a
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Rrc_r), // 0x0b
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Rrc_r), // 0x0c
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Rrc_r), // 0x0d
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Rrc_hl_ind), // 0x0e
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Rrc_r), // 0x0f

            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Rl_r), // 0x10
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Rl_r), // 0x11
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Rl_r), // 0x12
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Rl_r), // 0x13
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Rl_r), // 0x14
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Rl_r), // 0x15
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Rl_hl_ind), // 0x16
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Rl_r), // 0x17
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Rr_r), // 0x18
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Rr_r), // 0x19
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Rr_r), // 0x1a
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Rr_r), // 0x1b
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Rr_r), // 0x1c
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Rr_r), // 0x1d
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Rr_hl_ind), // 0x1e
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Rr_r), // 0x1f

            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Sla_r), // 0x20
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Sla_r), // 0x21
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Sla_r), // 0x22
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Sla_r), // 0x23
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Sla_r), // 0x24
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Sla_r), // 0x25
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Sla_hl_ind), // 0x26
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Sla_r), // 0x27
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Sra_r), // 0x28
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Sra_r), // 0x29
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Sra_r), // 0x2a
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Sra_r), // 0x2b
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Sra_r), // 0x2c
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Sra_r), // 0x2d
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Sra_hl_ind), // 0x2e
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Sra_r), // 0x2f

            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Sll_r), // 0x30
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Sll_r), // 0x31
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Sll_r), // 0x32
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Sll_r), // 0x33
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Sll_r), // 0x34
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Sll_r), // 0x35
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Sll_hl_ind), // 0x36
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Sll_r), // 0x37
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Srl_r), // 0x38
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Srl_r), // 0x39
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Srl_r), // 0x3a
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Srl_r), // 0x3b
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Srl_r), // 0x3c
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Srl_r), // 0x3d
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Srl_hl_ind), // 0x3e
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Srl_r), // 0x3f

            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x40
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x41
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x42
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x43
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x44
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x45
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Bit_bit_hl_ind), // 0x46
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x47
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x48
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x49
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x4a
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x4b
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x4c
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x4d
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Bit_bit_hl_ind), // 0x4e
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x4f

            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x50
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x51
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x52
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x53
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x54
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x55
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Bit_bit_hl_ind), // 0x56
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x57
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x58
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x59
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x5a
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x5b
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x5c
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x5d
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Bit_bit_hl_ind), // 0x5e
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x5f

            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x60
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x61
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x62
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x63
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x64
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x65
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Bit_bit_hl_ind), // 0x66
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x67
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x68
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x69
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x6a
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x6b
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x6c
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x6d
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Bit_bit_hl_ind), // 0x6e
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x6f

            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x70
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x71
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x72
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x73
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x74
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x75
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Bit_bit_hl_ind), // 0x76
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x77
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x78
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x79
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x7a
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x7b
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x7c
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x7d
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Bit_bit_hl_ind), // 0x7e
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Bit_bit_r), // 0x7f

            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0x80
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0x81
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0x82
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0x83
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0x84
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0x85
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Res_bit_hl_ind), // 0x86
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0x87
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0x88
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0x89
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0x8a
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0x8b
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0x8c
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0x8d
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Res_bit_hl_ind), // 0x8e
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0x8f

            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0x90
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0x91
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0x92
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0x93
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0x94
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0x95
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Res_bit_hl_ind), // 0x96
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0x97
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0x98
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0x99
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0x9a
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0x9b
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0x9c
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0x9d
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Res_bit_hl_ind), // 0x9e
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0x9f

            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0xa0
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0xa1
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0xa2
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0xa3
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0xa4
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0xa5
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Res_bit_hl_ind), // 0xa6
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0xa7
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0xa8
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0xa9
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0xaa
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0xab
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0xac
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0xad
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Res_bit_hl_ind), // 0xae
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0xaf

            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0xb0
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0xb1
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0xb2
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0xb3
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0xb4
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0xb5
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Res_bit_hl_ind), // 0xb6
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0xb7
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0xb8
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0xb9
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0xba
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0xbb
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0xbc
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0xbd
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Res_bit_hl_ind), // 0xbe
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Res_bit_r), // 0xbf

            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xc0
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xc1
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xc2
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xc3
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xc4
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xc5
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Set_bit_hl_ind), // 0xc6
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xc7
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xc8
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xc9
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xca
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xcb
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xcc
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xcd
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Set_bit_hl_ind), // 0xce
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xcf

            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xd0
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xd1
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xd2
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xd3
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xd4
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xd5
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Set_bit_hl_ind), // 0xd6
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xd7
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xd8
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xd9
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xda
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xdb
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xdc
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xdd
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Set_bit_hl_ind), // 0xde
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xdf

            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xe0
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xe1
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xe2
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xe3
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xe4
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xe5
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Set_bit_hl_ind), // 0xe6
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xe7
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xe8
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xe9
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xea
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xeb
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xec
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xed
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Set_bit_hl_ind), // 0xee
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xef

            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xf0
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xf1
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xf2
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xf3
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xf4
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xf5
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Set_bit_hl_ind), // 0xf6
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xf7
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xf8
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xf9
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xfa
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xfb
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xfc
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xfd
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Set_bit_hl_ind), // 0xfe
            OpcodeInfo(DecodeAction::Register, &Z80ProcessorImpl::Set_bit_r), // 0xff
        },

        // DD prefix
        {
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x00
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x01
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x02
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x03
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x04
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x05
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x06
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x07
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x08
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Add_ix_bc), // 0x09
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x0a
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x0b
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x0c
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x0d
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x0e
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x0f

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x10
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x11
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x12
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x13
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x14
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x15
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x16
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x17
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x18
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Add_ix_de), // 0x19
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x1a
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x1b
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x1c
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x1d
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x1e
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x1f

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x20
            OpcodeInfo(DecodeAction::ImmediateExtended, &Z80ProcessorImpl::Ld_ix_nn), // 0x21
            OpcodeInfo(DecodeAction::ImmediateExtended, &Z80ProcessorImpl::Ld_nn_ind_ix), // 0x22
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Inc_ix), // 0x23
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Inc_hx), // 0x24
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Dec_hx), // 0x25
            OpcodeInfo(DecodeAction::Immediate, &Z80ProcessorImpl::Ld_hx_n), // 0x26
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x27
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x28
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Add_ix_ix), // 0x29
            OpcodeInfo(DecodeAction::ImmediateExtended, &Z80ProcessorImpl::Ld_ix_nn_ind), // 0x2a
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Dec_ix), // 0x2b
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Inc_lx), // 0x2c
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Dec_lx), // 0x2d
            OpcodeInfo(DecodeAction::Immediate, &Z80ProcessorImpl::Ld_lx_n), // 0x2e
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x2f

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x30
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x31
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x32
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x33
            OpcodeInfo(DecodeAction::Displacement, &Z80ProcessorImpl::Inc_ix_d_ind), // 0x34
            OpcodeInfo(DecodeAction::Displacement, &Z80ProcessorImpl::Dec_ix_d_ind), // 0x35
            OpcodeInfo(DecodeAction::DisplacementImmediate, &Z80ProcessorImpl::Ld_ix_d_ind_n), // 0x36
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x37
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x38
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Add_ix_sp), // 0x39
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x3a
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x3b
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x3c
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x3d
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x3e
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x3f

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x40
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x41
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x42
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x43
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_b_hx), // 0x44
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_b_lx), // 0x45
            OpcodeInfo(DecodeAction::Displacement, &Z80ProcessorImpl::Ld_b_ix_d_ind), // 0x46
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x47
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x48
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x49
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x4a
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x4b
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_c_hx), // 0x4c
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_c_lx), // 0x4d
            OpcodeInfo(DecodeAction::Displacement, &Z80ProcessorImpl::Ld_c_ix_d_ind), // 0x4e
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x4f

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x50
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x51
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x52
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x53
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_d_hx), // 0x54
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_d_lx), // 0x55
            OpcodeInfo(DecodeAction::Displacement, &Z80ProcessorImpl::Ld_d_ix_d_ind), // 0x56
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x57
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x58
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x59
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x5a
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x5b
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_e_hx), // 0x5c
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_e_lx), // 0x5d
            OpcodeInfo(DecodeAction::Displacement, &Z80ProcessorImpl::Ld_e_ix_d_ind), // 0x5e
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x5f

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_hx_b), // 0x60
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_hx_c), // 0x61
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_hx_d), // 0x62
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_hx_e), // 0x63
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_hx_hx), // 0x64
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_hx_lx), // 0x65
            OpcodeInfo(DecodeAction::Displacement, &Z80ProcessorImpl::Ld_h_ix_d_ind), // 0x66
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_hx_a), // 0x67
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_lx_b), // 0x68
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_lx_c), // 0x69
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_lx_d), // 0x6a
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_lx_e), // 0x6b
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_lx_hx), // 0x6c
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_lx_lx), // 0x6d
            OpcodeInfo(DecodeAction::Displacement, &Z80ProcessorImpl::Ld_l_ix_d_ind), // 0x6e
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_lx_a), // 0x6f

            OpcodeInfo(DecodeAction::DisplacementRegistry, &Z80ProcessorImpl::Ld_ix_d_ind_r), // 0x70
            OpcodeInfo(DecodeAction::DisplacementRegistry, &Z80ProcessorImpl::Ld_ix_d_ind_r), // 0x71
            OpcodeInfo(DecodeAction::DisplacementRegistry, &Z80ProcessorImpl::Ld_ix_d_ind_r), // 0x72
            OpcodeInfo(DecodeAction::DisplacementRegistry, &Z80ProcessorImpl::Ld_ix_d_ind_r), // 0x73
            OpcodeInfo(DecodeAction::DisplacementRegistry, &Z80ProcessorImpl::Ld_ix_d_ind_r), // 0x74
            OpcodeInfo(DecodeAction::DisplacementRegistry, &Z80ProcessorImpl::Ld_ix_d_ind_r), // 0x75
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x76
            OpcodeInfo(DecodeAction::DisplacementRegistry, &Z80ProcessorImpl::Ld_ix_d_ind_r), // 0x77
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x78
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x79
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x7a
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x7b
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_a_hx), // 0x7c
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_a_lx), // 0x7d
            OpcodeInfo(DecodeAction::Displacement, &Z80ProcessorImpl::Ld_a_ix_d_ind), // 0x7e
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x7f

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x80
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x81
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x82
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x83
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Add_a_hx), // 0x84
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Add_a_lx), // 0x85
            OpcodeInfo(DecodeAction::Displacement, &Z80ProcessorImpl::Add_a_ix_d_ind), // 0x86
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x87
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x88
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x89
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x8a
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x8b
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Adc_a_hx), // 0x8c
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Adc_a_lx), // 0x8d
            OpcodeInfo(DecodeAction::Displacement, &Z80ProcessorImpl::Adc_a_ix_d_ind), // 0x8e
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x8f

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x90
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x91
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x92
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x93
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Sub_a_hx), // 0x94
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Sub_a_lx), // 0x95
            OpcodeInfo(DecodeAction::Displacement, &Z80ProcessorImpl::Sub_a_ix_d_ind), // 0x96
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x97
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x98
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x99
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x9a
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x9b
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Sbc_a_hx), // 0x9c
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Sbc_a_lx), // 0x9d
            OpcodeInfo(DecodeAction::Displacement, &Z80ProcessorImpl::Sbc_a_ix_d_ind), // 0x9e
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x9f

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xa0
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xa1
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xa2
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xa3
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::And_a_hx), // 0xa4
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::And_a_lx), // 0xa5
            OpcodeInfo(DecodeAction::Displacement, &Z80ProcessorImpl::And_a_ix_d_ind), // 0xa6
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xa7
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xa8
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xa9
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xaa
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xab
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Xor_a_hx), // 0xac
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Xor_a_lx), // 0xad
            OpcodeInfo(DecodeAction::Displacement, &Z80ProcessorImpl::Xor_a_ix_d_ind), // 0xae
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xaf

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xb0
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xb1
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xb2
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xb3
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Or_a_hx), // 0xb4
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Or_a_lx), // 0xb5
            OpcodeInfo(DecodeAction::Displacement, &Z80ProcessorImpl::Or_a_ix_d_ind), // 0xb6
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xb7
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xb8
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xb9
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xba
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xbb
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Cp_a_hx), // 0xbc
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Cp_a_lx), // 0xbd
            OpcodeInfo(DecodeAction::Displacement, &Z80ProcessorImpl::Cp_a_ix_d_ind), // 0xbe
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xbf

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xc0
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xc1
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xc2
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xc3
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xc4
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xc5
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xc6
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xc7
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xc8
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xc9
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xca
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xcb //
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xcc
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xcd
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xce
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xcf

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xd0
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xd1
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xd2
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xd3
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xd4
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xd5
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xd6
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xd7
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xd8
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xd9
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xda
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xdb
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xdc
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xdd
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xde
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xdf

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xe0
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Pop_ix), // 0xe1
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xe2
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ex_sp_ind_ix), // 0xe3
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xe4
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Push_ix), // 0xe5
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xe6
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xe7
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xe8
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Jp_ix_ind), // 0xe9
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xea
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xeb
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xec
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xed
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xee
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xef

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xf0
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xf1
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xf2
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xf3
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xf4
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xf5
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xf6
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xf7
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xf8
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_sp_ix), // 0xf9
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xfa
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xfb
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xfc
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xfd
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xfe
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xff
        },

        // ED prefix
        {
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x00
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x01
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x02
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x03
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x04
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x05
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x06
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x07
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x08
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x09
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x0a
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x0b
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x0c
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x0d
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x0e
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x0f

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x10
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x11
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x12
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x13
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x14
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x15
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x16
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x17
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x18
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x19
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x1a
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x1b
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x1c
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x1d
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x1e
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x1f

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x20
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x21
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x22
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x23
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x24
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x25
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x26
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x27
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x28
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x29
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x2a
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x2b
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x2c
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x2d
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x2e
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x2f

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x30
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x31
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x32
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x33
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x34
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x35
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x36
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x37
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x38
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x39
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x3a
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x3b
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x3c
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x3d
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x3e
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x3f

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::In_r_c), // 0x40
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Out_c_r), // 0x41
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Sbc_hl_bc), // 0x42
            OpcodeInfo(DecodeAction::ImmediateExtended, &Z80ProcessorImpl::Ld_nn_ind_bc), // 0x43
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Neg), // 0x44
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Retn), // 0x45
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Im_0), // 0x46
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_i_a), // 0x47
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::In_r_c), // 0x48
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Out_c_r), // 0x49
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Adc_hl_bc), // 0x4a
            OpcodeInfo(DecodeAction::ImmediateExtended, &Z80ProcessorImpl::Ld_bc_nn_ind), // 0x4b
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Neg), // 0x4c
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Reti), // 0x4d
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Im_0), // 0x4e
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_mr_a), // 0x4f

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::In_r_c), // 0x50
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Out_c_r), // 0x51
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Sbc_hl_de), // 0x52
            OpcodeInfo(DecodeAction::ImmediateExtended, &Z80ProcessorImpl::Ld_nn_ind_de), // 0x53
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Neg), // 0x54
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Retn), // 0x55
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Im_1), // 0x56
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_a_i), // 0x57
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::In_r_c), // 0x58
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Out_c_r), // 0x59
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Adc_hl_de), // 0x5a
            OpcodeInfo(DecodeAction::ImmediateExtended, &Z80ProcessorImpl::Ld_de_nn_ind), // 0x5b
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Neg), // 0x5c
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Reti), // 0x5d
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Im_2), // 0x5e
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_a_mr), // 0x5f

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::In_r_c), // 0x60
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Out_c_r), // 0x61
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Sbc_hl_hl), // 0x62
            OpcodeInfo(DecodeAction::ImmediateExtended, &Z80ProcessorImpl::Ld_nn_ind_hl), // 0x63
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Neg), // 0x64
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Retn), // 0x65
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Im_0), // 0x66
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Rrd_a_hl), // 0x67
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::In_r_c), // 0x68
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Out_c_r), // 0x69
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Adc_hl_hl), // 0x6a
            OpcodeInfo(DecodeAction::ImmediateExtended, &Z80ProcessorImpl::Ld_hl_nn_ind), // 0x6b
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Neg), // 0x6c
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Reti), // 0x6d
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Im_0), // 0x6e
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Rld_a_hl), // 0x6f

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::In_0_c), // 0x70
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Out_c_0), // 0x71
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Sbc_hl_sp), // 0x72
            OpcodeInfo(DecodeAction::ImmediateExtended, &Z80ProcessorImpl::Ld_nn_ind_sp), // 0x73
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Neg), // 0x74
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Retn), // 0x75
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Im_1), // 0x76
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x77
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::In_r_c), // 0x78
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Out_c_r), // 0x79
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Adc_hl_sp), // 0x7a
            OpcodeInfo(DecodeAction::ImmediateExtended, &Z80ProcessorImpl::Ld_sp_nn_ind), // 0x7b
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Neg), // 0x7c
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Reti), // 0x7d
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Im_2), // 0x7e
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x7f

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x80
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x81
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x82
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x83
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x84
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x85
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x86
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x87
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x88
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x89
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x8a
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x8b
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x8c
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x8d
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x8e
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x8f

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x90
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x91
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x92
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x93
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x94
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x95
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x96
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x97
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x98
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x99
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x9a
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x9b
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x9c
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x9d
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x9e
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x9f

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ldi), // 0xa0
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Cpi), // 0xa1
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ini), // 0xa2
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Outi), // 0xa3
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xa4
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xa5
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xa6
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xa7
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ldd), // 0xa8
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Cpd), // 0xa9
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ind), // 0xaa
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Outd), // 0xab
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xac
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xad
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xae
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xaf

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ldir), // 0xb0
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Cpir), // 0xb1
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Inir), // 0xb2
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Otir), // 0xb3
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xb4
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xb5
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xb6
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xb7
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Lddr), // 0xb8
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Cpdr), // 0xb9
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Indr), // 0xba
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Otdr), // 0xbb
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xbc
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xbd
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xbe
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xbf

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xc0
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xc1
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xc2
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xc3
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xc4
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xc5
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xc6
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xc7
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xc8
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xc9
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xca
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xcb
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xcc
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xcd
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xce
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xcf

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xd0
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xd1
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xd2
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xd3
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xd4
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xd5
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xd6
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xd7
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xd8
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xd9
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xda
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xdb
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xdc
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xdd
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xde
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xdf

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xe0
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xe1
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xe2
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xe3
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xe4
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xe5
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xe6
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xe7
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xe8
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xe9
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xea
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xeb
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xec
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xed
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xee
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xef

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xf0
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xf1
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xf2
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xf3
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xf4
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xf5
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xf6
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xf7
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xf8
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xf9
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xfa
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xfb
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xfc
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xfd
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xfe
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xff
        },

        // FD prefix
        {
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x00
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x01
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x02
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x03
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x04
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x05
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x06
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x07
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x08
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Add_iy_bc), // 0x09
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x0a
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x0b
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x0c
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x0d
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x0e
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x0f

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x10
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x11
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x12
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x13
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x14
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x15
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x16
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x17
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x18
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Add_iy_de), // 0x19
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x1a
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x1b
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x1c
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x1d
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x1e
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x1f

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x20
            OpcodeInfo(DecodeAction::ImmediateExtended, &Z80ProcessorImpl::Ld_iy_nn), // 0x21
            OpcodeInfo(DecodeAction::ImmediateExtended, &Z80ProcessorImpl::Ld_nn_ind_iy), // 0x22
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Inc_iy), // 0x23
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Inc_hy), // 0x24
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Dec_hy), // 0x25
            OpcodeInfo(DecodeAction::Immediate, &Z80ProcessorImpl::Ld_hy_n), // 0x26
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x27
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x28
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Add_iy_iy), // 0x29
            OpcodeInfo(DecodeAction::ImmediateExtended, &Z80ProcessorImpl::Ld_iy_nn_ind), // 0x2a
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Dec_iy), // 0x2b
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Inc_ly), // 0x2c
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Dec_ly), // 0x2d
            OpcodeInfo(DecodeAction::Immediate, &Z80ProcessorImpl::Ld_ly_n), // 0x2e
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x2f

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x30
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x31
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x32
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x33
            OpcodeInfo(DecodeAction::Displacement, &Z80ProcessorImpl::Inc_iy_d_ind), // 0x34
            OpcodeInfo(DecodeAction::Displacement, &Z80ProcessorImpl::Dec_iy_d_ind), // 0x35
            OpcodeInfo(DecodeAction::DisplacementImmediate, &Z80ProcessorImpl::Ld_iy_d_ind_n), // 0x36
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x37
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x38
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Add_iy_sp), // 0x39
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x3a
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x3b
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x3c
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x3d
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x3e
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x3f

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x40
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x41
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x42
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x43
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_b_hy), // 0x44
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_b_ly), // 0x45
            OpcodeInfo(DecodeAction::Displacement, &Z80ProcessorImpl::Ld_b_iy_d_ind), // 0x46
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x47
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x48
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x49
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x4a
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x4b
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_c_hy), // 0x4c
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_c_ly), // 0x4d
            OpcodeInfo(DecodeAction::Displacement, &Z80ProcessorImpl::Ld_c_iy_d_ind), // 0x4e
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x4f

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x50
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x51
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x52
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x53
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_d_hy), // 0x54
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_d_ly), // 0x55
            OpcodeInfo(DecodeAction::Displacement, &Z80ProcessorImpl::Ld_d_iy_d_ind), // 0x56
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x57
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x58
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x59
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x5a
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x5b
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_e_hy), // 0x5c
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_e_ly), // 0x5d
            OpcodeInfo(DecodeAction::Displacement, &Z80ProcessorImpl::Ld_e_iy_d_ind), // 0x5e
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x5f

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_hy_b), // 0x60
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_hy_c), // 0x61
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_hy_d), // 0x62
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_hy_e), // 0x63
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_hy_hy), // 0x64
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_hy_ly), // 0x65
            OpcodeInfo(DecodeAction::Displacement, &Z80ProcessorImpl::Ld_h_iy_d_ind), // 0x66
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_hy_a), // 0x67
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_ly_b), // 0x68
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_ly_c), // 0x69
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_ly_d), // 0x6a
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_ly_e), // 0x6b
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_ly_hy), // 0x6c
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_ly_ly), // 0x6d
            OpcodeInfo(DecodeAction::Displacement, &Z80ProcessorImpl::Ld_l_iy_d_ind), // 0x6e
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_ly_a), // 0x6f

            OpcodeInfo(DecodeAction::DisplacementRegistry, &Z80ProcessorImpl::Ld_iy_d_ind_r), // 0x70
            OpcodeInfo(DecodeAction::DisplacementRegistry, &Z80ProcessorImpl::Ld_iy_d_ind_r), // 0x71
            OpcodeInfo(DecodeAction::DisplacementRegistry, &Z80ProcessorImpl::Ld_iy_d_ind_r), // 0x72
            OpcodeInfo(DecodeAction::DisplacementRegistry, &Z80ProcessorImpl::Ld_iy_d_ind_r), // 0x73
            OpcodeInfo(DecodeAction::DisplacementRegistry, &Z80ProcessorImpl::Ld_iy_d_ind_r), // 0x74
            OpcodeInfo(DecodeAction::DisplacementRegistry, &Z80ProcessorImpl::Ld_iy_d_ind_r), // 0x75
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x76
            OpcodeInfo(DecodeAction::DisplacementRegistry, &Z80ProcessorImpl::Ld_iy_d_ind_r), // 0x77
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x78
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x79
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x7a
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x7b
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_a_hy), // 0x7c
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_a_ly), // 0x7d
            OpcodeInfo(DecodeAction::Displacement, &Z80ProcessorImpl::Ld_a_iy_d_ind), // 0x7e
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x7f

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x80
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x81
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x82
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x83
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Add_a_hy), // 0x84
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Add_a_ly), // 0x85
            OpcodeInfo(DecodeAction::Displacement, &Z80ProcessorImpl::Add_a_iy_d_ind), // 0x86
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x87
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x88
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x89
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x8a
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x8b
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Adc_a_hy), // 0x8c
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Adc_a_ly), // 0x8d
            OpcodeInfo(DecodeAction::Displacement, &Z80ProcessorImpl::Adc_a_iy_d_ind), // 0x8e
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x8f

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x90
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x91
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x92
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x93
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Sub_a_hy), // 0x94
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Sub_a_ly), // 0x95
            OpcodeInfo(DecodeAction::Displacement, &Z80ProcessorImpl::Sub_a_iy_d_ind), // 0x96
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x97
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x98
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x99
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x9a
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x9b
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Sbc_a_hy), // 0x9c
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Sbc_a_ly), // 0x9d
            OpcodeInfo(DecodeAction::Displacement, &Z80ProcessorImpl::Sbc_a_iy_d_ind), // 0x9e
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0x9f

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xa0
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xa1
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xa2
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xa3
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::And_a_hy), // 0xa4
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::And_a_ly), // 0xa5
            OpcodeInfo(DecodeAction::Displacement, &Z80ProcessorImpl::And_a_iy_d_ind), // 0xa6
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xa7
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xa8
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xa9
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xaa
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xab
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Xor_a_hy), // 0xac
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Xor_a_ly), // 0xad
            OpcodeInfo(DecodeAction::Displacement, &Z80ProcessorImpl::Xor_a_iy_d_ind), // 0xae
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xaf

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xb0
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xb1
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xb2
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xb3
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Or_a_hy), // 0xb4
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Or_a_ly), // 0xb5
            OpcodeInfo(DecodeAction::Displacement, &Z80ProcessorImpl::Or_a_iy_d_ind), // 0xb6
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xb7
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xb8
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xb9
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xba
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xbb
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Cp_a_hy), // 0xbc
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Cp_a_ly), // 0xbd
            OpcodeInfo(DecodeAction::Displacement, &Z80ProcessorImpl::Cp_a_iy_d_ind), // 0xbe
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xbf

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xc0
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xc1
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xc2
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xc3
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xc4
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xc5
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xc6
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xc7
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xc8
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xc9
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xca
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xcb //
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xcc
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xcd
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xce
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xcf

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xd0
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xd1
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xd2
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xd3
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xd4
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xd5
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xd6
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xd7
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xd8
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xd9
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xda
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xdb
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xdc
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xdd
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xde
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xdf

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xe0
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Pop_iy), // 0xe1
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xe2
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ex_sp_ind_iy), // 0xe3
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xe4
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Push_iy), // 0xe5
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xe6
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xe7
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xe8
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Jp_iy_ind), // 0xe9
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xea
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xeb
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xec
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xed
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xee
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xef

            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xf0
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xf1
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xf2
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xf3
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xf4
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xf5
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xf6
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xf7
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xf8
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Ld_sp_iy), // 0xf9
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xfa
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xfb
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xfc
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xfd
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xfe
            OpcodeInfo(DecodeAction::None, &Z80ProcessorImpl::Noni), // 0xff
        },

        // DDCB prefix
        {
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rlc_ix_d_ind_Ld_b), // 0x00
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rlc_ix_d_ind_Ld_c), // 0x01
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rlc_ix_d_ind_Ld_d), // 0x02
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rlc_ix_d_ind_Ld_e), // 0x03
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rlc_ix_d_ind_Ld_h), // 0x04
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rlc_ix_d_ind_Ld_l), // 0x05
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rlc_ix_d_ind), // 0x06
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rlc_ix_d_ind_Ld_a), // 0x07
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rrc_ix_d_ind_Ld_b), // 0x08
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rrc_ix_d_ind_Ld_c), // 0x09
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rrc_ix_d_ind_Ld_d), // 0x0a
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rrc_ix_d_ind_Ld_e), // 0x0b
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rrc_ix_d_ind_Ld_h), // 0x0c
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rrc_ix_d_ind_Ld_l), // 0x0d
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rrc_ix_d_ind), // 0x0e
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rrc_ix_d_ind_Ld_a), // 0x0f

            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rl_ix_d_ind_Ld_b), // 0x10
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rl_ix_d_ind_Ld_c), // 0x11
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rl_ix_d_ind_Ld_d), // 0x12
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rl_ix_d_ind_Ld_e), // 0x13
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rl_ix_d_ind_Ld_h), // 0x14
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rl_ix_d_ind_Ld_l), // 0x15
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rl_ix_d_ind), // 0x16
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rl_ix_d_ind_Ld_a), // 0x17
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rr_ix_d_ind_Ld_b), // 0x18
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rr_ix_d_ind_Ld_c), // 0x19
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rr_ix_d_ind_Ld_d), // 0x1a
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rr_ix_d_ind_Ld_e), // 0x1b
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rr_ix_d_ind_Ld_h), // 0x1c
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rr_ix_d_ind_Ld_l), // 0x1d
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rr_ix_d_ind), // 0x1e
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rr_ix_d_ind_Ld_a), // 0x1f

            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sla_ix_d_ind_Ld_b), // 0x20
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sla_ix_d_ind_Ld_c), // 0x21
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sla_ix_d_ind_Ld_d), // 0x22
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sla_ix_d_ind_Ld_e), // 0x23
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sla_ix_d_ind_Ld_h), // 0x24
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sla_ix_d_ind_Ld_l), // 0x25
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sla_ix_d_ind), // 0x26
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sla_ix_d_ind_Ld_a), // 0x27
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sra_ix_d_ind_Ld_b), // 0x28
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sra_ix_d_ind_Ld_c), // 0x29
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sra_ix_d_ind_Ld_d), // 0x2a
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sra_ix_d_ind_Ld_e), // 0x2b
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sra_ix_d_ind_Ld_h), // 0x2c
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sra_ix_d_ind_Ld_l), // 0x2d
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sra_ix_d_ind), // 0x2e
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sra_ix_d_ind_Ld_a), // 0x2f

            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sll_ix_d_ind_Ld_b), // 0x30
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sll_ix_d_ind_Ld_c), // 0x31
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sll_ix_d_ind_Ld_d), // 0x32
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sll_ix_d_ind_Ld_e), // 0x33
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sll_ix_d_ind_Ld_h), // 0x34
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sll_ix_d_ind_Ld_l), // 0x35
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sll_ix_d_ind), // 0x36
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sll_ix_d_ind_Ld_a), // 0x37
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Srl_ix_d_ind_Ld_b), // 0x38
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Srl_ix_d_ind_Ld_c), // 0x39
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Srl_ix_d_ind_Ld_d), // 0x3a
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Srl_ix_d_ind_Ld_e), // 0x3b
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Srl_ix_d_ind_Ld_h), // 0x3c
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Srl_ix_d_ind_Ld_l), // 0x3d
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Srl_ix_d_ind), // 0x3e
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Srl_ix_d_ind_Ld_a), // 0x3f

            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x40
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x41
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x42
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x43
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x44
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x45
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x46
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x47
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x48
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x49
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x4a
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x4b
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x4c
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x4d
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x4e
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x4f

            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x50
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x51
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x52
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x53
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x54
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x55
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x56
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x57
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x58
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x59
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x5a
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x5b
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x5c
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x5d
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x5e
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x5f

            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x60
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x61
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x62
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x63
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x64
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x65
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x66
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x67
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x68
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x69
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x6a
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x6b
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x6c
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x6d
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x6e
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x6f

            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x70
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x71
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x72
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x73
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x74
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x75
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x76
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x77
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x78
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x79
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x7a
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x7b
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x7c
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x7d
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x7e
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_ix_d_ind), // 0x7f

            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_b), // 0x80
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_c), // 0x81
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_d), // 0x82
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_e), // 0x83
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_h), // 0x84
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_l), // 0x85
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind), // 0x86
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_a), // 0x87
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_b), // 0x88
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_c), // 0x89
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_d), // 0x8a
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_e), // 0x8b
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_h), // 0x8c
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_l), // 0x8d
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind), // 0x8e
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_a), // 0x8f

            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_b), // 0x90
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_c), // 0x91
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_d), // 0x92
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_e), // 0x93
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_h), // 0x94
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_l), // 0x95
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind), // 0x96
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_a), // 0x97
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_b), // 0x98
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_c), // 0x99
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_d), // 0x9a
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_e), // 0x9b
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_h), // 0x9c
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_l), // 0x9d
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind), // 0x9e
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_a), // 0x9f

            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_b), // 0xa0
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_c), // 0xa1
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_d), // 0xa2
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_e), // 0xa3
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_h), // 0xa4
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_l), // 0xa5
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind), // 0xa6
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_a), // 0xa7
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_b), // 0xa8
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_c), // 0xa9
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_d), // 0xaa
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_e), // 0xab
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_h), // 0xac
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_l), // 0xad
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind), // 0xae
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_a), // 0xaf

            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_b), // 0xb0
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_c), // 0xb1
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_d), // 0xb2
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_e), // 0xb3
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_h), // 0xb4
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_l), // 0xb5
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind), // 0xb6
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_a), // 0xb7
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_b), // 0xb8
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_c), // 0xb9
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_d), // 0xba
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_e), // 0xbb
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_h), // 0xbc
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_l), // 0xbd
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind), // 0xbe
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_ix_d_ind_Ld_a), // 0xbf

            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_b), // 0xc0
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_c), // 0xc1
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_d), // 0xc2
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_e), // 0xc3
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_h), // 0xc4
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_l), // 0xc5
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind), // 0xc6
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_a), // 0xc7
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_b), // 0xc8
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_c), // 0xc9
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_d), // 0xca
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_e), // 0xcb
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_h), // 0xcc
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_l), // 0xcd
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind), // 0xce
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_a), // 0xcf

            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_b), // 0xd0
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_c), // 0xd1
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_d), // 0xd2
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_e), // 0xd3
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_h), // 0xd4
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_l), // 0xd5
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind), // 0xd6
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_a), // 0xd7
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_b), // 0xd8
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_c), // 0xd9
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_d), // 0xda
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_e), // 0xdb
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_h), // 0xdc
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_l), // 0xdd
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind), // 0xde
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_a), // 0xdf

            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_b), // 0xe0
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_c), // 0xe1
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_d), // 0xe2
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_e), // 0xe3
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_h), // 0xe4
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_l), // 0xe5
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind), // 0xe6
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_a), // 0xe7
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_b), // 0xe8
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_c), // 0xe9
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_d), // 0xea
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_e), // 0xeb
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_h), // 0xec
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_l), // 0xed
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind), // 0xee
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_a), // 0xef

            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_b), // 0xf0
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_c), // 0xf1
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_d), // 0xf2
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_e), // 0xf3
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_h), // 0xf4
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_l), // 0xf5
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind), // 0xf6
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_a), // 0xf7
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_b), // 0xf8
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_c), // 0xf9
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_d), // 0xfa
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_e), // 0xfb
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_h), // 0xfc
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_l), // 0xfd
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind), // 0xfe
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_ix_d_ind_Ld_a), // 0xff
        },

        // FDCB prefix
        {
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rlc_iy_d_ind_Ld_b), // 0x00
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rlc_iy_d_ind_Ld_c), // 0x01
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rlc_iy_d_ind_Ld_d), // 0x02
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rlc_iy_d_ind_Ld_e), // 0x03
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rlc_iy_d_ind_Ld_h), // 0x04
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rlc_iy_d_ind_Ld_l), // 0x05
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rlc_iy_d_ind), // 0x06
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rlc_iy_d_ind_Ld_a), // 0x07
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rrc_iy_d_ind_Ld_b), // 0x08
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rrc_iy_d_ind_Ld_c), // 0x09
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rrc_iy_d_ind_Ld_d), // 0x0a
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rrc_iy_d_ind_Ld_e), // 0x0b
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rrc_iy_d_ind_Ld_h), // 0x0c
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rrc_iy_d_ind_Ld_l), // 0x0d
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rrc_iy_d_ind), // 0x0e
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rrc_iy_d_ind_Ld_a), // 0x0f

            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rl_iy_d_ind_Ld_b), // 0x10
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rl_iy_d_ind_Ld_c), // 0x11
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rl_iy_d_ind_Ld_d), // 0x12
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rl_iy_d_ind_Ld_e), // 0x13
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rl_iy_d_ind_Ld_h), // 0x14
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rl_iy_d_ind_Ld_l), // 0x15
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rl_iy_d_ind), // 0x16
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rl_iy_d_ind_Ld_a), // 0x17
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rr_iy_d_ind_Ld_b), // 0x18
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rr_iy_d_ind_Ld_c), // 0x19
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rr_iy_d_ind_Ld_d), // 0x1a
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rr_iy_d_ind_Ld_e), // 0x1b
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rr_iy_d_ind_Ld_h), // 0x1c
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rr_iy_d_ind_Ld_l), // 0x1d
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rr_iy_d_ind), // 0x1e
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Rr_iy_d_ind_Ld_a), // 0x1f

            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sla_iy_d_ind_Ld_b), // 0x20
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sla_iy_d_ind_Ld_c), // 0x21
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sla_iy_d_ind_Ld_d), // 0x22
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sla_iy_d_ind_Ld_e), // 0x23
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sla_iy_d_ind_Ld_h), // 0x24
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sla_iy_d_ind_Ld_l), // 0x25
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sla_iy_d_ind), // 0x26
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sla_iy_d_ind_Ld_a), // 0x27
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sra_iy_d_ind_Ld_b), // 0x28
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sra_iy_d_ind_Ld_c), // 0x29
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sra_iy_d_ind_Ld_d), // 0x2a
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sra_iy_d_ind_Ld_e), // 0x2b
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sra_iy_d_ind_Ld_h), // 0x2c
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sra_iy_d_ind_Ld_l), // 0x2d
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sra_iy_d_ind), // 0x2e
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sra_iy_d_ind_Ld_a), // 0x2f

            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sll_iy_d_ind_Ld_b), // 0x30
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sll_iy_d_ind_Ld_c), // 0x31
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sll_iy_d_ind_Ld_d), // 0x32
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sll_iy_d_ind_Ld_e), // 0x33
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sll_iy_d_ind_Ld_h), // 0x34
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sll_iy_d_ind_Ld_l), // 0x35
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sll_iy_d_ind), // 0x36
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Sll_iy_d_ind_Ld_a), // 0x37
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Srl_iy_d_ind_Ld_b), // 0x38
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Srl_iy_d_ind_Ld_c), // 0x39
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Srl_iy_d_ind_Ld_d), // 0x3a
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Srl_iy_d_ind_Ld_e), // 0x3b
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Srl_iy_d_ind_Ld_h), // 0x3c
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Srl_iy_d_ind_Ld_l), // 0x3d
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Srl_iy_d_ind), // 0x3e
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Srl_iy_d_ind_Ld_a), // 0x3f

            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x40
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x41
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x42
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x43
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x44
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x45
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x46
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x47
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x48
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x49
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x4a
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x4b
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x4c
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x4d
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x4e
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x4f

            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x50
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x51
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x52
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x53
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x54
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x55
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x56
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x57
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x58
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x59
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x5a
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x5b
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x5c
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x5d
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x5e
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x5f

            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x60
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x61
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x62
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x63
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x64
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x65
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x66
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x67
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x68
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x69
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x6a
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x6b
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x6c
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x6d
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x6e
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x6f

            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x70
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x71
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x72
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x73
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x74
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x75
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x76
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x77
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x78
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x79
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x7a
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x7b
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x7c
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x7d
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x7e
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Bit_bit_iy_d_ind), // 0x7f

            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_b), // 0x80
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_c), // 0x81
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_d), // 0x82
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_e), // 0x83
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_h), // 0x84
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_l), // 0x85
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind), // 0x86
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_a), // 0x87
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_b), // 0x88
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_c), // 0x89
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_d), // 0x8a
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_e), // 0x8b
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_h), // 0x8c
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_l), // 0x8d
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind), // 0x8e
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_a), // 0x8f

            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_b), // 0x90
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_c), // 0x91
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_d), // 0x92
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_e), // 0x93
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_h), // 0x94
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_l), // 0x95
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind), // 0x96
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_a), // 0x97
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_b), // 0x98
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_c), // 0x99
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_d), // 0x9a
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_e), // 0x9b
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_h), // 0x9c
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_l), // 0x9d
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind), // 0x9e
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_a), // 0x9f

            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_b), // 0xa0
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_c), // 0xa1
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_d), // 0xa2
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_e), // 0xa3
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_h), // 0xa4
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_l), // 0xa5
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind), // 0xa6
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_a), // 0xa7
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_b), // 0xa8
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_c), // 0xa9
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_d), // 0xaa
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_e), // 0xab
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_h), // 0xac
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_l), // 0xad
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind), // 0xae
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_a), // 0xaf

            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_b), // 0xb0
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_c), // 0xb1
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_d), // 0xb2
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_e), // 0xb3
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_h), // 0xb4
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_l), // 0xb5
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind), // 0xb6
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_a), // 0xb7
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_b), // 0xb8
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_c), // 0xb9
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_d), // 0xba
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_e), // 0xbb
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_h), // 0xbc
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_l), // 0xbd
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind), // 0xbe
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Res_bit_iy_d_ind_Ld_a), // 0xbf

            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_b), // 0xc0
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_c), // 0xc1
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_d), // 0xc2
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_e), // 0xc3
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_h), // 0xc4
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_l), // 0xc5
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind), // 0xc6
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_a), // 0xc7
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_b), // 0xc8
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_c), // 0xc9
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_d), // 0xca
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_e), // 0xcb
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_h), // 0xcc
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_l), // 0xcd
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind), // 0xce
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_a), // 0xcf

            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_b), // 0xd0
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_c), // 0xd1
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_d), // 0xd2
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_e), // 0xd3
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_h), // 0xd4
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_l), // 0xd5
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind), // 0xd6
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_a), // 0xd7
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_b), // 0xd8
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_c), // 0xd9
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_d), // 0xda
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_e), // 0xdb
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_h), // 0xdc
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_l), // 0xdd
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind), // 0xde
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_a), // 0xdf

            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_b), // 0xe0
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_c), // 0xe1
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_d), // 0xe2
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_e), // 0xe3
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_h), // 0xe4
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_l), // 0xe5
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind), // 0xe6
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_a), // 0xe7
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_b), // 0xe8
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_c), // 0xe9
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_d), // 0xea
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_e), // 0xeb
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_h), // 0xec
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_l), // 0xed
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind), // 0xee
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_a), // 0xef

            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_b), // 0xf0
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_c), // 0xf1
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_d), // 0xf2
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_e), // 0xf3
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_h), // 0xf4
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_l), // 0xf5
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind), // 0xf6
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_a), // 0xf7
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_b), // 0xf8
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_c), // 0xf9
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_d), // 0xfa
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_e), // 0xfb
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_h), // 0xfc
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_l), // 0xfd
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind), // 0xfe
            OpcodeInfo(DecodeAction::PrefixedDisplacement, &Z80ProcessorImpl::Set_bit_iy_d_ind_Ld_a), // 0xff
        }
    };

	Z80ProcessorImpl::Z80ProcessorImpl(double clockFrequency, uint8_t ram[], Emulator::IOHandler& ioHandler,
		const std::string tag, Emulator::IMemoryHandler* memoryHandler, Emulator::Cpu& cpu) :
#pragma warning (disable: 4355) // 'this' : used in base member initializer list
		irqLine(cpu, nullptr, 0x000000ff),
		nmiLine(cpu, [this](Emulator::LineState state) {
			this->OnNmiLineChanged(state);
		}, 0x000000ff),
#pragma warning (default: 4355)
		haltLine(cpu, nullptr, 0x000000ff),
		resetLine(cpu, nullptr, 0x000000ff),

		//cpu(cpu),
		tag(tag),
		clockFrequency(clockFrequency),
		halt(false),
		ram(ram),
		ioHandler(ioHandler),
		memoryHandler(memoryHandler),

		aborted(false),
		cyclesLeft(0),
		cyclesInCurrentTimeslice(0)
	{
        ResetStatus();
        InitFlagArrays();
	}

	Z80ProcessorImpl::~Z80ProcessorImpl()
	{
	}

    void Z80ProcessorImpl::InitFlagArrays()
    {
        for (int i = 0; i < 256; i++)
        {
            int totBits = 0;
            if ((i & 0x01) != 0)
            {
                ++totBits;
            }
            if ((i & 0x02) != 0)
            {
                ++totBits;
            }
            if ((i & 0x04) != 0)
            {
                ++totBits;
            }
            if ((i & 0x08) != 0)
            {
                ++totBits;
            }
            if ((i & 0x10) != 0)
            {
                ++totBits;
            }
            if ((i & 0x20) != 0)
            {
                ++totBits;
            }
            if ((i & 0x40) != 0)
            {
                ++totBits;
            }
            if ((i & 0x80) != 0)
            {
                ++totBits;
            }

            flag_SZ[i] = (uint8_t)((i != 0) ? (i & S_mask) : Z_mask);

            flag_SZ_BIT[i] = (uint8_t)((i != 0) ? (i & S_mask) : (Z_mask | PV_mask));

            flag_SZP[i] = (uint8_t)(flag_SZ[i] | ((0 != (totBits % 2)) ? 0 : PV_mask));

            flag_SZHV_inc[i] = flag_SZ[i];
            if (i == 0x80)
            {
                flag_SZHV_inc[i] |= PV_mask;
            }
            if ((i & 0x0f) == 0x00)
            {
                flag_SZHV_inc[i] |= H_mask;
            }

            flag_SZHV_dec[i] = (uint8_t)(flag_SZ[i] | N_mask);
            if (i == 0x7f)
            {
                flag_SZHV_dec[i] |= PV_mask;
            }
            if ((i & 0x0f) == 0x0f)
            {
                flag_SZHV_dec[i] |= H_mask;
            }
        }

        for (int oldval = 0; oldval < 256; oldval++)
        {
            for (int newval = 0; newval < 256; newval++)
            {
                // add or adc with carry == 0
                int val = newval - oldval;
                uint8_t flags = (uint8_t)((0 != newval) ? ((0 != (newval & 0x80)) ? S_mask : 0) : Z_mask);
                if ((newval & 0x0f) < (oldval & 0x0f))
                {
                    flags |= H_mask;
                }
                    
                if (newval < oldval)
                {
                    flags |= C_mask;
                }

                if (0 != ((val ^ oldval ^ 0x80) & (val ^ newval) & 0x80))
                {
                    flags |= PV_mask;
                }

                flag_SZHVC_add[oldval][newval][0] = flags;

                // adc with carry set
                val = newval - oldval - 1;
                flags = (uint8_t)((0 != newval) ? ((0 != (newval & 0x80)) ? S_mask : 0) : Z_mask);
                if ((newval & 0x0f) <= (oldval & 0x0f))
                {
                    flags |= H_mask;
                }

                if (newval <= oldval)
                {
                    flags |= C_mask;
                }

                if (0 != ((val ^ oldval ^ 0x80) & (val ^ newval) & 0x80))
                {
                    flags |= PV_mask;
                }

                flag_SZHVC_add[oldval][newval][1] = flags;

                // cp, sub or sbc with carry == 0
                val = oldval - newval;
                flags = (uint8_t)(N_mask | ((0 != newval) ? ((0 != (newval & 0x80)) ? S_mask : 0) : Z_mask));
                if ((newval & 0x0f) > (oldval & 0x0f))
                {
                    flags |= H_mask;
                }

                if (newval > oldval)
                {
                    flags |= C_mask;
                }

                if (0 != ((val ^ oldval) & (oldval ^ newval) & 0x80))
                {
                    flags |= PV_mask;
                }

                flag_SZHVC_sub[oldval][newval][0] = flags;
                    
                // sbc with carry set
                val = oldval - newval - 1;
                flags = (uint8_t)(N_mask | ((0 != newval) ? ((0 != (newval & 0x80)) ? S_mask : 0) : Z_mask));
                //flags |= (uint8_t)((newval & (Y_mask | X_mask)));	// undocumented flag bits 5+3
                if ((newval & 0x0f) >= (oldval & 0x0f))
                {
                    flags |= H_mask;
                }

                if (newval >= oldval)
                {
                    flags |= C_mask;
                }

                if (0 != ((val ^ oldval) & (oldval ^ newval) & 0x80))
                {
                    flags |= PV_mask;
                }

                flag_SZHVC_sub[oldval][newval][1] = flags;
            }
        }
    }

	void Z80ProcessorImpl::ResetStatus()
    {
        set_B(0);
		set_C(0);
		set_D(0);
		set_E(0);
		set_H(0);
		set_L(0);
		set_A(0);

		reg[6] =0;
        set_ZeroFlag(true); // Zero flag is set 
        IX = IY = 0xffff; // IX and IY are FFFF after a reset!
        PC = 0;
        SP = 0;
        I = R = 0;
        nmiState = Emulator::LineState::Clear;
        isNmiPending = false;
        IFF1 = IFF2 = false;
        irqVector = 0;
        interruptMode = Mode0;
        AfterEI = false;
    }

    Z80ProcessorImpl::Instruction Z80ProcessorImpl::FetchDecode(uint16_t pc)
    {
        uint16_t pcInit = pc;

        // 1. parse prefixes
        uint8_t opcode = ReadMemory(pc++);
        if (0xcb == opcode)
        {
            this->instr.prefix = Instruction::Prefix::CB;
            opcode = ReadMemory(pc++);
            this->instr.cycles = cc_cb[opcode];
        }
        else if (0xed == opcode)
        {
            this->instr.prefix = Instruction::Prefix::ED;
            opcode = ReadMemory(pc++);
            this->instr.cycles = cc_ed[opcode];
        }
        else if (0xdd == opcode)
        {
            this->instr.prefix = Instruction::Prefix::DD;
            opcode = ReadMemory(pc++);
            if (0xcb == opcode)
            {
                this->instr.prefix = Instruction::Prefix::DDCB;
                this->instr.immediate = ReadMemory(pc++);
                opcode = ReadMemory(pc++);
                this->instr.cycles = cc_xycb[opcode];
            }
            else
            {
                this->instr.cycles = cc_xy[opcode];
            }
        }
        else if (0xfd == opcode)
        {
            this->instr.prefix = Instruction::Prefix::FD;
            opcode = ReadMemory(pc++);
            if (0xcb == opcode)
            {
                this->instr.prefix = Instruction::Prefix::FDCB;
                this->instr.immediate = ReadMemory(pc++);
                opcode = ReadMemory(pc++);
                this->instr.cycles = cc_xycb[opcode];
            }
            else
            {
                this->instr.cycles = cc_xy[opcode];
            }
        }
        else
        {
            this->instr.prefix = Instruction::Prefix::None;
            this->instr.cycles = cc_op[opcode];
        }

        // 2. parse opcode
        this->instr.opcode = opcode;

        // 3. Table Look up based on opcode to find action and function pointer
        OpcodeInfo opcodeInfo = fetchDecodeTable[(int)this->instr.prefix][opcode];
        DecodeAction decodeAction = opcodeInfo.decodeAction;
        this->instr.execute = opcodeInfo.interpreterFunc;

        // 4. Operand Resolution
        switch (decodeAction)
        {
			case DecodeAction::None:
                break;

            case DecodeAction::Register:
                this->instr._register = (Instruction::Register)(opcode & 7);
                break;

            case DecodeAction::Immediate:
                this->instr.immediate = ReadMemory(pc++);
                break;

            case DecodeAction::ImmediateExtended:
                this->instr.immediateExtended = ReadMemory16(pc);
                pc += 2;
                break;

            case DecodeAction::Displacement:
                this->instr.displacement = ReadMemory(pc++);
                break;

            case DecodeAction::PrefixedDisplacement:
                this->instr.displacement = this->instr.immediate;
                break;

            case DecodeAction::DisplacementImmediate:
                this->instr.displacement = ReadMemory(pc++);
                this->instr.immediate = ReadMemory(pc++);
                break;

            case DecodeAction::DisplacementRegistry:
                this->instr._register = (Instruction::Register)(opcode & 7);
                this->instr.displacement = ReadMemory(pc++);
                break;
        }

        this->instr.length = pc - pcInit;
        return instr;
    }

	void Z80ProcessorImpl::Reset()
	{
		ResetStatus();
	}

	int64_t Z80ProcessorImpl::get_ExecutionTimeInCurrentTimeslice() const
	{
		int64_t cycles = cyclesInCurrentTimeslice - cyclesLeft;
		return (int64_t)(1e6 * cycles / clockFrequency);
	}


    // Execute 'cycles' T-states. Return number of T-states really executed
    int64_t Z80ProcessorImpl::Execute(int64_t us) // microseconds
    {
		aborted = false;
		cyclesInCurrentTimeslice = (int64_t)(clockFrequency * us / 1e6);
        cyclesLeft = cyclesInCurrentTimeslice;

        // check for NMIs
        if (this->isNmiPending)
        {
            if (halt) // Check if processor was halted
            {
                LeaveHalt();
            }

            this->IFF1 = false;
            Push_pc();
            this->PC = 0x0066;
            cyclesLeft -= 11;
            this->isNmiPending = false;
        }

        do
        {
            // check for IRQs before each instruction
            if (this->irqLine.get_State() != Emulator::LineState::Clear && this->IFF1 && !this->AfterEI)
            {
                TakeInterrupt();
            }
            this->AfterEI = false;

            // Callback call with current Program Counter value.
            if (_programCounterListener)
                _programCounterListener(this->PC);

            FetchDecode(this->PC);
            this->PC += (uint16_t)(this->instr.length);
            this->R++;
			Action execInstruction = this->instr.execute;
            ((*this).*execInstruction)();
			if (this->aborted) {
				cyclesLeft = 0;
			}
			else {
				cyclesLeft -= this->instr.cycles;
			}
        }
        while (!halt && cyclesLeft > 0);

		return get_ExecutionTimeInCurrentTimeslice();
    }

	void Z80ProcessorImpl::TakeInterrupt()
    {
        if (halt)
        {
            LeaveHalt();
        }

        // clear both interrupt flip flops 
        this->IFF1 = this->IFF2 = false;
        this->irqVector = irqLine.OnIrq();

        //Logger.Log(string.Format("z80 interrupt v: {0} mode: {1}", this->irqVector, this->interruptMode));
        switch (this->interruptMode)
        {
			case InterruptMode::Mode0:
                // Interrupt mode 0. We check for CALL and JP instructions, if neither
                // of these were found we assume a 1 uint8_t opcode was placed on the databus.
                if (this->irqVector != 0x00) // check for nop
                {
                    switch (this->irqVector & 0xff0000)
                    {
                        case 0xcd0000:	// call 
                            Push_pc();
                            this->PC = (uint16_t)(this->irqVector & 0xffff);
                            cyclesLeft -= cc_op[0xcd]; // CALL $xxxx cycles
                            break;

                        case 0xc30000:	// jump 
                            this->PC = (uint16_t)(this->irqVector & 0xffff);
                            cyclesLeft -= cc_op[0xc3]; // JP $xxxx cycles
                            break;

                        default:		// rst (or other opcodes?) 
                            Push_pc();
                            this->PC = (uint16_t)(this->irqVector & 0x0038);
                            cyclesLeft -= cc_op[0xff]; // RST $xx cycles
                            break;
                    }
                }

                // 'interrupt latency' cycles
                cyclesLeft -= cc_ex[0xff];
                break;

			case InterruptMode::Mode1:
                // Interrupt mode 1: RST 38h 
                Push_pc();
                this->PC = 0x0038; //Log = true;
                cyclesLeft -= (cc_op[0xff] + cc_ex[0xff]); // RST $38 + 'interrupt latency' cycles 
                break;

			case InterruptMode::Mode2:
                // Interrupt mode 2: Call [i:datauint8_t]
                this->irqVector = ((uint16_t)(this->irqVector & 0xff)) | ((uint16_t)(this->I << 8));
                Push_pc();
                this->PC = ReadMemory16((uint16_t)this->irqVector);
                cyclesLeft -= (cc_op[0xcd] + cc_ex[0xff]); // CALL opcode timing + 'interrupt latency' cycles
                break;
        }
    }

	void Z80ProcessorImpl::AbortTimeslice()
    {
        aborted = true;
    }

	std::vector<Emulator::IDeviceInputLine*> Z80ProcessorImpl::get_InputLines()
	{
		std::vector<Emulator::IDeviceInputLine*> result;
		result.push_back(&irqLine);
		result.push_back(&nmiLine);
		result.push_back(&haltLine);
		result.push_back(&resetLine);
		return result;
	}
}
