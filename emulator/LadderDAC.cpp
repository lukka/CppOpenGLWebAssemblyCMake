#include <math.h>
#include "LadderDAC.h"

namespace Emulator
{

void LadderDAC::step()
{
	int	data = (int)getInput(0); // DST_DAC_R1__DATA
	double v = m_v_step[data];
	double x_time = getInput(0) - data;
	double last_v = m_last_v;

	m_last_v = v;

	if (x_time > 0) {
		v = x_time * (v - last_v) + last_v;
	}

	/* Filter if needed, else just output voltage */
	if (m_has_c_filter)
	{
		double v_diff = v - m_v_out;
		/* optimization - if charged close enough to voltage */
		if (fabs(v_diff) < 0.000001) {
			m_v_out = v;
		}
		else {
			m_v_out += v_diff * m_exponent;
		}
	}
	else {
		m_v_out = v;
	}

	setOutput(m_v_out);
}

void LadderDAC::reset()
{
	int	bit;
	int ladderLength = m_info.ladderLength;
	int total_steps = 1 << ladderLength;
	double r_total = 0;
	double i_bias;
	double v_on = getInput(1); // DST_DAC_R1__VON

	m_last_v = 0;

	/* Calculate the Millman current of the bias circuit */
	if (m_info.rBias > 0) {
		i_bias = m_info.vBias / m_info.rBias;
	}
	else {
		i_bias = 0;
	}

	/*
     * We will do a small amount of error checking.
     * But if you are an idiot and pass a bad ladder table
     * then you deserve a crash.
     */
	if (ladderLength < 2 && m_info.rBias == 0 && m_info.rGnd == 0)
	{
		/* You need at least 2 resistors for a ladder */
		//m_device->discrete_log("dst_dac_r1_reset - Ladder length too small");
	}

	/*
     * Calculate the total of all resistors in parallel.
     * This is the combined resistance of the voltage sources.
     * This is used for the charging curve.
     */
	for (bit = 0; bit < ladderLength; bit++)
	{
		if (m_info.r[bit] > 0) {
			r_total += 1.0 / m_info.r[bit];
		}
	}

	if (m_info.rBias > 0) {
		r_total += 1.0 / m_info.rBias;
	}

	if (m_info.rGnd > 0)  {
		r_total += 1.0 / m_info.rGnd;
	}
	r_total = 1.0 / r_total;

	m_v_out = 0;

	if (m_info.cFilter > 0)
	{
		m_has_c_filter = 1;
		/* Setup filter constant */
		m_exponent = RC_CHARGE_EXP(r_total * m_info.cFilter);
	}
	else {
		m_has_c_filter = 0;
	}

	/* pre-calculate all possible values to speed up step routine */
	for (int i = 0; i < total_steps; i++)
	{
		double i_total = i_bias;
		for (bit = 0; bit < ladderLength; bit++)
		{
			/* Add up currents of ON circuits per Millman. */

			/* ignore if no resistor present */
			if (m_info.r[bit] > 0)
			{
				double i_bit;
				int bit_val = (i >> bit) & 0x01;

				if (bit_val != 0) {
					i_bit   = v_on / m_info.r[bit];
				}
				else {
					i_bit = 0;
				}
				i_total += i_bit;
			}
		}
		m_v_step[i] = i_total * r_total;
	}
}

} // namespace Emulator
