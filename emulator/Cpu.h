#pragma once

#include "emulator.h"
#include "DeviceInput.h"

namespace Emulator
{
	class Scheduler;
	class DeviceInput;

	class Cpu : public IMemoryHandler, public IDevice
	{
	public:
		Cpu(Scheduler& scheduler, int machineIndex, IProcessor* processor);
		virtual ~Cpu();

		// IDevice
        virtual void Reset() {
            processor->Reset();
        }
		virtual int64_t get_ExecutionTimeInCurrentTimeslice() const {
			return processor->get_ExecutionTimeInCurrentTimeslice();
		}
		virtual int64_t Execute(int64_t untilTime) // us
		{
			//assert(NULL != processor);

			if (untilTime > currentTime) {
				currentTime += processor->Execute(untilTime - currentTime);
			}
			else {
				currentTime = untilTime;
			}

			return currentTime;
		}
		virtual void stopTimeslice();

		bool get_IsInterruptEnabled() const {
			return isInterruptEnabled;
		}
		void set_IsInterruptEnabled(bool value);

        IProcessor& get_Processor() {
			return *processor;
		}

		Scheduler& get_Scheduler() {
			return scheduler;
		}

        bool IsSuspended(uint32_t reason = SuspendReason::SUSPEND_ANY_REASON) {
            return ((nextSuspend & reason) != 0);
        }

        void Suspend(uint32_t reason)
        {
            nextSuspend |= reason;
            processor->AbortTimeslice();
        }

        void Resume(uint32_t reason)
        {
            nextSuspend &= ~reason;
            processor->AbortTimeslice();
        }

        void SignalInterruptTrigger()
        {
            processor->AbortTimeslice();

            // see if this is a matching trigger
            if ((nextSuspend & (uint32_t)SuspendReason::SUSPEND_REASON_TRIGGER) != 0 && trigger == interruptTrigger)
            {
                Resume((uint32_t)SuspendReason::SUSPEND_REASON_TRIGGER);
                trigger = 0;
            }
        }

		void SetProgramCounterListener(ProgramCounterListener &pcListener) {
			processor->SetProgramCounterListener(pcListener);
		}
	private:
		void EmptyEventQueues();
		void ClearAllLines();

	protected:
        IProcessor* processor;

	private:
		Scheduler& scheduler;
        uint32_t trigger;
        int interruptTrigger;
		bool isInterruptEnabled;
		std::vector<IDeviceInputLine*> inputLines;
		uint32_t nextSuspend;
		CTimer* clearAllLinesTimer;
		int64_t currentTime;
	};

} // Emulator
