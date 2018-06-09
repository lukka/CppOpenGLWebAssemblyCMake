#include <math.h>
#include "OpAmpFilter.h"

/************************************************************************
 *
 * DST_FILTER2 - Generic 2nd order filter
 *
 * input[0]    - Enable input value
 * input[1]    - input value
 * input[2]    - Frequency value (initialization only)
 * input[3]    - Damping value (initialization only)
 * input[4]    - Filter type (initialization only)
 *
 ************************************************************************/
//#define DST_FILTER2__ENABLE	DISCRETE_INPUT(0)
//#define DST_FILTER2__IN		DISCRETE_INPUT(1)
//#define DST_FILTER2__FREQ	DISCRETE_INPUT(2)
//#define DST_FILTER2__DAMP	DISCRETE_INPUT(3)
//#define DST_FILTER2__TYPE	DISCRETE_INPUT(4)

namespace Emulator
{

static void calculateFilter2Coefficients(DiscreteSoundNode *node,
		                                 double fc, double d, double type,
		                                 struct discrete_filter_coeff &coeff)
{
	double w;	/* cutoff freq, in radians/sec */
	double w_squared;
	double den;	/* temp variable */
	double two_over_T = 2 * node->sampleRate();
	double two_over_T_squared = two_over_T * two_over_T;

	/* calculate digital filter coefficents */
	/*w = 2.0*M_PI*fc; no pre-warping */
	w = node->sampleRate() * 2.0 * tan(M_PI * fc / node->sampleRate()); /* pre-warping */
	w_squared = w * w;

	den = two_over_T_squared + d * w * two_over_T + w_squared;

	coeff.a1 = 2.0 * (-two_over_T_squared + w_squared) / den;
	coeff.a2 = (two_over_T_squared - d * w * two_over_T + w_squared) / den;

