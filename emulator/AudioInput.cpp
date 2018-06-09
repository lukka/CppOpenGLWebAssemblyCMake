#include "AudioInput.h"

namespace Emulator
{
	void AudioInput::reset()
	{
		m_gain = _initialData.m_gain;
		m_offset = _initialData.m_offset;
		m_data = _initialData.m_data;
		setOutput(m_data * m_gain + m_offset);
	}

	void AudioInput::input_write(uint8_t data)
	{
		uint8_t new_data    = 0;

		new_data = data;

		if (m_data != new_data)
		{
			/* Bring the system up to now */
			_device->updateToCurrentTime();

			m_data = new_data;

			/* Update the node output here so we don't have to do it each step */
			setOutput(m_data * m_gain + m_offset);
		}
	}

} // namespace Emulator