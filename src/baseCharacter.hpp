#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#pragma once

class BaseCharacter
{
public:
	sf::Keyboard::Key up;
	sf::Keyboard::Key right;
	sf::Keyboard::Key down;
	sf::Keyboard::Key left;
	sf::Keyboard::Key attack;
	sf::Keyboard::Key dodge;
	sf::Keyboard::Key skill;

	int direction;
	bool canTurn = true;

	bool player2;

	bool dead = false;
	int winCount = 0;

	int inputHorizontal = 0;
	int inputVertical = 0;

	bool inputAttack = false;
	bool inputDodge = false;
	bool inputSkill = false;


	float velocity = 0;
	float mass = 1;

	float position;
	float width = 16;

	float top = 64;
	float gravity = 0;

	bool canWalk = true;
	float walkSpeed = 2;
	float walkSpeedMax = 5;
	float friction = 1;

	sf::RectangleShape sprite;

	bool canBlock = true;
	int blocking = 0;

	
	bool canDodge = true;
	bool isDodging = false;
	float dodgeForce = 10;
	float dodges = 3;
	float MaxDodges = 3;
	float dodgeCharge = 0.042f;


	bool canAttack = true;
	bool isCharging = false;
	bool isAttacking = false;
	int AttackFrames = 8;
	int AttackCancelableFrames = 4;
	int attackFrame = 0;
	float charge = 0;
	float chargeSpeed = 1;
	float maxCharge = 15;

	float staminaUse = 1.f;
	float staminaRegain = 0.1f;
	float exhaustion = 0;

	int firstActiveAttackFrame = 5;

	float attackVelocity = 7;
	bool hitboxActive = false;
	int attackState = 0;
	float highAttackRange = 32;
	float attackRange = 32;
	float lowAttackRange = 32;

	float distToEdge1;
	float distToEdge2;


	BaseCharacter(bool isPlayer2)
	{

		sprite.setSize(sf::Vector2f(32, 64));


		if (!isPlayer2)
		{
			up = sf::Keyboard::W;
			right = sf::Keyboard::D;
			down = sf::Keyboard::S;
			left = sf::Keyboard::A;
			attack = sf::Keyboard::U;
			dodge = sf::Keyboard::I;
			skill = sf::Keyboard::O;
			sprite.setFillColor(sf::Color::Blue);
			SetPosition(96);
			direction = 1;
			player2 = false;
		}
		else
		{
			up = sf::Keyboard::Up;
			right = sf::Keyboard::Right;
			down = sf::Keyboard::Down;
			left = sf::Keyboard::Left;
			attack = sf::Keyboard::Numpad7;
			dodge = sf::Keyboard::Numpad8;
			skill = sf::Keyboard::Numpad9;
			sprite.setFillColor(sf::Color::Red);
			SetPosition(416);
			direction = -1;
			player2 = true;
		}
	}

	void Reset()
	{
		velocity = 0;
		dead = false;
		top = 64;
		gravity = 0;
		if (!player2)
		{
			sprite.setFillColor(sf::Color::Blue);
			SetPosition(96);
			direction = 1;
		}
		else
		{
			sprite.setFillColor(sf::Color::Red);
			SetPosition(416);
			direction = -1;
		}
	}
	void Input(std::vector<double> inputs) 
	{
		if (inputs[0] > 0.9 && inputs[1] < 0.9)
		{
			inputVertical = 1;
		}
		else if (inputs[0] < 0.9 && inputs[1] > 0.9)
		{
			inputVertical = -1;
		}
		else
		{
			inputVertical = 0;
		}

		if (inputs[2] > 0.9 && inputs[3] < 0.9)
		{
			inputHorizontal = 1;
		}
		else if (inputs[2] < 0.9 && inputs[3] > 0.9)
		{
			inputHorizontal = -1;
		}
		else
		{
			inputHorizontal = 0;
		}

		inputAttack = inputs[4] > 0.9;
		inputDodge = inputs[5] > 0.9;
		inputSkill = inputs[6] > 0.9;
	}
	void Input()
	{
		if (sf::Keyboard::isKeyPressed(up) && !sf::Keyboard::isKeyPressed(down))
		{
			inputVertical = 1;
		}
		else if(sf::Keyboard::isKeyPressed(down) && !sf::Keyboard::isKeyPressed(up))
		{
			inputVertical = -1;
		}
		else
		{
			inputVertical = 0;
		}

		if (sf::Keyboard::isKeyPressed(right) && !sf::Keyboard::isKeyPressed(left))
		{
			inputHorizontal = 1;
		}
		else if (sf::Keyboard::isKeyPressed(left) && !sf::Keyboard::isKeyPressed(right))
		{
			inputHorizontal = -1;
		}
		else
		{
			inputHorizontal = 0;
		}

		inputAttack = sf::Keyboard::isKeyPressed(attack);
		inputDodge = sf::Keyboard::isKeyPressed(dodge);
		inputSkill = sf::Keyboard::isKeyPressed(skill);
	}

