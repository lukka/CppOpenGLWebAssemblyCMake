#pragma once
#include "DiscreteSoundNode.h"

/************************************************************************
 *
 * DSS_INPUT_x    - Receives input from discrete_sound_w
 *
 * input[0]    - Gain value
 * input[1]    - Offset value
 * input[2]    - Starting Position
 * input[3]    - Current data value
 *
 ************************************************************************/

namespace Emulator
{
	class AudioInput : public DiscreteSoundNode
	{
	public:
		struct InitialData
		{
			InitialData(double gain, double offset, uint8_t data)
			{
				m_gain = gain;
				m_offset = offset;
				m_data = data;
			}

			double m_gain;
			double m_offset;
			uint8_t m_data;
		};

		AudioInput(DiscreteDevice* pDevice, const InitialData& data) :
			DiscreteSoundNode(pDevice),
			_initialData(data)
		{
		}
		virtual ~AudioInput() {}

		void reset();
		void input_write(uint8_t data);

	private:
		InitialData _initialData;
		double m_gain;				/* node gain */
		double m_offset;			/* node offset */
		uint8_t m_data;				/* data written */
	};

} // namespace Emulator
