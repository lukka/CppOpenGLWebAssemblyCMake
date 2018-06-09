#pragma once

#include <string>
#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include <functional>
#include <cassert>
#include "emulator.h"

namespace Emulator
{
	class CTimer;
	typedef std::function<void(int64_t, int64_t)> TimerCallback;

    class Scheduler
    {
	public:
        Scheduler(long quantum) //us
        {
            Quantum = quantum;
            CurrentTime = 0;
			executingDevice = nullptr;
        }

        void Reset()
        {
            this->CurrentTime = 0L;
        }

		void RegisterTimer(CTimer* timer);

        void UnregisterTimer(CTimer* timer)
        {
            for (auto it = timers.begin(); it != timers.end(); it++)
			{
				if (*it == timer) {
					timers.erase(it);
					return;
				}
            }
        }

        void RegisterDevice(Emulator::IDevice* device)
        {
            devices.push_back(device);
        }

        void UnregisterDevice(Emulator::IDevice* device)
        {
			for (std::vector<Emulator::IDevice*>::iterator it = devices.begin();
				 it != devices.end(); it++)
			{
				if (*it == device) {
					devices.erase(it);
					break;
				}
			}
        }

        void Timeslice()
        {
            ExecuteTimers();

			int64_t untilTime = CurrentTime + Quantum;
			for (size_t i = 0; i < devices.size(); i++)
			{
				Emulator::IDevice* device = devices[i];

				int64_t timeNextTimer = GetTimeNextTimer();
				if (timeNextTimer > 0 && timeNextTimer < untilTime)
				{
					untilTime = timeNextTimer;
				}
			
				if (untilTime > CurrentTime)
				{
					executingDevice = device;
					int64_t t = device->Execute(untilTime);

					if (t < untilTime) {
						untilTime = t;
					}

					executingDevice = nullptr;
				}
			}
				
			CurrentTime = (long)untilTime;
        }

		long get_CurrentTime() const
		{
			// if we're active, add in the time from the current slice
			long result = CurrentTime;
			if (executingDevice != nullptr) {
				result = result + (long)executingDevice->get_ExecutionTimeInCurrentTimeslice();
			}
			return result;
		}

	private:
		void ExecuteTimers();
		int64_t GetTimeNextTimer();

		long CurrentTime; // us
		long Quantum; // us
		std::vector<CTimer*> timers; // weak refs

        std::vector<IDevice*> devices; // weak refs
		IDevice* executingDevice; // weak ref
    };

	class CTimer
	{
	public:
		CTimer(const std::string& tag, Emulator::Scheduler& scheduler, TimerCallback callback) :
			ExpirationTime(0),
			Enabled(false),
			tag(tag),
			_scheduler(scheduler),
			Callback(callback),
			Period(0),
			IsPeriodic(false)
		{
			scheduler.RegisterTimer(this);
		}
		~CTimer();

		void Start(int64_t period, bool isPeriodic, long param = 0)
		{
			assert(period >= 0);

			Param = param;
			Period = period;
			IsPeriodic = isPeriodic;
			Enabled = true;
			_scheduler.UnregisterTimer(this);
			ExpirationTime = _scheduler.get_CurrentTime() + Period;
			_scheduler.RegisterTimer(this);
		}

		void Stop()
		{
			Enabled = false;
		}

		void OnTick(long currentTime);

	public:
		int64_t ExpirationTime;
		bool Enabled;
	private:
		std::string tag;
		Scheduler& _scheduler;
		TimerCallback Callback;
		int64_t Period; // us
		bool IsPeriodic;
		long Param;
	};

} // Emulator
