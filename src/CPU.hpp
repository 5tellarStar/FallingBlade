#include "baseCharacter.hpp"
#include <random>

#pragma once

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
		Defend,
		Push
	};
	CPUstate state = Approach;


	bool forward = false;

	int vertical = 0;

	void Reset()
	{
		forward = false;
		vertical = 0;
	}

	std::vector<double> inputs(BaseCharacter Target, BaseCharacter self)
	{
		float distToTarget = fabs(Target.position - self.position);

		bool advantage = (Target.distToEdge1 < Target.distToEdge2 && self.distToEdge1 > Target.distToEdge1 && self.distToEdge1 < self.distToEdge2) || 
			(Target.distToEdge2 < Target.distToEdge1 && self.distToEdge2 > Target.distToEdge2 && self.distToEdge2 < self.distToEdge1);
		bool disadvantage = (Target.distToEdge1 < Target.distToEdge2 && self.distToEdge1 < Target.distToEdge1 && self.distToEdge1 < self.distToEdge2) || 
			(Target.distToEdge2 < Target.distToEdge1 && self.distToEdge2 < Target.distToEdge2 && self.distToEdge2 < self.distToEdge1);

		std::vector<double> inputs = { 0,0,0,0,0,0,0 };

		switch (state)
		{
		case HardCodedCPU::Approach:
			if (self.isAttacking && self.currentUpperBodyFrame == self.firstActiveAttackFrame - 1)
			{
				state = Push;
			}
			break;
		case HardCodedCPU::Defend:
			if (advantage)
			{
				state = Defend;
			}
			if (distToTarget > 150)
			{
				state = Approach;
				forward = false;
			}
			break;
		case HardCodedCPU::Push:
			if (disadvantage)
			{
				state = Defend;
			}
			if (distToTarget > 150)
			{
				state = Approach;
				forward = false;
			}
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
				if (distToTarget > (int)self.dodges * 30 + self.charge * 15 && !forward)
				{
					inputs[4] = 1;
				}
				else
				{
					vertical = rand() % 3 - 1;
					if (Target.blocking == vertical)
					{
						vertical += rand() % 2 == 1 ? -1 : 1;
						if (vertical < -1)
						{
							vertical = -1;
						}
						if (vertical > 1)
						{
							vertical = 1;
						}
					}
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
		case HardCodedCPU::Defend:
			vertical = Target.attackState;

			break;
		case HardCodedCPU::Push:
			break;
		}

		

		switch (vertical)
		{
		case -1:
			inputs[1] = 1;
 			break;
		case 0:
			break;
		case 1:
			inputs[0] = 1;
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