#pragma once

#include "emulator.h"
#include <vector>

namespace Emulator
{
	class Cpu;
	class CTimer;

	typedef std::function<uint16_t()> IrqCallback;

	class IDeviceInputLine
	{
	public:
		virtual void EmptyEventQueue(/*int64_t param*/) = 0;
		virtual LineState get_State() const = 0;
		virtual void SetState(LineState state, uint32_t vector = USE_STORED_VECTOR) = 0;
		virtual uint32_t DefaultIrqCallback() = 0;

        static const uint32_t USE_STORED_VECTOR = 0xff000000;
	};

    class _DeviceInput : public IDeviceInputLine
    {
        static const size_t MaxQueueLength = 32;
        static const int DefaultIrqVector = 0x4001;

	protected:
		struct Event {
			Event() : state(LineState::Clear), vector(DefaultIrqVector) {}
			Event(LineState state, uint32_t vector) : state(state), vector(vector) {}

			LineState state;
			uint32_t vector;
		};

	public:
		typedef std::function<void(LineState state)> StatusChangedCallback;

        _DeviceInput(Cpu& cpu, StatusChangedCallback callback, uint32_t storedVector);
		virtual ~_DeviceInput();

		// IDeviceInputLine
		virtual void EmptyEventQueue(/*int64_t param*/);
		virtual LineState get_State() const {
			return curstate;
		}
		virtual uint32_t DefaultIrqCallback();

		virtual void HandleEvent(Event inputEvent) = 0;

		void onStateChanged(LineState state);

	protected:
		void PostState(LineState state, uint32_t vector);

		Cpu& cpu;
        LineState curstate;	// most recently processed state
		uint32_t curvector;		// most recently processed vector

	private:
		StatusChangedCallback callback;
		uint32_t storedVector;	// most recently written vector
		std::vector<Event> eventQueue; // queue of pending events
		CTimer* emptyEventQueueTimer;
	};

	class IrqDeviceInputLine : public _DeviceInput
	{
	public:
        IrqDeviceInputLine(Cpu& cpu, StatusChangedCallback callback, uint32_t storedVector);
		virtual void SetState(LineState state, uint32_t vector = USE_STORED_VECTOR);
		virtual void HandleEvent(_DeviceInput::Event inputEvent);
		uint32_t OnIrq();
	};

	class NmiDeviceInputLine : public _DeviceInput
	{
	public:
        NmiDeviceInputLine(Cpu& cpu, StatusChangedCallback callback, uint32_t storedVector);
		virtual void SetState(LineState state, uint32_t vector = USE_STORED_VECTOR);
		virtual void HandleEvent(_DeviceInput::Event inputEvent);
	};

	class HaltDeviceInputLine : public _DeviceInput
	{
	public:
        HaltDeviceInputLine(Cpu& cpu, StatusChangedCallback callback, uint32_t storedVector);
		virtual void SetState(LineState state, uint32_t vector = USE_STORED_VECTOR);
		virtual void HandleEvent(_DeviceInput::Event inputEvent);
	};

	class ResetDeviceInputLine : public _DeviceInput
	{
	public:
        ResetDeviceInputLine(Cpu& cpu, StatusChangedCallback callback, uint32_t storedVector);
		virtual void SetState(LineState state, uint32_t vector = USE_STORED_VECTOR);
		virtual void HandleEvent(_DeviceInput::Event inputEvent);
	};

	class _NullDeviceInput : public IDeviceInputLine
    {
	public:
		_NullDeviceInput() {};
		virtual ~_NullDeviceInput() {};

		// IDeviceInputLine
		virtual void EmptyEventQueue(/*int64_t param*/) {};
		virtual void SetState(LineState state, uint32_t vector) {};		
		virtual LineState get_State() const { return Clear; }
		virtual uint32_t DefaultIrqCallback() { return 0; };
	};

} // namespace Emulator
