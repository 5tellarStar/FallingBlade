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


	bool AttackingApproch = false;
	bool dashPast = false;

	int vertical = 0;

	void Reset()
	{
		AttackingApproch = false;
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
			if ((self.isAttacking && self.currentUpperBodyFrame == self.firstActiveAttackFrame - 1) || distToTarget < 50 || (AttackingApproch && ((self.velocity < 0&& Target.position - self.position < 0)||(self.velocity > 0 && Target.position - self.position > 0))))
			{
				state = Push;
			}
			break;
		case HardCodedCPU::Defend:
			if (advantage)
			{
				state = Push;
			}
			if (distToTarget > 150)
			{
				state = Approach;
				AttackingApproch = false;
			}
			break;
		case HardCodedCPU::Push:
			if (disadvantage)
			{
				state = Defend;
				dashPast = false;
			}
			if (distToTarget > 150)
			{
				state = Approach;
				AttackingApproch = false;
			}
			break;
		}

		switch (state)
		{
		case HardCodedCPU::Approach:
			if (!self.isCharging && !AttackingApproch)
			{
				inputs[4] = 1;
			}
			else
			{
				if (distToTarget > ((int)self.dodges - 1) * 20 + self.charge * 15 && !AttackingApproch)
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
					AttackingApproch = true;
					if (self.canDodge && distToTarget > 100 && self.dodges >= 2)
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
			if (self.dodges >= 3 && !dashPast)
			{
				dashPast = true;
			}

			if (dashPast)
			{
				if (self.canDodge && self.dodges >= 2)
				{
					inputs[5] = 1;
				}
				else
				{
					inputs[5] = 0;
				}
				if (Target.position > self.position)
				{
					inputs[2] = 1;
				}
				else
				{
					inputs[3] = 1;
				}
			}
			else
			{
				if (Target.position > self.position)
				{
					if (distToTarget > 50 || self.distToEdge2 < 50)
					{
						inputs[2] = 1;
					}
					else
					{
						inputs[3] = 1;
					}
				}
				else
				{
					if (distToTarget > 50 || self.distToEdge1 < 50)
					{
						inputs[3] = 1;
					}
					else
					{
						inputs[2] = 1;
					}
				}
			}

			break;
		case HardCodedCPU::Push:
			if (self.canAttack && distToTarget < 100 && self.dodges >= 2)
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
				inputs[4] = 1;
			}
			if (self.isAttacking && self.currentUpperBodyFrame == self.firstActiveAttackFrame - 1 && self.dodges >= 2)
			{
				inputs[5] = 1;

				if (Target.position > self.position)
				{
					inputs[2] = 1;
				}
				else
				{
					inputs[3] = 1;
				}
			}
			else if (Target.position > self.position)
			{
				if (distToTarget > 50 || self.distToEdge2 < 50)
				{
					inputs[2] = 1;
				}
				else
				{
					inputs[3] = 1;
				}
			}
			else
			{
				if (distToTarget > 50 || self.distToEdge1 < 50)
				{
					inputs[3] = 1;
				}
				else
				{
					inputs[2] = 1;
				}
			}
			break;
		}

		if (self.position + self.velocity > 420 && self.canDodge)
		{
			inputs[5] = 1;

			inputs[2] = 0;
			inputs[3] = 1;
		}
		if (self.position + self.velocity < 92 && self.canDodge)
		{
			inputs[5] = 1;

			inputs[2] = 1;
			inputs[3] = 0;
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