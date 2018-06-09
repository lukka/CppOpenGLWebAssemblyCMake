#pragma once

#include "emulator.h"
#include <vector>
#include <functional>

// M_PI is not part of the C/C++ standards and is not present on
// strict ANSI compilers or when compiling under GCC with -ansi
#ifndef M_PI
#define M_PI    						3.14159265358979323846
#endif

#define DISCRETE_INPUT(_num)	(input(_num))

#define DISCRETE_MAX_NODES					300
#define DISCRETE_MAX_INPUTS					10
#define DISCRETE_MAX_OUTPUTS				8

#define RES_K(res) ((double)(res) * 1e3)
#define CAP_U(cap) ((double)(cap) * 1e-6)
#define RES_2_PARALLEL(r1, r2)			(((r1) * (r2)) / ((r1) + (r2)))

/* calculate charge exponent using discrete sample time */
#define RC_CHARGE_EXP(rc)						(1.0 - exp(-this->sampleTime() / (rc)))

// stream_sample_t is used to represent a single sample in a sound stream
typedef int32_t stream_sample_t;

class DiscreteDevice
{
public:
	DiscreteDevice(uint32_t sampleRate) :
		_sampleRate(sampleRate)
	{}
	~DiscreteDevice() {}

	virtual void updateToCurrentTime() = 0;

	inline double sampleTime() {
		return 1.0 / sampleRate();
	}
	inline uint32_t sampleRate() {
		return _sampleRate;
	}

private:
	uint32_t _sampleRate;
};

class DiscreteSoundNode
{
public:
	DiscreteSoundNode(DiscreteDevice* pDevice);
	~DiscreteSoundNode();

	virtual void reset(void) { }
	virtual void start(void) { }
	virtual void stop(void) { }

	inline double getInput(int n) const {
		return _inputs[n]();
	}
	inline double getOutput() const { return _output; }
	inline void setOutput(double val) { _output = val; }

	void connect(const std::vector<std::function<double()>> inputs);

	inline int			numActiveInputs() { return _activeInputs; }

	inline double sampleTime() const {
		return _device->sampleTime();
	}
	inline int sampleRate() const {
		return _device->sampleRate();
	}

protected:
	DiscreteDevice*				    _device;							/* Points to the parent */
	double							_output;		/* The node's last output value */
	std::vector<std::function<double()>> _inputs;

private:
	int								_activeInputs;					    /* Number of active inputs on this node type */
};
