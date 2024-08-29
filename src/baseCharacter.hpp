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

	int inputAction;

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
	
	}


private:

};