	void SetPosition(float pos)
	{
		position = pos;
		sprite.setPosition(sf::Vector2f(pos - width, top));
	}


	void AddForce(float force)
	{
		velocity += force / mass;
	}

	void Skill()
	{

	}

	void ResetInput()
	{
		inputHorizontal = 0;
		inputVertical = 0;
		inputAttack = false;
		inputDodge = false;
		inputSkill = false;
	}

	bool Tick()
	{
			Skill();
			blocking = inputVertical;

			if (inputAttack)
			{
				if (!isCharging && canAttack)
				{
					canBlock = false;
					isCharging = true;
					canAttack = false;
					canTurn = false;
					sprite.setFillColor(sf::Color::Red);
				}
				if (isCharging && charge < maxCharge)
				{
					charge += chargeSpeed;
				}
			}
			else if (isCharging)
			{
				attackState = inputVertical;
				canWalk = false;
				isAttacking = true;
				isCharging = false;
				AddForce(charge * inputHorizontal);
				charge = 0;
				attackFrame = 0;
			}

			if (isAttacking && attackFrame < AttackFrames)
			{
				if (attackFrame == AttackCancelableFrames)
				{
					canDodge = false;
				}
				if (attackFrame == firstActiveAttackFrame)
				{
					sprite.setFillColor(sf::Color::Yellow);
					hitboxActive = true;
				}
				attackFrame++;
			}
			else if (isAttacking && attackFrame < exhaustion + AttackFrames)
			{
				attackFrame++;
				sprite.setFillColor(sf::Color::Blue);
			}
			else if (isAttacking)
			{
				sprite.setFillColor(sf::Color::Blue);
				attackFrame = 0;
				hitboxActive = 0;
				isAttacking = false;
				canAttack = true;
				canWalk = true;
				canDodge = true;
				canBlock = true;
				canTurn = true;
				exhaustion += staminaUse;
			}

			if (dodges < MaxDodges)
			{
				dodges += dodgeCharge;
			}
			if (inputDodge)
			{
				if (canDodge && dodges >= 1)
				{
					dodges -= 1;
					canDodge = false;
					isAttacking = false;
					isCharging = false;
					canAttack = true;
					canBlock = true;
					canWalk = true;
					if ((inputHorizontal < 0 && velocity > 0) || (inputHorizontal > 0 && velocity < 0))
					{
						velocity = 0;
					}
					AddForce(dodgeForce * inputHorizontal);
				}
			}
			else
			{
				canDodge = true;
			}



			if (canWalk)
			{
				if (inputHorizontal == 1)
				{
					if (!isCharging)
					{
						if (velocity < walkSpeedMax)
						{
							velocity += walkSpeed;
						}
					}
					else
					{
						if (velocity < walkSpeedMax / 2)
						{
							velocity += walkSpeed / 2;
						}
					}
				}
				if (inputHorizontal == -1)
				{
					if (!isCharging)
					{
						if (velocity > -walkSpeedMax)
						{
							velocity -= walkSpeed;
						}
					}
					else
					{
						if (velocity > -walkSpeedMax / 2)
						{
							velocity -= walkSpeed / 2;
						}
					}
				}
			}
		

		SetPosition(position + velocity);

		

		if (exhaustion > 0)
		{
			exhaustion -= staminaRegain;
		}

		if (velocity < 0)
		{
			velocity += friction;
			if (velocity > 0)
			{
				velocity = 0;
			}
		}
		else if (velocity > 0)
		{
			velocity -= friction;
			if (velocity <= 0)
			{
				velocity = 0;
			}
		}

		distToEdge1 =  440 - (position - width);
		distToEdge2 = (position + width) - 72;

		

		if (distToEdge1 <= 0 || distToEdge2 <= 0)
		{
			sprite.setFillColor(sf::Color::Magenta);
			gravity += 2;
			top += gravity;
			if (!dead)
			{
				dead = true;
				return true;
			}
		}
		return false;

	}


private:

};
