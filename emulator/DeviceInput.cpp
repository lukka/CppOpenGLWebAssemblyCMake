#include "DeviceInput.h"
#include "emulator.h"
#include "Cpu.h"
#include "Timer.h"
#include <exception>
#include <algorithm>

namespace Emulator
{
	_DeviceInput::_DeviceInput(Cpu& cpu, StatusChangedCallback callback, uint32_t storedVector) :
        cpu(cpu),
		curstate(LineState::Clear),
		curvector(DefaultIrqVector),
		callback(callback),
		storedVector(storedVector),
		eventQueue(),
		emptyEventQueueTimer(nullptr)
    {
	}
	
	_DeviceInput::~_DeviceInput()
	{
		delete emptyEventQueueTimer;
	}

    // enqueue an event for later execution via timer
    void _DeviceInput::PostState(LineState state, uint32_t vector)
    {
        assert (state != LineState::Pulse);

		// if we're full of events, flush the queue
		if (eventQueue.size() >= MaxQueueLength) {
			EmptyEventQueue(/*0*/);
		}

		// enqueue the event
		if (vector == USE_STORED_VECTOR) {
			vector = storedVector;
		}
		eventQueue.push_back(Event(state, vector));

		// if this is the first one, set the timer
		if (1 == eventQueue.size())
		{
			//cpu.get_Scheduler().Synchronize(
			//	[this](int64_t param) { this->EmptyEventQueue(param); 
			if (nullptr == emptyEventQueueTimer)
			{
				emptyEventQueueTimer = new CTimer("EmptyEventQueue", cpu.get_Scheduler(),
					[this](int64_t, int64_t) { this->EmptyEventQueue(); }
				);
			}
			emptyEventQueueTimer->Start(0, false);
		}
	}

    void _DeviceInput::EmptyEventQueue(/*int64_t param*/)
    {
        // loop over all events
        std::for_each(std::begin(eventQueue), std::end(eventQueue), [this](Event inputEvent) {
			this->HandleEvent(inputEvent);
        });
		eventQueue.clear();
    }

	void _DeviceInput::onStateChanged(LineState state)
	{
		if (callback) {
	        ((*this).callback)(state);
		}
	}

	uint32_t _DeviceInput::DefaultIrqCallback()
	{
		uint32_t vector = curvector;

		// if the IRQ state is HOLD_LINE, clear it
		if (curstate == LineState::Hold) {
			SetState(LineState::Clear);
		}
		return vector;
	}


    IrqDeviceInputLine::IrqDeviceInputLine(Cpu& cpu, StatusChangedCallback callback, uint32_t storedVector) :
		_DeviceInput(cpu, callback, storedVector)
	{
	}

    void IrqDeviceInputLine::SetState(LineState state, uint32_t vector)
    {
        if (state == LineState::Pulse)
        {
            // catch errors where people use PULSE_LINE for devices that don't support it
//            throw new std::exception("LineState.Pulse can only be used for NMI and RESET lines\n");
			return;
        }

		PostState(state, vector);
	}

	void IrqDeviceInputLine::HandleEvent(_DeviceInput::Event inputEvent)
	{
        curstate = inputEvent.state;
        curvector = inputEvent.vector;

        switch (curstate)
        {
            case LineState::Hold:
            case LineState::Assert:
                onStateChanged(LineState::Assert);
                break;

            case LineState::Clear:
                onStateChanged(LineState::Clear);
                break;

            default:
                break;
        }

        // generate a trigger to unsuspend any devices waiting on the interrupt
        if (curstate != LineState::Clear)
        {
            cpu.SignalInterruptTrigger();
        }
	}

	uint32_t IrqDeviceInputLine::OnIrq()
	{
		// if the IRQ state is HOLD_LINE, clear it
        if (curstate == LineState::Hold)
		{
			curstate = LineState::Clear;
			onStateChanged(curstate);
        }

        return curvector;
	}

    NmiDeviceInputLine::NmiDeviceInputLine(Cpu& cpu, StatusChangedCallback callback, uint32_t storedVector) :
		_DeviceInput(cpu, callback, storedVector)
	{
	}

    void NmiDeviceInputLine::SetState(LineState state, uint32_t vector)
    {
        // treat PULSE_LINE as assert+CLEAR
        if (state == LineState::Pulse)
        {
            PostState(LineState::Assert, vector);
            PostState(LineState::Clear, vector);
        }
		else {
			PostState(state, vector);
		}
	}

	void NmiDeviceInputLine::HandleEvent(_DeviceInput::Event inputEvent)
	{
        curstate = inputEvent.state;
        curvector = inputEvent.vector;

        switch (curstate)
        {
            case LineState::Hold:
            case LineState::Assert:
                onStateChanged(LineState::Assert);
                break;

            case LineState::Clear:
                onStateChanged(LineState::Clear);
                break;

            default:
                break;
        }

        // generate a trigger to unsuspend any devices waiting on the interrupt
        if (curstate != LineState::Clear)
        {
            cpu.SignalInterruptTrigger();
        }
	}

    ResetDeviceInputLine::ResetDeviceInputLine(Cpu& cpu, StatusChangedCallback callback, uint32_t storedVector) :
		_DeviceInput(cpu, callback, storedVector)
	{
	}

    void ResetDeviceInputLine::SetState(LineState state, uint32_t vector)
    {
        // treat PULSE_LINE as assert+CLEAR
        if (state == LineState::Pulse)
        {
            PostState(LineState::Assert, vector);
            PostState(LineState::Clear, vector);
        }
		else {
			PostState(state, vector);
		}
	}

	void ResetDeviceInputLine::HandleEvent(_DeviceInput::Event inputEvent)
	{
        curstate = inputEvent.state;
        curvector = inputEvent.vector;

        // if we're asserting the line, just halt the device
        if (curstate == LineState::Assert)
        {
            cpu.Suspend((uint32_t)SuspendReason::SUSPEND_REASON_RESET);
        }
        //  if we're clearing the line that was previously asserted, reset the device
        else if (cpu.IsSuspended((uint32_t)SuspendReason::SUSPEND_REASON_RESET))
        {
            cpu.Reset();
            cpu.Resume((uint32_t)SuspendReason::SUSPEND_REASON_RESET);
        }
	}

    HaltDeviceInputLine::HaltDeviceInputLine(Cpu& cpu, StatusChangedCallback callback, uint32_t storedVector) :
		_DeviceInput(cpu, callback, storedVector)
	{
	}

    void HaltDeviceInputLine::SetState(LineState state, uint32_t vector)
    {
        if (state == LineState::Pulse)
        {
            // catch errors where people use PULSE_LINE for devices that don't support it
//            throw new std::exception("LineState.Pulse can only be used for NMI and RESET lines\n");
			return;
        }

		PostState(state, vector);
	}

	void HaltDeviceInputLine::HandleEvent(_DeviceInput::Event inputEvent)
	{
        curstate = inputEvent.state;
        curvector = inputEvent.vector;

        // if asserting, halt the device
        if (curstate == LineState::Assert)
        {
            cpu.Suspend((uint32_t)SuspendReason::SUSPEND_REASON_HALT);
        }
        // if clearing, unhalt the device
        else if (curstate == LineState::Clear)
        {
            cpu.Resume((uint32_t)SuspendReason::SUSPEND_REASON_HALT);
        }
	}

} // namespace Emulator
