#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

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

	int inputHorizontal = 0;
	int inputVertical = 0;

	bool inputAttack = false;
	bool inputDodge = false;
	bool inputSkill = false;


	float velocity = 0;
	float mass = 1;

	float position;
	float width = 16;

	bool canWalk = true;
	float walkSpeed = 2;
	float walkSpeedMax = 5;
	float friction = 1;

	sf::RectangleShape sprite;

	bool canBlock = true;
	int blocking = 0;

	bool canDodge = true;
	int isDodging = 0;
	float dodgeForce = 7;

	int dodgeFrames = 6;
	int dodgeFrame = 0;

	bool canAttack = true;
	bool charging = false;
	bool attacking = false;
	int AttackFrames = 8;
	int AttackCancelableFrames = 4;
	int attackFrame = 0;
	float charge = 0;
	float chargeSpeed = 1;
	float maxCharge = 10;

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

	BaseCharacter(bool player2)
	{

		sprite.setSize(sf::Vector2f(32, 64));


		if (!player2)
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
			SetPosition(384);
			direction = -1;
		}
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
		sprite.setPosition(sf::Vector2f(pos - width, 64));
	}
	void AddForce(float force)
	{
		velocity += force / mass;
	}
	void Skill()
	{

	}

	void Tick()
	{
		Skill();
		blocking = inputVertical;

		if (inputAttack)
		{
			if (!charging && canAttack)
			{
				canBlock = false;
				charging = true;
				canAttack = false;
				canTurn = false;
				sprite.setFillColor(sf::Color::Red);
			}
			if (charging && charge < maxCharge)
			{
				charge += chargeSpeed;
			}
		}
		else if (charging)
		{
			attackState = inputVertical;
			canWalk = false;
			attacking = true;
			charging = false;
			AddForce(charge * inputHorizontal);
			charge = 0;
			attackFrame = 0;
		}

		if (attacking && attackFrame < AttackFrames)
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
		else if (attacking && attackFrame < exhaustion)
		{
			attackFrame++;
			sprite.setFillColor(sf::Color::Blue);
		}
		else if(attacking)
		{
			sprite.setFillColor(sf::Color::Blue);
			hitboxActive = 0;
			attacking = false;
			canAttack = true;
			canWalk = true;
			canDodge = true;
			canBlock = true;
			canTurn = true;
			exhaustion += staminaUse;
		}

		if (inputDodge) 
		{
			if (isDodging == 0 && canDodge)
			{
				charge = 0;
				canWalk = false;
				canDodge = false;
				canTurn = false;
				attacking = false;
				charging = false;
				canAttack = false;
				if (inputVertical == 1)
				{
					isDodging = 1;
					canBlock = false;
				}
				else
				{
					isDodging = -1;
				}
				AddForce(dodgeForce * inputHorizontal);
				dodgeFrame = 0;
			}
			dodgeFrame++;
		}
		else if (isDodging != 0 && dodgeFrame > dodgeFrames)
		{
			isDodging = 0;
			canWalk = true;
			canDodge = true;
			canBlock = true;
			canTurn = true;
			canAttack = true;
		}
		else
		{
			dodgeFrame++;
		}

		if (canWalk)
		{
			if (inputHorizontal == 1)
			{
				if (!charging)
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
				if (!charging)
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
		else if (velocity >= 0)
		{
			velocity -= friction;
			if (velocity <= 0)
			{
				velocity = 0;
			}
		}
	}


private:

};
