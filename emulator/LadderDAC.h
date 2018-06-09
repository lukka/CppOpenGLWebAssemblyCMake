#pragma once
#include "DiscreteSoundNode.h"

#define DISC_LADDER_MAXRES					8

namespace Emulator
{

class LadderDAC : public DiscreteSoundNode
{
public:
	struct Info
	{
		int		ladderLength;		// 2 to DISC_LADDER_MAXRES.  1 would be useless.
		double	r[DISC_LADDER_MAXRES];	// Don't use 0 for valid resistors.  That is a short.
		double	vBias;			// Voltage Bias resistor is tied to (0 = not used)
		double	rBias;			// Additional resistor tied to vBias (0 = not used)
		double	rGnd;			// Resistor tied to ground (0 = not used)
		double	cFilter;		// Filtering cap (0 = not used)
	};

	LadderDAC(DiscreteDevice* pDevice, const Info& info) :
		DiscreteSoundNode(pDevice),
		m_info(info)
	{
	}
	virtual ~LadderDAC() {}

	void step(void);
	void reset(void);
	int max_output(void) { return 1; }

private:
	Info			m_info;
	double			m_v_out;
	double			m_exponent;
	double			m_last_v;
	double			m_v_step[256];
	int				m_has_c_filter;
};

} // namespace Emulator
