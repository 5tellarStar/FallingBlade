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
        player1.input();
        player2.input();

        if (globalTime.getElapsedTime().asSeconds() > (1.f/24.f))
        {
            player1.tick();
            player2.tick();
            globalTime.restart();
            bool p1Hitp2 = player1.hitboxActive != 0 && 
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

            bool p2Hitp1 = player2.hitboxActive != 0 &&
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

            bool swordHit = player1.hitboxActive != 0 && player2.hitboxActive != 0 &&
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


            if (swordHit && !p1Hitp2 && !p2Hitp1)
            {
            }
        }

        window.clear();
        window.draw(background);
        window.draw(player1.sprite);
        window.draw(player2.sprite);
        window.display();
    }
}
