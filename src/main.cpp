#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include "baseCharacter.hpp"


bool battling = false;

int main()
{
    auto window = sf::RenderWindow{ { 512u, 128u }, "Fighter" };
    BaseCharacter player1(false);
    BaseCharacter player2(true);

    sf::Clock globalTime;

    sf::View veiw = window.getDefaultView();
    window.setFramerateLimit(144);
    sf::RectangleShape background(sf::Vector2f(512, 128));
    background.setFillColor(sf::Color::White);
    while (window.isOpen())
    {
        for (auto event = sf::Event{}; window.pollEvent(event);)
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            if (event.type == sf::Event::Resized)
            {
                veiw.setSize(sf::Vector2f(512,512 * event.size.height / event.size.width));
                window.setView(veiw);
            }
        }
        player1.Input();
        player2.Input();

        if (globalTime.getElapsedTime().asSeconds() > (1.f/24.f))
        {
            player1.Tick();
            player2.Tick();
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
            globalTime.restart();
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
                    )||
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
                player2.AddForce(((tempVel1* player1.direction + player1.attackVelocity)* player1.direction* player1.mass + ((tempVel2 * player2.direction + player2.attackVelocity) * player1.direction) * player2.mass));
            }
        }

        window.clear();
        window.draw(background);
        window.draw(player1.sprite);
        window.draw(player2.sprite);
        window.display();
    }
}