	if (type == DISC_FILTER_LOWPASS)
	{
		coeff.b0 = coeff.b2 = w_squared/den;
		coeff.b1 = 2.0 * (coeff.b0);
	}
	else if (type == DISC_FILTER_BANDPASS)
	{
		coeff.b0 = d * w * two_over_T / den;
		coeff.b1 = 0.0;
		coeff.b2 = -(coeff.b0);
	}
	else if (type == DISC_FILTER_HIGHPASS)
	{
		coeff.b0 = coeff.b2 = two_over_T_squared / den;
		coeff.b1 = -2.0 * (coeff.b0);
	}
}

void OpAmpFilter::step()
{
	double v_out = 0;

	double i, v = 0;

	if (0 != getInput(0)) // DST_FILTER2__ENABLE
	{
		if (m_is_norton)
		{
			v = getInput(1) - OP_AMP_NORTON_VBE; // DST_OP_AMP_FILT__INP1
			if (v < 0) v = 0;
		}
		else
		{
			/* Millman the input voltages. */
			i  = m_iFixed;
			switch (m_type)
			{
				case DISC_OP_AMP_FILTER_IS_LOW_PASS_1_A:
					i += (getInput(1) - getInput(2)) / m_info.r1;
					if (m_info.r2 != 0)
						i += (m_vP - getInput(2)) / m_info.r2;
					if (m_info.r3 != 0)
						i += (m_vN - getInput(2)) / m_info.r3;
					break;

				default:
					i += (getInput(1) - m_vRef) / m_info.r1;
					if (m_info.r2 != 0)
						i += (getInput(2) - m_vRef) / m_info.r2;
					break;
			}
			v = i * m_rTotal;
		}

		switch (m_type)
		{
			case DISC_OP_AMP_FILTER_IS_LOW_PASS_1:
				m_vC1 += (v - m_vC1) * m_exponentC1;
				v_out = m_vC1 * m_gain + m_info.vRef;
				break;

			case DISC_OP_AMP_FILTER_IS_LOW_PASS_1_A:
				m_vC1 += (v - m_vC1) * m_exponentC1;
				v_out = m_vC1 * m_gain + getInput(2);
				break;

			case DISC_OP_AMP_FILTER_IS_HIGH_PASS_1:
				v_out = (v - m_vC1) * m_gain + m_info.vRef;
				m_vC1 += (v - m_vC1) * m_exponentC1;
				break;

			case DISC_OP_AMP_FILTER_IS_BAND_PASS_1:
				v_out = (v - m_vC2);
				m_vC2 += (v - m_vC2) * m_exponentC2;
				m_vC1 += (v_out - m_vC1) * m_exponentC1;
				v_out = m_vC1 * m_gain + m_info.vRef;
				break;

			case DISC_OP_AMP_FILTER_IS_BAND_PASS_0 | DISC_OP_AMP_IS_NORTON:
				m_vC1 += (v - m_vC1) * m_exponentC1;
				m_vC2 += (m_vC1 - m_vC2) * m_exponentC2;
				v = m_vC2;
				v_out = v - m_vC3;
				m_vC3 += (v - m_vC3) * m_exponentC3;
				i = v_out / m_rTotal;
				v_out = (m_iFixed - i) * m_info.rF;
				break;

			case DISC_OP_AMP_FILTER_IS_HIGH_PASS_0 | DISC_OP_AMP_IS_NORTON:
				v_out = v - m_vC1;
				m_vC1 += (v - m_vC1) * m_exponentC1;
				i = v_out / m_rTotal;
				v_out = (m_iFixed - i) * m_info.rF;
				break;

			case DISC_OP_AMP_FILTER_IS_BAND_PASS_1M:
			case DISC_OP_AMP_FILTER_IS_BAND_PASS_1M | DISC_OP_AMP_IS_NORTON:
				v_out = -m_fc.a1 * m_fc.y1 - m_fc.a2 * m_fc.y2 +
								m_fc.b0 * v + m_fc.b1 * m_fc.x1 + m_fc.b2 * m_fc.x2 +
								m_vRef;
				m_fc.x2 = m_fc.x1;
				m_fc.x1 = v;
				m_fc.y2 = m_fc.y1;
				break;
		}

		/* Clip the output to the voltage rails.
         * This way we get the original distortion in all it's glory.
         */
		if (v_out > m_vP) v_out = m_vP;
		if (v_out < m_vN) v_out = m_vN;
		m_fc.y1 = v_out - m_vRef;
		setOutput(v_out);
	}
	else {
		setOutput(0);
	}

}

void OpAmpFilter::reset()
{
	/* Convert the passed filter type into an int for easy use. */
	m_type = (int)getInput(3) & DISC_OP_AMP_FILTER_TYPE_MASK; // DST_OP_AMP_FILT__TYPE
	m_is_norton = (int)getInput(3) & DISC_OP_AMP_IS_NORTON;

	if (m_is_norton)
	{
		m_vRef = 0;
		m_rTotal = m_info.r1;
		if (m_type == (DISC_OP_AMP_FILTER_IS_BAND_PASS_0 | DISC_OP_AMP_IS_NORTON)) {
			m_rTotal += m_info.r2 +  m_info.r3;
		}

		/* Setup the current to the + input. */
		m_iFixed = (m_info.vP - OP_AMP_NORTON_VBE) / m_info.r4;

		/* Set the output max. */
		m_vP =  m_info.vP - OP_AMP_NORTON_VBE;
		m_vN =  m_info.vN;
	}
	else
	{
		m_vRef = m_info.vRef;
		/* Set the output max. */
		m_vP =  m_info.vP - OP_AMP_VP_RAIL_OFFSET;
		m_vN =  m_info.vN;

		/* Work out the input resistance.  It is all input and bias resistors in parallel. */
		m_rTotal  = 1.0 / m_info.r1;			/* There has to be an R1.  Otherwise the table is wrong. */

		if (m_info.r2 != 0) {
			m_rTotal += 1.0 / m_info.r2;
		}

		if (m_info.r3 != 0) {
			m_rTotal += 1.0 / m_info.r3;
		}

		m_rTotal = 1.0 / m_rTotal;

		m_iFixed = 0;

		m_rRatio = m_info.rF / (m_rTotal + m_info.rF);
		m_gain = -m_info.rF / m_rTotal;
	}

	switch (m_type)
	{
		case DISC_OP_AMP_FILTER_IS_LOW_PASS_1:
		case DISC_OP_AMP_FILTER_IS_LOW_PASS_1_A:
			m_exponentC1 = RC_CHARGE_EXP(m_info.rF * m_info.c1);
			m_exponentC2 =  0;
			break;

		case DISC_OP_AMP_FILTER_IS_HIGH_PASS_1:
			m_exponentC1 = RC_CHARGE_EXP(m_rTotal * m_info.c1);
			m_exponentC2 =  0;
			break;

		case DISC_OP_AMP_FILTER_IS_BAND_PASS_1:
			m_exponentC1 = RC_CHARGE_EXP(m_info.rF * m_info.c1);
			m_exponentC2 = RC_CHARGE_EXP(m_rTotal * m_info.c2);
			break;

		case DISC_OP_AMP_FILTER_IS_BAND_PASS_1M | DISC_OP_AMP_IS_NORTON:
			if (m_info.r2 == 0) {
				m_rTotal = m_info.r1;
			}
			else {
				m_rTotal = RES_2_PARALLEL(m_info.r1, m_info.r2);
			}
		case DISC_OP_AMP_FILTER_IS_BAND_PASS_1M:
		{
			double fc = 1.0 / (2 * M_PI * sqrt(m_rTotal * m_info.rF * m_info.c1 * m_info.c2));
			double d  = (m_info.c1 + m_info.c2) / sqrt(m_info.rF / m_rTotal * m_info.c1 * m_info.c2);
			double gain = -m_info.rF / m_rTotal * m_info.c2 / (m_info.c1 + m_info.c2);

			calculateFilter2Coefficients(this, fc, d, DISC_FILTER_BANDPASS, m_fc);
			m_fc.b0 *= gain;
			m_fc.b1 *= gain;
			m_fc.b2 *= gain;

			if (m_is_norton) {
				m_vRef = (m_info.vP - OP_AMP_NORTON_VBE) / m_info.r3 * m_info.rF;
			}
			else {
				m_vRef = m_info.vRef;
			}
			break;
		}

		case DISC_OP_AMP_FILTER_IS_BAND_PASS_0 | DISC_OP_AMP_IS_NORTON:
			m_exponentC1 = RC_CHARGE_EXP(RES_2_PARALLEL(m_info.r1, m_info.r2 + m_info.r3 + m_info.r4) * m_info.c1);
			m_exponentC2 = RC_CHARGE_EXP(RES_2_PARALLEL(m_info.r1 + m_info.r2, m_info.r3 + m_info.r4) * m_info.c2);
			m_exponentC3 = RC_CHARGE_EXP((m_info.r1 + m_info.r2 + m_info.r3 + m_info.r4) * m_info.c3);
			break;

		case DISC_OP_AMP_FILTER_IS_HIGH_PASS_0 | DISC_OP_AMP_IS_NORTON:
			m_exponentC1 = RC_CHARGE_EXP(m_info.r1 * m_info.c1);
			break;
	}

	/* At startup there is no charge on the caps and output is 0V in relation to vRef. */
	m_vC1 = 0;
	m_vC1b = 0;
	m_vC2 = 0;
	m_vC3 = 0;

	setOutput(m_info.vRef);
}

} // namespace Emulator
