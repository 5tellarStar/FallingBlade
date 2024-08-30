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

	int inputHorizontal;
	int inputVertical;

	bool inputAttack;
	bool inputDodge;
	bool inputSkill;

	bool canWalk = true;

	float velocity;

	float walkSpeed = 1;
	float walkSpeedMax = 5;

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
			sprite.setPosition(sf::Vector2f(96, 64));
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
			sprite.setPosition(sf::Vector2f(384, 64));
		}
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
		else if (sf::Keyboard::isKeyPressed(left) && sf::Keyboard::isKeyPressed(right))
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
			if (inputHorizontal = 1)
			{
				if (velocity < walkSpeedMax)
				{
					velocity += walkSpeed;
				}
			}
		}
	}


private:

};
