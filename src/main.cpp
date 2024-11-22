#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <array>
#include "baseCharacter.hpp"
#include "neuralNetwork.hpp"
#include "trainingPair.hpp"
#include <string>
#include <random>
#include <fstream>

int ImpactFrame = 0;
bool battling = false;
bool first = true;
int winsToWin = 3;
bool training = false;
double highestReward = -1;
std::vector<TrainingPair> pairs;
NeuralNetwork bestAi(std::vector<int>{13, 20, 20, 20, 7});

std::default_random_engine rnd{ std::random_device{}() };
std::uniform_real_distribution<double> dist(-0.2, 0.2);

int main()
{
    for (int i = 0; i < 50; i++)
    {
        pairs.push_back({ true,0,0,512,0,0,BaseCharacter(false),BaseCharacter(true),NeuralNetwork(std::vector<int>{13, 100, 100, 100, 100, 100, 7}),NeuralNetwork(std::vector<int>{13,100, 100, 100, 100, 100, 7})});
    }
    auto window = sf::RenderWindow{ { 512u, 128u }, "Falling Blade" };
    BaseCharacter player1(false);
    BaseCharacter player2(true);
    sf::Font font;
    font.loadFromFile("Roboto-Black.ttf");

    // Create a text
    sf::Text debug("hello", font);
    debug.setCharacterSize(30);
    debug.setFillColor(sf::Color::Red);
    sf::Clock globalTime;
    sf::Clock trainingTime;


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

    sf::View view = window.getDefaultView();
    window.setFramerateLimit(144);
    sf::RectangleShape background(sf::Vector2f(512, 128));
    background.setFillColor(sf::Color::White);

    std::ofstream trainingData1("trainingData1.txt");
    std::ofstream trainingData2("trainingData2.txt");
    std::string dataLine1;
    std::string oldDataLine1;
    std::string dataLine2;
    std::string oldDataLine2;

    while (window.isOpen())
    {
        while ((player1.winCount < winsToWin && player2.winCount < winsToWin) || !sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
        {
            window.clear(sf::Color(51, 173, 255, 0));
            window.draw(platformSprite);
            window.draw(titleSprite);
            for (auto event = sf::Event{}; window.pollEvent(event);)
            {
                if (event.type == sf::Event::Closed)
                {
                    window.close();
                }
                if (event.type == sf::Event::Resized)
                {
                    view.setSize(sf::Vector2f(512, 512 * event.size.height / event.size.width));
                    window.setView(view);
                }
            }
            if (training)
            {
                bool done = true;
                for(int i = 0; i < pairs.size(); i++)
                {
                    TrainingPair pair = pairs[i];
                    if (!pair.done)
                    {
                        pair.player2.Input(pair.ai2.CalcOutputs(std::vector<double>
                        {
                            pair.player2.distToEdge1,
                                pair.player2.distToEdge2,
                                pair.player1.distToEdge1,
                                pair.player1.distToEdge2,
                                pair.player2.position - pair.player1.position,
                                pair.player2.dodges,
                                pair.player1.dodges,
                                (double)pair.player1.blocking,
                                (double)pair.player2.blocking,
                                (double)pair.player2.firstActiveAttackFrame - pair.player2.attackFrame,
                                (double)pair.player1.firstActiveAttackFrame - pair.player1.attackFrame,
                                pair.player1.velocity,
                                pair.player2.velocity
                        }));
                        pair.player1.Input(pair.ai1.CalcOutputs(std::vector<double>
                        {
                                pair.player1.distToEdge2,
                            pair.player1.distToEdge1,
                                pair.player2.distToEdge2,
                                pair.player2.distToEdge1,
                                pair.player1.position - pair.player2.position,
                                pair.player1.dodges,
                                pair.player2.dodges,
                                (double)pair.player2.blocking,
                                (double)pair.player1.blocking,
                                (double)pair.player1.firstActiveAttackFrame - pair.player1.attackFrame,
                                (double)pair.player2.firstActiveAttackFrame - pair.player2.attackFrame,
                                pair.player1.velocity,
                                pair.player2.velocity
                        }));

                        if (pair.player1.Tick())
                        {
                            pair.done = true;   
                            pair.player1.ResetInput();
                            pair.player2.ResetInput();
                            pair.rewards1 -= 1000;
                        }
                        if (pair.player2.Tick())
                        {
                            pair.done = true;
                            pair.player1.ResetInput();
                            pair.player2.ResetInput();
                            pair.rewards2 -= 1000;
                        }

                        if (pair.player2.attackFrame - 1 == pair.player2.firstActiveAttackFrame)
                        {
                            pair.rewards2 += 512 - fabs(pair.player2.position - pair.player1.position);
                        }

                        if (pair.player1.attackFrame - 1 == pair.player1.firstActiveAttackFrame)
                        {
                            pair.rewards1 += 512 - fabs(pair.player2.position - pair.player1.position);
                        }

                        if (fabs(pair.player2.position - pair.player1.position) < pair.closestDist)
                        {
                            pair.closestDist = fabs(pair.player2.position - pair.player1.position);
                        }

                        if (fabs(pair.player2.velocity) > pair.fastestVelocity2)
                        {
                            pair.fastestVelocity2 = fabs(pair.player2.velocity);
                        }

                        if (fabs(pair.player1.velocity) > pair.fastestVelocity1)
                        {
                            pair.fastestVelocity1 = fabs(pair.player1.velocity);
                        }
                        if (pair.player1.velocity == 0)
                        {
                            pair.rewards1 -= 1;
                        }                        
                        if (pair.player2.velocity == 0)
                        {
                            pair.rewards2 -= 1;
                        }

                        float tempVel1 = pair.player1.velocity;
                        float tempVel2 = pair.player2.velocity;
                        if (pair.player1.canTurn)
                        {
                            if (pair.player1.position < pair.player2.position)
                            {
                                pair.player1.direction = 1;
                            }
                            else
                            {
                                pair.player1.direction = -1;
                            }
                        }
                        if (pair.player2.canTurn)
                        {
                            if (pair.player2.position < pair.player1.position)
                            {
                                pair.player2.direction = 1;
                            }
                            else
                            {
                                pair.player2.direction = -1;
                            }
                        }
                        bool p1Hitp2 = pair.player1.hitboxActive &&
                            (
                                (
                                    pair.player1.direction == -1 &&
                                    (
                                        (pair.player2.position - pair.player2.width < pair.player1.position && pair.player2.position - pair.player2.width > pair.player1.position - pair.player1.attackRange) ||
                                        (pair.player2.position + pair.player2.width < pair.player1.position && pair.player2.position + pair.player2.width > pair.player1.position - pair.player1.attackRange)
                                        )
                                    ) ||
                                (
                                    pair.player1.direction == 1 &&
                                    (
                                        (pair.player2.position - pair.player2.width > pair.player1.position && pair.player2.position - pair.player2.width < pair.player1.position + pair.player1.attackRange) ||
                                        (pair.player2.position + pair.player2.width > pair.player1.position && pair.player2.position + pair.player2.width < pair.player1.position + pair.player1.attackRange)
                                        )
                                    ) ||
                                (pair.player2.position - pair.player2.width < pair.player1.position && pair.player2.position + pair.player2.width > pair.player1.position)
                                );

                        bool p2Hitp1 = pair.player2.hitboxActive &&
                            (
                                (
                                    pair.player2.direction == -1 &&
                                    (
                                        (pair.player1.position - pair.player1.width < pair.player2.position && pair.player1.position - pair.player1.width > pair.player2.position - pair.player2.attackRange) ||
                                        (pair.player1.position + pair.player1.width < pair.player2.position && pair.player1.position + pair.player1.width > pair.player2.position - pair.player2.attackRange)
                                        )
                                    ) ||
                                (
                                    pair.player2.direction == 1 &&
                                    (
                                        (pair.player1.position - pair.player1.width > pair.player2.position && pair.player1.position - pair.player1.width < pair.player2.position + pair.player2.attackRange) ||
                                        (pair.player1.position + pair.player1.width > pair.player2.position && pair.player1.position + pair.player1.width < pair.player2.position + pair.player2.attackRange)
                                        )
                                    ) ||
                                (pair.player2.position - pair.player2.width < pair.player1.position && pair.player2.position + pair.player2.width > pair.player1.position)
                                );

                        bool swordHit = pair.player1.hitboxActive && pair.player2.hitboxActive &&
                            (
                                (
                                    pair.player1.direction == 1 && pair.player2.direction == -1 &&
                                    pair.player1.position + pair.player1.attackRange > pair.player2.position - pair.player2.attackRange && pair.player1.position + pair.player1.attackRange < pair.player2.position
                                    ) ||
                                (
                                    pair.player1.direction == -1 && pair.player2.direction == 1 &&
                                    pair.player1.position - pair.player1.attackRange < pair.player2.position + pair.player2.attackRange && pair.player1.position + pair.player1.attackRange > pair.player2.position
                                    )

                                );

                        if (p1Hitp2 && !p2Hitp1 && !swordHit)
                        {
                            if (pair.player1.attackState == pair.player2.blocking)
                            {
                                pair.player2.sprite.setFillColor(sf::Color::Green);
                                pair.player2.AddForce((pair.player1.attackVelocity / 2) * pair.player1.mass * pair.player1.direction);
                                pair.rewards1 += 1000;
                            }
                            else
                            {
                                pair.player2.sprite.setFillColor(sf::Color::Black);
                                pair.player2.AddForce((tempVel1 - tempVel2 + pair.player1.attackVelocity * pair.player1.direction) * pair.player1.mass);
                                pair.rewards1 += 3000;
                            }
                            pair.player1.hitboxActive = false;
                            pair.player1.velocity = pair.player1.direction * -1 * (pair.player1.attackVelocity);
                        }
                        else if (!p1Hitp2 && p2Hitp1 && !swordHit)
                        {
                            if (pair.player2.attackState == pair.player1.blocking)
                            {
                                pair.player1.sprite.setFillColor(sf::Color::Green);

                                pair.player1.AddForce((pair.player2.attackVelocity / 2) * pair.player2.mass * pair.player2.direction);
                                pair.rewards2 += 1000;
                            }
                            else
                            {
                                pair.player1.sprite.setFillColor(sf::Color::Black);
                                pair.player1.AddForce((tempVel2 - tempVel1 + pair.player2.attackVelocity * pair.player2.direction) * pair.player2.mass);
                                pair.rewards2 += 3000;
                            }
                            pair.player2.hitboxActive = false;
                            pair.player2.velocity = pair.player2.direction * -1 * (pair.player2.attackVelocity);
                        }
                        else if (swordHit)
                        {
                            pair.player1.hitboxActive = false;
                            pair.player2.hitboxActive = false;
                            pair.player1.AddForce(((tempVel1 * pair.player1.direction + pair.player1.attackVelocity) * pair.player2.direction * pair.player1.mass + ((tempVel2 * pair.player2.direction + pair.player2.attackVelocity) * pair.player2.direction) * pair.player2.mass));
                            pair.player2.AddForce(((tempVel1 * pair.player1.direction + pair.player1.attackVelocity) * pair.player1.direction * pair.player1.mass + ((tempVel2 * pair.player2.direction + pair.player2.attackVelocity) * pair.player1.direction) * pair.player2.mass));
                        }
                        window.draw(pair.player1.sprite);
                        window.draw(pair.player2.sprite);
                        for each (std::vector<sf::CircleShape> vectors in pair.ai1.nodes)
                        {
                            for each (sf::CircleShape circle in vectors)
                            {
                                window.draw(circle);
                            }
                        }
                        done = false;
                        pairs[i] = pair;
                    }
                    
                }
                if (trainingTime.getElapsedTime().asSeconds() > 30.f)
                {
                    for (int i = 0; i < pairs.size(); i++)
                    {
                        TrainingPair pair = pairs[i];
                        if (!pair.done)
                        {
                            pair.done = true;
                            
                        }
                        pairs[i] = pair;
                    }
                    trainingTime.restart();
                }
                if (done)
                {
                    trainingTime.restart();

                    for (int i = 0; i < pairs.size(); i++)
                    {
                        TrainingPair pair = pairs[i];
                        pair.player1.Reset();
                        pair.player2.Reset();
                        pair.done = false;
                        pair.rewards1 += pair.fastestVelocity1 * 10;
                        pair.rewards2 += pair.fastestVelocity2 * 10;
                        pair.rewards2 += (512 - pair.closestDist);
                        pair.rewards1 += (512 - pair.closestDist);
                        if (pair.rewards1 > highestReward)
                        {
                            highestReward = pair.rewards1;
                            bestAi = pair.ai1;
                        }
                        if (pair.rewards2 > highestReward)
                        {
                            highestReward = pair.rewards2;
                            bestAi = pair.ai2;
                        }
                        debug.setString(std::to_string(highestReward));
                        pair.rewards1 = 0;
                        pair.rewards2 = 0;
                        pair.closestDist = 512;
                        pair.fastestVelocity1 = 0;
                        pair.fastestVelocity2 = 0;
                        pairs[i] = pair;
                    }
                    if (!first)
                    {
                        for (int i = 0; i < pairs.size(); i++)
                        {
                            TrainingPair pair = pairs[i];
                            pair.ai1 = bestAi;
                            pair.ai2 = bestAi;
                            for (int j = 0; j < pair.ai1.layers.size(); j++)
                            {
                                Layer layer = pair.ai1.layers[j];
                                Layer layer2 = pair.ai2.layers[j];
                                for (int k = 0; k < layer.weights.size(); k++)
                                {
                                    std::vector<double> weights = layer.weights[k];
                                    std::vector<double> weights2 = layer2.weights[k];
                                    for (int l = 0; l < weights.size(); l++)
                                    {
                                        weights[l] += dist(rnd);
                                        weights2[l] += dist(rnd);
                                    }
                                    layer.weights[k] = weights;
                                    layer2.weights[k] = weights2;
                                }
                                for (int k = 0; k < layer.biases.size(); k++)
                                {
                                    layer.biases[k] += dist(rnd);
                                    layer2.biases[k] += dist(rnd);
                                }
                                pair.ai1.layers[j] = layer;
                                pair.ai2.layers[j] = layer2;
                            }

                            for (int j = 0; j < pair.ai2.layers.size(); j++)
                            {
                                Layer layer = pair.ai2.layers[j];
                                for (int k = 0; k < layer.weights.size(); k++)
                                {
                                    std::vector<double> weights = layer.weights[k];
                                    for (int l = 0; l < weights.size(); l++)
                                    {
                                        weights[l] += dist(rnd);
                                    }
                                    layer.weights[k] = weights;
                                }
                                for (int k = 0; k < layer.biases.size(); k++)
                                {
                                    layer.biases[k] += dist(rnd);
                                }
                                pair.ai2.layers[j] = layer;
                            }
                            pairs[i] = pair;
                        }
                    }
                    first = false;
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
                    training = false;
            }
            else
            {
                if (!player1.dead && !player2.dead)
                {
                    player1.Input();
                    player2.Input();
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
                {
                    player1.Reset();
                    player2.Reset();
                }

                if (globalTime.getElapsedTime().asSeconds() > (1.0 / 12.0) && ImpactFrame == 0)
                {
                    
                    if (!first)
                    {
                        if (!player1.dead && !player2.dead)
                        {
                            dataLine2 +=
                                std::to_string(player2.inputVertical) + "|" +
                                std::to_string(player2.inputHorizontal) + "|" +
                                std::to_string(player2.inputAttack) + "|" +
                                std::to_string(player2.inputDodge) + "|" +
                                std::to_string(player2.inputSkill);
                            dataLine1 +=
                                std::to_string(player1.inputVertical) + "|" +
                                std::to_string(player1.inputHorizontal) + "|" +
                                std::to_string(player1.inputAttack) + "|" +
                                std::to_string(player1.inputDodge) + "|" +
                                std::to_string(player1.inputSkill);
                            if (dataLine2 != oldDataLine2)
                            {
                                trainingData2 << dataLine2 << "\n";
                                oldDataLine2 = dataLine2;
                            }
                            if (dataLine1 != oldDataLine1)
                            {
                                trainingData1 << dataLine1 << "\n";
                                oldDataLine1 = dataLine1;
                            }
                        }
                    }
                    first = false;
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
                        if (player1.attackState == 2)
                        {
                            player2.AddForce((-player1.direction)*8);
                        }
                        else
                        {
                            if (player1.attackState == player2.blocking)
                            {
                                player2.sprite.setFillColor(sf::Color::Green);
                                player2.AddForce((player1.attackVelocity / 2) * player1.mass * player1.direction);
                                player2.blocked = true;
                            }
                            else
                            {
                                player2.sprite.setFillColor(sf::Color::Black);
                                player2.AddForce((tempVel1 - tempVel2 + player1.attackVelocity * player1.direction) * player1.mass);
                            }
                        player1.velocity = player1.direction * -1 * (player1.attackVelocity);
                        ImpactFrame = 5;
                        }
                        player1.hitboxActive = false;
                    }
                    else if (!p1Hitp2 && p2Hitp1 && !swordHit)
                    {
                        if (player2.attackState == 2)
                        {
                            player2.AddForce((-player1.direction) * 8);
                        }
                        else
                        {
                            if (player2.attackState == player1.blocking)
                            {
                                player1.sprite.setFillColor(sf::Color::Green);
                                player1.AddForce((player2.attackVelocity / 2) * player2.mass * player2.direction);
                                player1.blocked = true;
                            }
                            else
                            {
                                player1.sprite.setFillColor(sf::Color::Black);
                                player1.AddForce((tempVel2 - tempVel1 + player2.attackVelocity * player2.direction) * player2.mass);
                            }
                        player2.velocity = player2.direction * -1 * (player2.attackVelocity);
                        ImpactFrame = 5;
                        }
                        player2.hitboxActive = false;
                    }
                    else if (swordHit)
                    {
                        player1.hitboxActive = false;
                        player2.hitboxActive = false;
                        player1.AddForce(((tempVel1 * player1.direction + player1.attackVelocity) * player2.direction * player1.mass + ((tempVel2 * player2.direction + player2.attackVelocity) * player2.direction) * player2.mass));
                        player2.AddForce(((tempVel1 * player1.direction + player1.attackVelocity) * player1.direction * player1.mass + ((tempVel2 * player2.direction + player2.attackVelocity) * player1.direction) * player2.mass));
                        ImpactFrame = 5;
                    }

                    if (!player1.dead && !player2.dead)
                    {
                        dataLine2 =
                            std::to_string(player2.distToEdge2) + "|" +
                            std::to_string(player2.distToEdge1) + "|" +
                            std::to_string(player1.distToEdge2) + "|" +
                            std::to_string(player1.distToEdge1) + "|" +
                            std::to_string(player1.position - player2.position) + "|" +
                            std::to_string(player2.dodges) + "|" +
                            std::to_string(player1.dodges) + "|" +
                            std::to_string((double)player2.blocking) + "|" +
                            std::to_string((double)player1.blocking) + "|" +
                            std::to_string((double)player2.firstActiveAttackFrame - player2.attackFrame) + "|" +
                            std::to_string((double)player1.firstActiveAttackFrame - player1.attackFrame) + "|" +
                            std::to_string((double)player2.firstActiveGrabFrame - player2.grabFrame) + "|" +
                            std::to_string((double)player1.firstActiveGrabFrame - player1.grabFrame) + "|" +
                            std::to_string(player2.charge) + "|" +
                            std::to_string(player1.charge) + "|" +
                            std::to_string(player2.velocity) + "|" +
                            std::to_string(player1.velocity) + "|"
                            ;
                        dataLine1 =
                            std::to_string(player1.distToEdge1) + "|" +
                            std::to_string(player1.distToEdge2) + "|" +
                            std::to_string(player2.distToEdge1) + "|" +
                            std::to_string(player2.distToEdge2) + "|" +
                            std::to_string(player1.position - player2.position) + "|" +
                            std::to_string(player1.dodges) + "|" +
                            std::to_string(player2.dodges) + "|" +
                            std::to_string((double)player1.blocking) + "|" +
                            std::to_string((double)player2.blocking) + "|" +
                            std::to_string((double)player1.firstActiveAttackFrame - player1.attackFrame) + "|" +
                            std::to_string((double)player2.firstActiveAttackFrame - player2.attackFrame) + "|" +
                            std::to_string((double)player1.firstActiveGrabFrame - player1.grabFrame) + "|" +
                            std::to_string((double)player2.firstActiveGrabFrame - player2.grabFrame) + "|" +
                            std::to_string(player1.charge) + "|" +
                            std::to_string(player2.charge) + "|" +
                            std::to_string(player1.velocity) + "|" +
                            std::to_string(player2.velocity) + "|"
                            ;
                    }
                    globalTime.restart();
                }
                else
                {
                    if (globalTime.getElapsedTime().asSeconds() > (1.f / 12.f))
                    {
                        ImpactFrame--;
                        globalTime.restart();
                    }
                }
                window.draw(player1.sprite);
                window.draw(player1.legsSprite);
                window.draw(player2.sprite);
            }
            window.draw(debug);
            window.display();
        }

        window.clear();
        window.display();
    }
    trainingData2.close();
}
