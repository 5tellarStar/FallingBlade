#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <array>
#include <list>
#include <vector>
#include "baseCharacter.hpp"
#include "neuralNetwork.hpp"

class TrainingPair
{
public:
	BaseCharacter player1 = BaseCharacter(false);
	BaseCharacter player2 = BaseCharacter(true);
    NeuralNetwork ai = NeuralNetwork(std::vector<int>{11, 15, 15, 15, 15, 7});
	TrainingPair()
	{

	}
	void Tick()
	{
        
        if (player1.Tick())
        {
            player1.ResetInput();
            player2.ResetInput();
        }
        if (player2.Tick())
        {
            player1.ResetInput();
            player2.ResetInput();
        }


        float tempVel1 = player1.velocity;
        float tempVel2 = player2.velocity;
        if (player1.canTurn)
        {
            if (player1.position < player2.position)
            {
                player1.direction = 1;
            }
            else
            {
                player1.direction = -1;
            }
        }
        if (player2.canTurn)
        {
            if (player2.position < player1.position)
            {
                player2.direction = 1;
            }
            else
            {
                player2.direction = -1;
            }
        }
        bool p1Hitp2 = player1.hitboxActive &&
            (
                (
                    player1.direction == -1 &&
                    (
                        (player2.position - player2.width < player1.position && player2.position - player2.width > player1.position - player1.attackRange) ||
                        (player2.position + player2.width < player1.position && player2.position + player2.width > player1.position - player1.attackRange)
                        )
                    ) ||
                (
                    player1.direction == 1 &&
                    (
                        (player2.position - player2.width > player1.position && player2.position - player2.width < player1.position + player1.attackRange) ||
                        (player2.position + player2.width > player1.position && player2.position + player2.width < player1.position + player1.attackRange)
                        )
                    ) ||
                (player2.position - player2.width < player1.position && player2.position + player2.width > player1.position)
                );

        bool p2Hitp1 = player2.hitboxActive &&
            (
                (
                    player2.direction == -1 &&
                    (
                        (player1.position - player1.width < player2.position && player1.position - player1.width > player2.position - player2.attackRange) ||
                        (player1.position + player1.width < player2.position && player1.position + player1.width > player2.position - player2.attackRange)
                        )
                    ) ||
                (
                    player2.direction == 1 &&
                    (
                        (player1.position - player1.width > player2.position && player1.position - player1.width < player2.position + player2.attackRange) ||
                        (player1.position + player1.width > player2.position && player1.position + player1.width < player2.position + player2.attackRange)
                        )
                    ) ||
                (player2.position - player2.width < player1.position && player2.position + player2.width > player1.position)
                );

        bool swordHit = player1.hitboxActive && player2.hitboxActive &&
            (
                (
                    player1.direction == 1 && player2.direction == -1 &&
                    player1.position + player1.attackRange > player2.position - player2.attackRange && player1.position + player1.attackRange < player2.position
                    ) ||
                (
                    player1.direction == -1 && player2.direction == 1 &&
                    player1.position - player1.attackRange < player2.position + player2.attackRange && player1.position + player1.attackRange > player2.position
                    )

                );

        if (p1Hitp2 && !p2Hitp1 && !swordHit)
        {
            if (player1.attackState == player2.blocking)
            {
                player2.sprite.setFillColor(sf::Color::Green);
                player2.AddForce((player1.attackVelocity / 2) * player1.mass * player1.direction);
            }
            else
            {
                player2.sprite.setFillColor(sf::Color::Black);
                player2.AddForce((tempVel1 - tempVel2 + player1.attackVelocity * player1.direction) * player1.mass);
            }
            player1.hitboxActive = false;
            player1.velocity = player1.direction * -1 * (player1.attackVelocity);
        }
        else if (!p1Hitp2 && p2Hitp1 && !swordHit)
        {
            if (player2.attackState == player1.blocking)
            {
                player1.sprite.setFillColor(sf::Color::Green);

                player1.AddForce((player2.attackVelocity / 2) * player2.mass * player2.direction);
            }
            else
            {
                player1.sprite.setFillColor(sf::Color::Black);
                player1.AddForce((tempVel2 - tempVel1 + player2.attackVelocity * player2.direction) * player2.mass);
            }
            player2.hitboxActive = false;
            player2.velocity = player2.direction * -1 * (player2.attackVelocity);
        }
        else if (swordHit)
        {
            player1.hitboxActive = false;
            player2.hitboxActive = false;
            player1.AddForce(((tempVel1 * player1.direction + player1.attackVelocity) * player2.direction * player1.mass + ((tempVel2 * player2.direction + player2.attackVelocity) * player2.direction) * player2.mass));
            player2.AddForce(((tempVel1 * player1.direction + player1.attackVelocity) * player1.direction * player1.mass + ((tempVel2 * player2.direction + player2.attackVelocity) * player1.direction) * player2.mass));
        }
	}

private:

};

