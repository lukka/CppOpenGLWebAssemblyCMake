#include <math.h>
#include "Mixer.h"

namespace Emulator
{

	void Mixer::step()
	{
		double	v, vTemp, r_total, rTemp, rTemp2 = 0;
		double	i = 0;		/* total current of inputs */
		int		bit, connected;

		/* put commonly used stuff in local variables for speed */
		int		r_node_bit_flag = m_r_node_bit_flag;
		int		c_bit_flag = m_c_bit_flag;
		int		bit_mask = 1;
		int		has_rF = (m_desc.rF != 0);
		int		type = m_type;
		double	v_ref = m_desc.vRef;
		double	rI = m_desc.rI;

		if (getInput(0)) // enabled
		{
			r_total = m_r_total;

			if (m_r_node_bit_flag != 0)
			{
				/* loop and do any high pass filtering for connected caps */
				/* but first see if there is an r_node for the current path */
				/* if so, then the exponents need to be re-calculated */
				for (bit = 0; bit < m_size; bit++)
				{
					rTemp     = m_desc.r[bit];
					connected = 1;
					vTemp     = getInput(bit + 1);

					/* is there a resistor? */
					if (r_node_bit_flag & bit_mask)
					{
						/* a node has the possibility of being disconnected from the circuit. */
						if (*m_r_node[bit] == 0) {
							connected = 0;
						}
						else
						{
							/* value currently holds resistance */
							rTemp   += *m_r_node[bit];
							r_total += 1.0 / rTemp;
					
							/* is there a capacitor? */
							if (c_bit_flag & bit_mask)
							{
								switch (type)
								{
									case DISC_MIXER_IS_RESISTOR:
										/* is there an rF? */
										if (has_rF)
										{
											rTemp2 = RES_2_PARALLEL(rTemp, m_desc.rF);
											break;
										}
										/* else, fall through and just use the resistor value */
									case DISC_MIXER_IS_OP_AMP:
										rTemp2 = rTemp;
										break;
									case DISC_MIXER_IS_OP_AMP_WITH_RI:
										rTemp2 = rTemp + rI;
										break;
								}
						
								/* Re-calculate exponent if resistor is a node and has changed value */
								if (*m_r_node[bit] != m_r_last[bit])
								{
									m_exponent_rc[bit] =  RC_CHARGE_EXP(rTemp2 * m_desc.c[bit]);
									m_r_last[bit] = *m_r_node[bit];
								}
							}
						}
					}

					if (connected)
					{
						/* is there a capacitor? */
						if (c_bit_flag & bit_mask)
						{
							/* do input high pass filtering if needed. */
							m_v_cap[bit] += (vTemp - v_ref - m_v_cap[bit]) * m_exponent_rc[bit];
							vTemp -= m_v_cap[bit];
						}
						i += ((type == DISC_MIXER_IS_OP_AMP) ? v_ref - vTemp : vTemp) / rTemp;
					}
					bit_mask = bit_mask << 1;
				}
			}
			else if (c_bit_flag != 0)
			{
				/* no r_nodes, so just do high pass filtering */
				for (bit = 0; bit < m_size; bit++)
				{
					vTemp = getInput(bit + 1);

					if (c_bit_flag & (1 << bit))
					{
						/* do input high pass filtering if needed. */
						m_v_cap[bit] += (vTemp - v_ref - m_v_cap[bit]) * m_exponent_rc[bit];
						vTemp -= m_v_cap[bit];
					}
					i += ((type == DISC_MIXER_IS_OP_AMP) ? v_ref - vTemp : vTemp) / m_desc.r[bit];
				}
			}
			else
			{
				/* no r_nodes or c_nodes, mixing only */
				if (type == DISC_MIXER_IS_OP_AMP)
				{
					for (bit = 0; bit < m_size; bit++) {
						i += ( v_ref - getInput(bit + 1)) / m_desc.r[bit];
					}
				}
				else
				{
					for (bit = 0; bit < m_size; bit++) {
						i += getInput(bit + 1) / m_desc.r[bit];
					}
				}
			}

			if (type == DISC_MIXER_IS_OP_AMP_WITH_RI) {
				i += v_ref / rI;
			}

			r_total = 1.0 / r_total;

			/* If resistor network or has rI then Millman is used.
			 * If op-amp then summing formula is used. */
			v = i * ((type == DISC_MIXER_IS_OP_AMP) ? m_desc.rF : r_total);

			if (type == DISC_MIXER_IS_OP_AMP_WITH_RI) {
				v = v_ref + (m_gain * (v_ref - v));
			}

			/* Do the low pass filtering for cF */
			if (m_desc.cF != 0)
			{
				if (r_node_bit_flag != 0)
				{
					/* Re-calculate exponent if resistor nodes are used */
					m_exponent_c_f =  RC_CHARGE_EXP(r_total * m_desc.cF);
				}
				m_v_cap_f += (v - v_ref - m_v_cap_f) * m_exponent_c_f;
				v = m_v_cap_f;
			}

			/* Do the high pass filtering for cAmp */
			if (m_desc.cAmp != 0)
			{
				m_v_cap_amp += (v - m_v_cap_amp) * m_exponent_c_amp;
				v -= m_v_cap_amp;
			}
			setOutput(v * m_desc.gain);
		}
		else {
			setOutput(0);
		}
	}

