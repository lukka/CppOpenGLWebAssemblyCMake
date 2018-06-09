#include "Timer.h"

namespace Emulator
{
	CTimer::~CTimer()
	{
		_scheduler.UnregisterTimer(this);
	}

	void CTimer::OnTick(long currentTime)
	{
		if (Enabled && ExpirationTime <= currentTime)
		{
			if (IsPeriodic)
			{
				_scheduler.UnregisterTimer(this);
				while (ExpirationTime <= currentTime) {
					ExpirationTime = ExpirationTime + Period;
				}
				_scheduler.RegisterTimer(this);
			}
			else
			{
				Enabled = false;
			}

			Callback(Param, currentTime);
		}
	}

	void Scheduler::RegisterTimer(CTimer* timer)
	{
		bool stopTimeslice = timer->ExpirationTime < GetTimeNextTimer();
		timers.push_back(timer);

		if (stopTimeslice) {
			if (executingDevice != nullptr) {
				executingDevice->stopTimeslice();
			}
		}
	}

	void Scheduler::ExecuteTimers()
	{
		for (size_t i = 0; i < timers.size(); i++)
		{
			timers[i]->OnTick(CurrentTime);
		}
	}

	int64_t Scheduler::GetTimeNextTimer()
	{
		int64_t time = INT64_MAX;
		for (size_t i = 0; i < timers.size(); i++)
		{
			if (timers[i]->Enabled && timers[i]->ExpirationTime < time) {
				time = timers[i]->ExpirationTime;
			}
		}
		return (time == INT64_MAX ? 0 : time);
	}

} // Emulator
