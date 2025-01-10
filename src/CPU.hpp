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

	bool forward = false;

	std::vector<double> inputs(BaseCharacter Target, BaseCharacter self)
	{
		float distToTarget = fabs(Target.position - self.position);
		
		bool advantage = (Target.distToEdge1 < Target.distToEdge2 && self.distToEdge1 > Target.distToEdge1 && self.distToEdge1 < self.distToEdge2) || (Target.distToEdge2 < Target.distToEdge1 && self.distToEdge2 > Target.distToEdge2 && self.distToEdge2 < self.distToEdge1);

		std::vector<double> inputs = { 0,0,0,0,0,0,0 };

		switch (state)
		{
		case HardCodedCPU::Approach:
			
			break;
		case HardCodedCPU::Retreat:
			break;
		case HardCodedCPU::Push:
			break;
		}

		switch (state)
		{
		case HardCodedCPU::Approach:
			if (!self.isCharging && !forward)
			{
				inputs[4] = 1;
			}
			else
			{
				if (distToTarget > (int)self.dodges * 40 + self.charge * 9 && !forward)
				{
					inputs[4] = 1;
				}
				else
				{
					forward = true;
					if (self.canDodge)
					{
						inputs[5] = 1;
					}
					else
					{
						inputs[5] = 0;
					}
				}
			}

			if (Target.position > self.position)
			{
				inputs[2] = 1;
			}
			else
			{
				inputs[3] = 1;
			}
			break;
		case HardCodedCPU::Retreat:
			break;
		case HardCodedCPU::Push:
			break;
		}


		return inputs;
	}
};

enum CPUstate
{
	Approach,
	Retreat,
	Push
};