	void Mixer::reset()
	{
		int	bit;
		double rTemp = 0;

		/* link to r_node outputs */
		m_r_node_bit_flag = 0;
		for (bit = 0; bit < 8; bit++)
		{
			//m_r_node[bit] = _device->getNodeOutputPtr(m_desc.r_node[bit]);
			//if (m_r_node[bit] != NULL) {
			//	m_r_node_bit_flag |= 1 << bit;
			//}

			/* flag any caps */
			if (m_desc.c[bit] != 0) {
				m_c_bit_flag |= 1 << bit;
			}
		}

		m_size = numActiveInputs() - 1;

		/*
		 * THERE IS NO ERROR CHECKING!!!!!!!!!
		 * If you pass a bad ladder table
		 * then you deserve a crash.
		 */

		m_type = m_desc.type;
		if ((m_desc.type == DISC_MIXER_IS_OP_AMP) && (m_desc.rI != 0)) {
			m_type = DISC_MIXER_IS_OP_AMP_WITH_RI;
		}

		/*
		 * Calculate the total of all resistors in parallel.
		 * This is the combined resistance of the voltage sources.
		 * Also calculate the exponents while we are here.
		 */
		m_r_total = 0;
		for (bit = 0; bit < m_size; bit++)
		{
			if ((m_desc.r[bit] != 0) && !m_desc.r_node[bit])
			{
				m_r_total += 1.0 / m_desc.r[bit];
			}

			m_v_cap[bit] = 0;
			m_exponent_rc[bit] = 0;
			if ((m_desc.c[bit] != 0)  && !m_desc.r_node[bit])
			{
				switch (m_type)
				{
					case DISC_MIXER_IS_RESISTOR:
						/* is there an rF? */
						if (m_desc.rF != 0)
						{
							rTemp = 1.0 / ((1.0 / m_desc.r[bit]) + (1.0 / m_desc.rF));
							break;
						}
						/* else, fall through and just use the resistor value */
					case DISC_MIXER_IS_OP_AMP:
						rTemp = m_desc.r[bit];
						break;
					case DISC_MIXER_IS_OP_AMP_WITH_RI:
						rTemp = m_desc.r[bit] + m_desc.rI;
						break;
				}
				/* Setup filter constants */
				m_exponent_rc[bit] = RC_CHARGE_EXP(rTemp * m_desc.c[bit]);
			}
		}

		if (m_desc.rF != 0) {
			if (m_type == DISC_MIXER_IS_RESISTOR) m_r_total += 1.0 / m_desc.rF;
		}

		if (m_type == DISC_MIXER_IS_OP_AMP_WITH_RI) {
			m_r_total += 1.0 / m_desc.rI;
		}

		m_v_cap_f      = 0;
		m_exponent_c_f = 0;
		if (m_desc.cF != 0)
		{
			/* Setup filter constants */
			m_exponent_c_f = RC_CHARGE_EXP(((m_desc.type == DISC_MIXER_IS_OP_AMP) ? m_desc.rF : (1.0 / m_r_total)) * m_desc.cF);
		}

		m_v_cap_amp      = 0;
		m_exponent_c_amp = 0;
		if (m_desc.cAmp != 0)
		{
			/* Setup filter constants */
			/* We will use 100k ohms as an average final stage impedance. */
			/* Your amp/speaker system will have more effect on incorrect filtering then any value used here. */
			m_exponent_c_amp = RC_CHARGE_EXP(RES_K(100) * m_desc.cAmp);
		}

		if (m_type == DISC_MIXER_IS_OP_AMP_WITH_RI) {
			m_gain = m_desc.rF / m_desc.rI;
		}

		setOutput(0);
	}
} // namespace Emulator