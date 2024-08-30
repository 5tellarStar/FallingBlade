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

	int inputHorizontal = 0;
	int inputVertical = 0;

	bool inputAttack = false;
	bool inputDodge = false;
	bool inputSkill = false;

	bool canWalk = true;

	float velocity = 0;

	float position;

	float walkSpeed = 2;
	float walkSpeedMax = 50;
	float friction = 1;

	sf::RectangleShape sprite;

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
			position = 96;
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
			position = 384;
		}
		sprite.setPosition(sf::Vector2f(position, 64));
	}
	void input()
	{
		if (sf::Keyboard::isKeyPressed(up) && ! sf::Keyboard::isKeyPressed(down))
		{
			inputVertical = 1;
		}
		else if(sf::Keyboard::isKeyPressed(down) && sf::Keyboard::isKeyPressed(up))
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

	void tick()
	{
		if (canWalk)
		{
			if (inputHorizontal == 1)
			{
				if (velocity < walkSpeedMax)
				{
					velocity += walkSpeed;
				}
			}
			if (inputHorizontal == -1)
			{
				if (velocity > -walkSpeedMax)
				{
					velocity -= walkSpeed;
				}
			}
		}

		position += velocity;
		sprite.setPosition(sf::Vector2f(position, 64));

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
