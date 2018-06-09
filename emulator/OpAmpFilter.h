#pragma once

#include "DiscreteSoundNode.h"

/************************************************************************
 *
 * DST_OP_AMP_FILT - Op Amp filter circuit RC filter
 *
 * input[0]    - Enable input value
 * input[1]    - IN0 node
 * input[2]    - IN1 node
 * input[3]    - Filter Type
 *
 * also passed discrete_op_amp_filt_info structure
 *
 * Mar 2004, D Renaud.
 ************************************************************************/
#define DST_OP_AMP_FILT__ENABLE	DISCRETE_INPUT(0)
#define DST_OP_AMP_FILT__INP1	DISCRETE_INPUT(1)
#define DST_OP_AMP_FILT__INP2	DISCRETE_INPUT(2)
#define DST_OP_AMP_FILT__TYPE	DISCRETE_INPUT(3)

/* Common Op Amp Flags and values */
#define DISC_OP_AMP_IS_NORTON				0x100
#define OP_AMP_NORTON_VBE					0.5		// This is the norton junction voltage. Used only internally.
#define OP_AMP_VP_RAIL_OFFSET				1.5		// This is how close an op-amp can get to the vP rail. Used only internally.

/* Filter types */
#define DISC_FILTER_LOWPASS					0
#define DISC_FILTER_HIGHPASS				1
#define DISC_FILTER_BANDPASS				2

/* Op Amp Filter Options */
#define DISC_OP_AMP_FILTER_IS_LOW_PASS_1	0x00
#define DISC_OP_AMP_FILTER_IS_HIGH_PASS_1	0x10
#define DISC_OP_AMP_FILTER_IS_BAND_PASS_1	0x20
#define DISC_OP_AMP_FILTER_IS_BAND_PASS_1M	0x30
#define DISC_OP_AMP_FILTER_IS_HIGH_PASS_0	0x40
#define DISC_OP_AMP_FILTER_IS_BAND_PASS_0	0x50
#define DISC_OP_AMP_FILTER_IS_LOW_PASS_1_A	0x60

#define DISC_OP_AMP_FILTER_TYPE_MASK		(0xf0 | DISC_OP_AMP_IS_NORTON)	// Used only internally.

namespace Emulator
{

struct discrete_filter_coeff
{
	double x1, x2;		/* x[k-1], x[k-2], previous 2 input values */
	double y1, y2;		/* y[k-1], y[k-2], previous 2 output values */
	double a1, a2;		/* digital filter coefficients, denominator */
	double b0, b1, b2;	/* digital filter coefficients, numerator */
};

class OpAmpFilter : public DiscreteSoundNode
{
public:
	//struct Desc
	//{
	//	int		type;
	//	double	r[DISC_MAX_MIXER_INPUTS];		/* static input resistance values.  These are in series with rNode, if used. */
	//	int		r_node[DISC_MAX_MIXER_INPUTS];	/* variable resistance nodes, if needed.  0 if not used. */
	//	double	c[DISC_MAX_MIXER_INPUTS];
	//	double	rI;
	//	double	rF;
	//	double	cF;
	//	double	cAmp;
	//	double	vRef;
	//	double	gain;				/* Scale value to get output close to +/- 32767 */
	//};

	struct Info
	{
		double	r1;
		double	r2;
		double	r3;
		double	r4;
		double	rF;
		double	c1;
		double	c2;
		double	c3;
		double	vRef;
		double	vP;
		double	vN;
	};

	OpAmpFilter(DiscreteDevice* pDevice, const Info& info) :
		DiscreteSoundNode(pDevice),
		m_info(info)
	{
	}
	virtual ~OpAmpFilter() {}

	void step(void);
	void reset(void);
	int max_output(void) { return 1; }

private:
	Info			m_info;
	int				m_type;					/* What kind of filter */
	int				m_is_norton;			/* 1 = Norton op-amps */
	double			m_vRef;
	double			m_vP;
	double			m_vN;
	double			m_rTotal;				/* All input resistance in parallel. */
	double			m_iFixed;				/* Current supplied by r3 & r4 if used. */
	double			m_exponentC1;
	double			m_exponentC2;
	double			m_exponentC3;
	double			m_rRatio;				/* divide ratio of resistance network */
	double			m_vC1;					/* Charge on C1 */
	double			m_vC1b;					/* Charge on C1, part of C1 charge if needed */
	double			m_vC2;					/* Charge on C2 */
	double			m_vC3;					/* Charge on C2 */
	double			m_gain;					/* Gain of the filter */
	struct discrete_filter_coeff m_fc;
};

} // namespace Emulator