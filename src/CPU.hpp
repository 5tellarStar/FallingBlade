#include "baseCharacter.hpp"
#include <random>

#pragma once

class CPU
{
public:
	CPU() 
	{
	}
	virtual void Reset()
	{
	}
	virtual std::vector<double> inputs(BaseCharacter Target, BaseCharacter self, int frame)
	{
		std::vector<double> out = std::vector<double>{ 0, 0, 0, 0, 0, 0, 0 };

		switch (self.inputVertical)
		{
		case -1:
			out[1] = 1;
			break;
		case 0:
			break;
		case 1:
			out[0] = 1;
			break;
		}
		switch (self.inputHorizontal)
		{
		case -1:
			out[3] = 1;
			break;
		case 0:
			break;
		case 1:
			out[2] = 1;
			break;
		}

		if (self.inputAttack)
		{
			out[4] = 1;
		}
		if (self.inputDodge)
		{
			out[5] = 1;
		}
		return out;
	}
};

class HardCodedCPUImpossiable : public CPU
{
public:
	HardCodedCPUImpossiable() 
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

	void Reset() override
	{
		AttackingApproch = false;
		vertical = 0;
	}

	std::vector<double> inputs(BaseCharacter Target, BaseCharacter self, int frame) override
	{
		float distToTarget = fabs(Target.position - self.position);

		bool advantage = (Target.distToEdge1 < Target.distToEdge2 && self.distToEdge1 > Target.distToEdge1 && self.distToEdge1 < self.distToEdge2) || 
			(Target.distToEdge2 < Target.distToEdge1 && self.distToEdge2 > Target.distToEdge2 && self.distToEdge2 < self.distToEdge1);
		bool disadvantage = (Target.distToEdge1 < Target.distToEdge2 && self.distToEdge1 < Target.distToEdge1 && self.distToEdge1 < self.distToEdge2) || 
			(Target.distToEdge2 < Target.distToEdge1 && self.distToEdge2 < Target.distToEdge2 && self.distToEdge2 < self.distToEdge1);

		std::vector<double> inputs = { 0,0,0,0,0,0,0 };

		switch (state)
		{
		case HardCodedCPUImpossiable::Approach:
			if ((self.isAttacking && self.currentUpperBodyFrame == self.firstActiveAttackFrame - 1) || distToTarget < 50 || (AttackingApproch && ((self.velocity < 0&& Target.position - self.position < 0)||(self.velocity > 0 && Target.position - self.position > 0))))
			{
				state = Push;
			}
			break;
		case HardCodedCPUImpossiable::Defend:
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
		case HardCodedCPUImpossiable::Push:
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
		case HardCodedCPUImpossiable::Approach:
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
		case HardCodedCPUImpossiable::Defend:
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
		case HardCodedCPUImpossiable::Push:
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

class HardCodedCPUVariable : public CPU
{
public:
	HardCodedCPUVariable(int diff)
	{
		difficulty = diff;
	}

	enum CPUstate
	{
		Approach,
		Defend,
		Push
	};
	CPUstate state = Push;

	int difficulty;
	bool AttackingApproch = false;
	bool dashPast = false;

	int vertical = 0;

	void Reset() override
	{
		AttackingApproch = false;
		vertical = 0;
	}

	std::vector<double> inputs(BaseCharacter Target, BaseCharacter self, int frame) override
	{
		float distToTarget = fabs(Target.position - self.position);

		bool advantage = (Target.distToEdge1 < Target.distToEdge2 && self.distToEdge1 > Target.distToEdge1 && self.distToEdge1 < self.distToEdge2) ||
			(Target.distToEdge2 < Target.distToEdge1 && self.distToEdge2 > Target.distToEdge2 && self.distToEdge2 < self.distToEdge1);
		bool disadvantage = (Target.distToEdge1 < Target.distToEdge2 && self.distToEdge1 < Target.distToEdge1 && self.distToEdge1 < self.distToEdge2) ||
			(Target.distToEdge2 < Target.distToEdge1 && self.distToEdge2 < Target.distToEdge2 && self.distToEdge2 < self.distToEdge1);

		std::vector<double> inputs = { 0,0,0,0,0,0,0 };

		switch (state)
		{
		case HardCodedCPUVariable::Approach:
			if ((self.isAttacking && self.currentUpperBodyFrame == self.firstActiveAttackFrame - 1) || distToTarget < 50 || (AttackingApproch && ((self.velocity < 0 && Target.position - self.position < 0) || (self.velocity > 0 && Target.position - self.position > 0))))
			{
				state = Push;
			}
			break;
		case HardCodedCPUVariable::Defend:
			if (advantage && rand() % difficulty == 0)
			{
				state = Push;
			}
			if (distToTarget > 150 && rand() % difficulty == 0)
			{
				state = Approach;
				AttackingApproch = false;
			}
			break;
		case HardCodedCPUVariable::Push:
			if (disadvantage && rand() % difficulty == 0)
			{
				state = Defend;
				dashPast = false;
			}
			if (distToTarget > 150 && rand() % difficulty == 0)
			{
				state = Approach;
				AttackingApproch = false;
			}
			break;
		}

		switch (state)
		{
		case HardCodedCPUVariable::Approach:
			if (!self.isCharging && !AttackingApproch)
			{
				inputs[4] = 1;
			}
			else
			{
				if (distToTarget > ((int)self.dodges) * 20 + self.charge * 15 && !AttackingApproch)
				{
					inputs[4] = 1;
				}
				else
				{
					vertical = rand() % 3 - 1;
					if (Target.blocking == vertical && rand() % difficulty == 0)
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
					if (self.canDodge && distToTarget > 100)
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
		case HardCodedCPUVariable::Defend:
			if (Target.isAttacking || Target.isCharging)
			{
				if (rand() % difficulty == 0)
				{
					if (Target.isCharging)
					{
						vertical = Target.inputVertical;
					}
					else
					{
						vertical = Target.attackState;
					}
				}
			}
			else if(rand() % difficulty == 0)
			{
				inputs[4] = 0;
			}
			if (self.dodges >= 2 && !dashPast && rand() % difficulty == 0)
			{
				dashPast = true;
			}

			if (dashPast)
			{
				if (self.canDodge)
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
		case HardCodedCPUVariable::Push:
			if (self.canAttack && distToTarget < 100 && rand() % difficulty == 0)
			{
				vertical = rand() % 3 - 1;
				if (Target.blocking == vertical && rand() % difficulty == 0)
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
			if (self.isAttacking && self.currentUpperBodyFrame == self.firstActiveAttackFrame - 1)
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

		if (self.position + self.velocity > 420 && self.canDodge && rand() % difficulty == 0)
		{
			inputs[5] = 1;

			inputs[2] = 0;
			inputs[3] = 1;
		}
		if (self.position + self.velocity < 92 && self.canDodge && rand() % difficulty == 0)
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

class NeuralNetworkCPU : public CPU
{
public:

	NeuralNetworkCPU(NeuralNetwork ai)
	{
		AI = ai;
	}
	NeuralNetwork AI = NeuralNetwork(std::vector<int>{0,0,0});
	std::vector<double> inputs(BaseCharacter Target, BaseCharacter self, int frame) override
	{
		return AI.CalcOutputs(std::vector<double>{
			Target.distToEdge2,
			Target.distToEdge1,
			self.distToEdge2,
			self.distToEdge1,
			self.position - Target.position,
			Target.dodges,
			self.dodges,
			(double)Target.blocking,
			(double)self.blocking,
			(double)Target.firstActiveAttackFrame - Target.attackFrame,
			(double)self.firstActiveAttackFrame - self.attackFrame,
			(double)Target.firstActiveGrabFrame - Target.grabFrame,
			(double)self.firstActiveGrabFrame - self.grabFrame,
			Target.charge,
			self.charge,
			Target.velocity,
			self.velocity,
			(double)frame
		});
	}
};