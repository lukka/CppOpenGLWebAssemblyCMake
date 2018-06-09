#include "DiscreteSoundNode.h"
#include <exception>
#include <cassert>

DiscreteSoundNode::DiscreteSoundNode(DiscreteDevice* pDevice) :
	_device(pDevice),
	_output(.0),
	_inputs(),
	_activeInputs(0) // TODO
{
}

DiscreteSoundNode::~DiscreteSoundNode()
{
}

void DiscreteSoundNode::connect(const std::vector<std::function<double()>> inputs)
{
	for (auto& input : inputs)
	{
		_inputs.push_back(input);
	}
	_activeInputs = _inputs.size();
}
