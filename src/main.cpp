#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <array>
#include "baseCharacter.hpp"


bool battling = false;

int winsToWin = 3;
bool training = true;

int main()
{
    auto window = sf::RenderWindow{ { 512u, 128u }, "Falling Blade" };
    BaseCharacter player1(false);
    BaseCharacter player2(true);

    sf::Clock globalTime;


    sf::Texture titleTexture;
    titleTexture.loadFromFile("title.png");
    sf::Sprite titleSprite;
    titleSprite.setTexture(titleTexture);
    titleSprite.setPosition(sf::Vector2f(192, -32));


    sf::Texture platformTexture;
    platformTexture.loadFromFile("platform.png");
    sf::Sprite platformSprite;
    platformSprite.setTexture(platformTexture);
    platformSprite.setPosition(sf::Vector2f(64, 96));

    sf::View veiw = window.getDefaultView();
    window.setFramerateLimit(144);
    sf::RectangleShape background(sf::Vector2f(512, 128));
    background.setFillColor(sf::Color::White);
    while (window.isOpen())
    {
        while ((player1.winCount < winsToWin && player2.winCount < winsToWin) || !sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
        {
            for (auto event = sf::Event{}; window.pollEvent(event);)
            {
                if (event.type == sf::Event::Closed)
                {
                    window.close();
                }
                if (event.type == sf::Event::Resized)
                {
                    veiw.setSize(sf::Vector2f(512, 512 * event.size.height / event.size.width));
                    window.setView(veiw);
                }
            }
            if (!player1.dead && !player2.dead)
            {
                player1.Input();
                player2.Input();
            }
            else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
            {
                player1.Reset();
                player2.Reset();
                player1.sprite.setFillColor(sf::Color::Cyan);
            }

            if (globalTime.getElapsedTime().asSeconds() > (1.f / 24.f))
            {
                if (!training)
                {
                    if (player1.Tick())
                    {
                        player1.ResetInput();
                        player2.ResetInput();
                        player2.winCount++;
                    }
                    if (player2.Tick())
                    {
                        player1.ResetInput();
                        player2.ResetInput();
                        player1.winCount++;
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
                else
                {

                }
                globalTime.restart();
            }

            window.clear(sf::Color(51, 173, 255,0));
            window.draw(platformSprite);
            window.draw(titleSprite);
            window.draw(player1.sprite);
            window.draw(player2.sprite);
            window.display();
        }

        window.clear();
        window.display();
    }
}
