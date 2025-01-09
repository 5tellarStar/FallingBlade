#include "baseCharacter.hpp"

class CPU
{
public:
	CPU() 
	{

	}
	std::vector<double> inputs()
	{

	}
};

class HardCodedCPU : CPU
{
public:
	HardCodedCPU() 
	{

	}

	enum CPUstate
	{
		Approach,
		Retreat,
		Push
	};
	CPUstate state = Approach;

	std::vector<double> inputs(BaseCharacter Target, BaseCharacter self)
	{
		float distToTarget = fabs(Target.position - self.position);
		
		switch (state)
		{
		case 0:

			break;
		case 1:
			break;
		case 2:
			break;
		}
		return std::vector<double>{0};
	}
};

enum CPUstate
{
	Approach,
	Retreat,
	Push
};