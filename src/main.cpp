#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <array>
#include "baseCharacter.hpp"
#include "neuralNetwork.hpp"
#include "trainingPair.hpp"
#include "CPU.hpp"
#include <string>
#include <random>
#include <fstream>

bool fightingCPU = false;
int ImpactFrame = 0;
bool battling = false;
bool first = true;
int winsToWin = 3;
bool training = false;
double highestReward = -1;
int frame = 0;
int selectedButton = 0;
int selectedDifficulty = 0;
bool Settings = false;


NeuralNetwork bestAi(std::vector<int>{13, 20, 20, 20, 7});
std::vector<TrainingPair> pairs;

std::default_random_engine rnd{ std::random_device{}() };
std::uniform_real_distribution<double> dist(-0.2, 0.2);

int main()
{
    bestAi.Randomize();

    auto window = sf::RenderWindow{ { 512u, 128u }, "Falling Blade" };
    BaseCharacter player1(false);
    BaseCharacter player2(true);

    CPU* test = new HardCodedCPUVariable(2);

    sf::Clock globalTime;

    sf::Texture titleTexture;
    titleTexture.loadFromFile("title.png");
    sf::Sprite titleSprite;
    titleSprite.setTexture(titleTexture);
    titleSprite.setPosition(sf::Vector2f(192, -240));

    sf::Texture vsHumanTexture;
    vsHumanTexture.loadFromFile("vsHuman.png");
    sf::Sprite vsHumanSprite;
    vsHumanSprite.setTexture(vsHumanTexture);
    vsHumanSprite.setPosition(sf::Vector2f(222, -160));

    sf::Texture vsCPUTexture;
    vsCPUTexture.loadFromFile("vsCPU.png");
    sf::Sprite vsCPUSprite;
    vsCPUSprite.setTexture(vsCPUTexture);
    vsCPUSprite.setPosition(sf::Vector2f(222, -120));

    sf::Texture settingsTexture;
    settingsTexture.loadFromFile("settings.png");
    sf::Sprite settingsSprite;
    settingsSprite.setTexture(settingsTexture);
    settingsSprite.setPosition(sf::Vector2f(222, -80));

    sf::Texture CPULevelTexture;
    CPULevelTexture.loadFromFile("CPULevel.png");
    sf::Sprite CPULevelSprite;
    CPULevelSprite.setTexture(CPULevelTexture);
    CPULevelSprite.setPosition(sf::Vector2f(221, -160));
    CPULevelSprite.setColor(sf::Color::Transparent);

    sf::Texture CPULevelsTexture;
    CPULevelsTexture.loadFromFile("CPULevels.png");
    sf::IntRect CPULevelsRectSource = sf::IntRect(74 * 0, 0, 74, 17);
    sf::Sprite CPULevelsSprite;
    CPULevelsSprite.setTextureRect(CPULevelsRectSource);
    CPULevelsSprite.setTexture(CPULevelsTexture);
    CPULevelsSprite.setPosition(sf::Vector2f(302, -160));
    CPULevelsSprite.setColor(sf::Color::Transparent);


    sf::Texture selectorTexture;
    selectorTexture.loadFromFile("selector.png");
    sf::Sprite selectorSprite;
    selectorSprite.setTexture(selectorTexture);
    selectorSprite.setPosition(sf::Vector2f(190, -156));

    sf::Texture platformTexture;
    platformTexture.loadFromFile("platform.png");
    sf::Sprite platformSprite;
    platformSprite.setTexture(platformTexture);
    platformSprite.setPosition(sf::Vector2f(64, 96));

    sf::View view = window.getDefaultView();
    view.setCenter(view.getCenter().x, -128);
    window.setView(view);
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
        window.clear(sf::Color(51, 173, 255, 0));
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

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && globalTime.getElapsedTime().asSeconds() > 0.2f)
        {
            if (selectedButton != 0)
            {
                selectedButton--;
            }
            globalTime.restart();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && globalTime.getElapsedTime().asSeconds() > 0.2f)
        {
            if (!(selectedButton == 2 || (Settings && selectedButton == 0)))
            {
                selectedButton++;
            }
            globalTime.restart();
        }



        if ((selectedButton == 0 || selectedButton == 1) && sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) && !Settings)
        {
            fightingCPU = selectedButton == 1;
            while (view.getCenter().y != 64)
            {
                window.clear(sf::Color(51, 173, 255, 0));
                view.setCenter(view.getCenter().x, view.getCenter().y + 2);
                window.setView(view);
                window.draw(titleSprite);
                window.draw(platformSprite);
                window.draw(vsHumanSprite);
                window.draw(vsCPUSprite);
                window.draw(settingsSprite);
                window.draw(CPULevelSprite);
                window.draw(CPULevelsSprite);
                window.draw(selectorSprite);
                window.draw(player1.sprite);
                window.draw(player1.legsSprite);
                window.draw(player1.upperBodySprite);
                window.draw(player1.upperBodyColorSprite);
                window.draw(player2.sprite);
                window.draw(player2.legsSprite);
                window.draw(player2.upperBodySprite);
                window.draw(player2.upperBodyColorSprite);
                for (int i = 0; i < 3; i++)
                {
                    window.draw(player1.Stamina[i]);
                    window.draw(player2.Stamina[i]);
                }
                window.display();
            }

            if (!training)
            {
                if (fightingCPU)
                {
                    switch (selectedDifficulty)
                    {
                    case 0:
                        pairs = std::vector<TrainingPair>{ {player1,player2,CPU(),HardCodedCPUVariable(200)} };
                        break;
                    case 1:
                        pairs = std::vector<TrainingPair>{ {player1,player2,CPU(),HardCodedCPUVariable(100)} };
                        break;
                    case 2:
                        pairs = std::vector<TrainingPair>{ {player1,player2,CPU(),HardCodedCPUVariable(50)} };
                        break;
                    case 3:
                        pairs = std::vector<TrainingPair>{ {player1,player2,CPU(),HardCodedCPUVariable(10)} };
                        break;
                    case 4:
                        pairs = std::vector<TrainingPair>{ {player1,player2,CPU(),HardCodedCPUVariable(1)} };
                        break;
                    case 5:
                        pairs = std::vector<TrainingPair>{ {player1,player2,CPU(),HardCodedCPUImpossiable()} };
                        break;
                    case 6:
                        break;
                    case 7:
                        break;
                    }
                }
                else
                {
                    pairs = std::vector<TrainingPair>{ {player1,player2,CPU(),CPU()} };
                }
            }

            while (((player1.winCount < winsToWin && player2.winCount < winsToWin) || !(sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) && player1.currentLegsFrame == 0 && player1.currentUpperBodyFrame == 0 && player2.currentLegsFrame == 0 && player2.currentUpperBodyFrame == 0)))
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
                    if (event.type == sf::Event::KeyPressed)
                    {
                        for (int i = 0; i < pairs.size(); i++)
                        {
                            if (event.key.code == pairs[i].player1.up)
                                pairs[i].player1.upIsPressed = true;
                            if (event.key.code == pairs[i].player1.down)
                                pairs[i].player1.downIsPressed = true;
                            if (event.key.code == pairs[i].player1.right)
                                pairs[i].player1.rightIsPressed = true;
                            if (event.key.code == pairs[i].player1.left)
                                pairs[i].player1.leftIsPressed = true;
                            if (event.key.code == pairs[i].player1.attack)
                                pairs[i].player1.attackIsPressed = true;
                            if (event.key.code == pairs[i].player1.dodge)
                                pairs[i].player1.dodgeIsPressed = true;
                            if (event.key.code == pairs[i].player1.skill)
                                pairs[i].player1.skillIsPressed = true;

                            if (event.key.code == pairs[i].player2.up)
                                pairs[i].player2.upIsPressed = true;
                            if (event.key.code == pairs[i].player2.down)
                                pairs[i].player2.downIsPressed = true;
                            if (event.key.code == pairs[i].player2.right)
                                pairs[i].player2.rightIsPressed = true;
                            if (event.key.code == pairs[i].player2.left)
                                pairs[i].player2.leftIsPressed = true;
                            if (event.key.code == pairs[i].player2.attack)
                                pairs[i].player2.attackIsPressed = true;
                            if (event.key.code == pairs[i].player2.dodge)
                                pairs[i].player2.dodgeIsPressed = true;
                            if (event.key.code == pairs[i].player2.skill)
                                pairs[i].player2.skillIsPressed = true;
                        }

                    }
                    if (event.type == sf::Event::KeyReleased)
                    {
                        for (int i = 0; i < pairs.size(); i++)
                        {
                            if (event.key.code == pairs[0].player1.up)
                                pairs[0].player1.upIsPressed = false;
                            if (event.key.code == pairs[0].player1.down)
                                pairs[0].player1.downIsPressed = false;
                            if (event.key.code == pairs[0].player1.right)
                                pairs[0].player1.rightIsPressed = false;
                            if (event.key.code == pairs[0].player1.left)
                                pairs[0].player1.leftIsPressed = false;
                            if (event.key.code == pairs[0].player1.attack)
                                pairs[0].player1.attackIsPressed = false;
                            if (event.key.code == pairs[0].player1.dodge)
                                pairs[0].player1.dodgeIsPressed = false;
                            if (event.key.code == pairs[0].player1.skill)
                                pairs[0].player1.skillIsPressed = false;

                            if (event.key.code == pairs[0].player2.up)
                                pairs[0].player2.upIsPressed = false;
                            if (event.key.code == pairs[0].player2.down)
                                pairs[0].player2.downIsPressed = false;
                            if (event.key.code == pairs[0].player2.right)
                                pairs[0].player2.rightIsPressed = false;
                            if (event.key.code == pairs[0].player2.left)
                                pairs[0].player2.leftIsPressed = false;
                            if (event.key.code == pairs[0].player2.attack)
                                pairs[0].player2.attackIsPressed = false;
                            if (event.key.code == pairs[0].player2.dodge)
                                pairs[0].player2.dodgeIsPressed = false;
                            if (event.key.code == pairs[0].player2.skill)
                                pairs[0].player2.skillIsPressed = false;
                        }
                    }
                }

                if (!training)
                {

                }

                if ((globalTime.getElapsedTime().asSeconds() > (1.f / 24.f) || training) && ImpactFrame == 0)
                {
                    for (int i = 0; i < pairs.size(); i++)
                    {
                        if (!first && !training)
                        {
                            if (!pairs[0].player1.dead && !pairs[0].player2.dead)
                            {
                                dataLine2 +=
                                    std::to_string(pairs[0].player2.inputVertical) + "|" +
                                    std::to_string(pairs[0].player2.inputHorizontal) + "|" +
                                    std::to_string(pairs[0].player2.inputAttack) + "|" +
                                    std::to_string(pairs[0].player2.inputDodge) + "|" +
                                    std::to_string(pairs[0].player2.inputSkill);
                                dataLine1 +=
                                    std::to_string(pairs[0].player1.inputVertical) + "|" +
                                    std::to_string(pairs[0].player1.inputHorizontal) + "|" +
                                    std::to_string(pairs[0].player1.inputAttack) + "|" +
                                    std::to_string(pairs[0].player1.inputDodge) + "|" +
                                    std::to_string(pairs[0].player1.inputSkill);
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

                        if (!pairs[0].player1.dead && !pairs[0].player2.dead)
                        {
                            pairs[0].player1.Input();
                            pairs[0].player2.Input();
                            pairs[0].player1.Input((pairs[0].cpu1.inputs(pairs[0].player2, pairs[0].player1, pairs[0].frame)));
                            pairs[0].player2.Input((pairs[0].cpu2.inputs(pairs[0].player1, pairs[0].player2, pairs[0].frame)));
                        }

                        first = false;
                        if (pairs[0].player1.Tick())
                        {
                            pairs[0].player1.ResetInput();
                            pairs[0].player2.ResetInput();
                            pairs[0].player2.winCount++;
                        }
                        if (pairs[0].player2.Tick())
                        {
                            pairs[0].player1.ResetInput();
                            pairs[0].player2.ResetInput();
                            pairs[0].player1.winCount++;
                        }

                        if (pairs[0].player1.closestDist < fabs(pairs[0].player1.position - pairs[0].player2.position))
                        {
                            pairs[0].player1.closestDist = fabs(pairs[0].player1.position - pairs[0].player2.position);
                            pairs[0].player2.closestDist = fabs(pairs[0].player1.position - pairs[0].player2.position);
                        }


                        float tempVel1 = pairs[0].player1.velocity;
                        float tempVel2 = pairs[0].player2.velocity;
                        if (pairs[0].player1.canTurn)
                        {
                            if (pairs[0].player1.position < pairs[0].player2.position)
                            {
                                pairs[0].player1.direction = 1;
                            }
                            else
                            {
                                pairs[0].player1.direction = -1;
                            }
                        }
                        if (pairs[0].player2.canTurn)
                        {
                            if (pairs[0].player2.position < pairs[0].player1.position)
                            {
                                pairs[0].player2.direction = 1;
                            }
                            else
                            {
                                pairs[0].player2.direction = -1;
                            }
                        }
                        bool p1Hitp2 = pairs[0].player1.hitboxActive &&
                            (
                                (
                                    pairs[0].player1.direction == -1 &&
                                    (
                                        (pairs[0].player2.position - pairs[0].player2.width < pairs[0].player1.position && pairs[0].player2.position - pairs[0].player2.width > pairs[0].player1.position - pairs[0].player1.attackRange) ||
                                        (pairs[0].player2.position + pairs[0].player2.width < pairs[0].player1.position && pairs[0].player2.position + pairs[0].player2.width > pairs[0].player1.position - pairs[0].player1.attackRange)
                                        )
                                    ) ||
                                (
                                    pairs[0].player1.direction == 1 &&
                                    (
                                        (pairs[0].player2.position - pairs[0].player2.width > pairs[0].player1.position && pairs[0].player2.position - pairs[0].player2.width < pairs[0].player1.position + pairs[0].player1.attackRange) ||
                                        (pairs[0].player2.position + pairs[0].player2.width > pairs[0].player1.position && pairs[0].player2.position + pairs[0].player2.width < pairs[0].player1.position + pairs[0].player1.attackRange)
                                        )
                                    ) ||
                                (pairs[0].player2.position - pairs[0].player2.width < pairs[0].player1.position && pairs[0].player2.position + pairs[0].player2.width > pairs[0].player1.position)
                                );

                        bool p2Hitp1 = pairs[0].player2.hitboxActive &&
                            (
                                (
                                    pairs[0].player2.direction == -1 &&
                                    (
                                        (pairs[0].player1.position - pairs[0].player1.width < pairs[0].player2.position && pairs[0].player1.position - pairs[0].player1.width > pairs[0].player2.position - pairs[0].player2.attackRange) ||
                                        (pairs[0].player1.position + pairs[0].player1.width < pairs[0].player2.position && pairs[0].player1.position + pairs[0].player1.width > pairs[0].player2.position - pairs[0].player2.attackRange)
                                        )
                                    ) ||
                                (
                                    pairs[0].player2.direction == 1 &&
                                    (
                                        (pairs[0].player1.position - pairs[0].player1.width > pairs[0].player2.position && pairs[0].player1.position - pairs[0].player1.width < pairs[0].player2.position + pairs[0].player2.attackRange) ||
                                        (pairs[0].player1.position + pairs[0].player1.width > pairs[0].player2.position && pairs[0].player1.position + pairs[0].player1.width < pairs[0].player2.position + pairs[0].player2.attackRange)
                                        )
                                    ) ||
                                (pairs[0].player2.position - pairs[0].player2.width < pairs[0].player1.position && pairs[0].player2.position + pairs[0].player2.width > pairs[0].player1.position)
                                );

                        bool swordHit = pairs[0].player1.hitboxActive && pairs[0].player2.hitboxActive &&
                            (
                                (
                                    pairs[0].player1.direction == 1 && pairs[0].player2.direction == -1 &&
                                    pairs[0].player1.position + pairs[0].player1.attackRange > pairs[0].player2.position - pairs[0].player2.attackRange && pairs[0].player1.position + pairs[0].player1.attackRange < pairs[0].player2.position
                                    ) ||
                                (
                                    pairs[0].player1.direction == -1 && pairs[0].player2.direction == 1 &&
                                    pairs[0].player1.position - pairs[0].player1.attackRange < pairs[0].player2.position + pairs[0].player2.attackRange && pairs[0].player1.position + pairs[0].player1.attackRange > pairs[0].player2.position
                                    )

                                );

                        if (p1Hitp2 && !p2Hitp1 && !swordHit)
                        {
                            if (pairs[0].player1.attackState == 2)
                            {
                                pairs[0].player2.AddForce((-pairs[0].player1.direction) * 8);
                            }
                            else
                            {
                                if (pairs[0].player1.attackState == pairs[0].player2.blocking && pairs[0].player2.canBlock)
                                {

                                    pairs[0].player1.upperBodyRectSource.left -= 127;
                                    pairs[0].player1.upperBodyColorSprite.setTextureRect(pairs[0].player1.upperBodyRectSource);
                                    pairs[0].player1.upperBodySprite.setTextureRect(pairs[0].player1.upperBodyRectSource);
                                    pairs[0].player1.currentUpperBodyAnimation += 3;
                                    pairs[0].player1.currentUpperBodyFrame = 0;
                                    pairs[0].player2.sprite.setFillColor(sf::Color::Green);
                                    pairs[0].player2.AddForce((pairs[0].player1.attackVelocity / 2) * pairs[0].player1.mass * pairs[0].player1.direction);
                                    pairs[0].player2.blocked = true;
                                }
                                else
                                {
                                    pairs[0].player2.canTurn = true;
                                    pairs[0].player2.canAttack = true;
                                    pairs[0].player2.canBlock = true;
                                    pairs[0].player2.charge = 0;
                                    pairs[0].player2.isAttacking = false;
                                    pairs[0].player2.isCharging = false;
                                    pairs[0].player2.hitboxActive = false;
                                    pairs[0].player2.currentUpperBodyFrame = 0;
                                    pairs[0].player2.currentUpperBodyAnimation = pairs[0].player2.direction == 1 ? pairs[0].player2.inputVertical + 1 : pairs[0].player2.inputVertical + 13;
                                    pairs[0].player2.sprite.setFillColor(sf::Color::Black);
                                    pairs[0].player2.AddForce((tempVel1 - tempVel2 + pairs[0].player1.attackVelocity * pairs[0].player1.direction) * pairs[0].player1.mass);
                                }
                                pairs[0].player1.velocity = pairs[0].player1.direction * -1 * (pairs[0].player1.attackVelocity / 2);
                                ImpactFrame = 3;
                            }
                            pairs[0].player1.hitboxActive = false;
                        }
                        else if (!p1Hitp2 && p2Hitp1 && !swordHit)
                        {
                            if (pairs[0].player2.attackState == 2)
                            {
                                pairs[0].player2.AddForce((-pairs[0].player1.direction) * 8);
                            }
                            else
                            {
                                if (pairs[0].player2.attackState == pairs[0].player1.blocking && pairs[0].player1.canBlock)
                                {
                                    pairs[0].player2.upperBodyRectSource.left -= 127;
                                    pairs[0].player2.upperBodyColorSprite.setTextureRect(pairs[0].player2.upperBodyRectSource);
                                    pairs[0].player2.upperBodySprite.setTextureRect(pairs[0].player2.upperBodyRectSource);
                                    pairs[0].player2.currentUpperBodyAnimation += 3;
                                    pairs[0].player2.currentUpperBodyFrame = 0;
                                    pairs[0].player1.sprite.setFillColor(sf::Color::Green);
                                    pairs[0].player1.AddForce((pairs[0].player2.attackVelocity / 2) * pairs[0].player2.mass * pairs[0].player2.direction);
                                    pairs[0].player1.blocked = true;
                                }
                                else
                                {
                                    pairs[0].player1.canTurn = true;
                                    pairs[0].player1.canAttack = true;
                                    pairs[0].player1.canBlock = true;
                                    pairs[0].player1.charge = 0;
                                    pairs[0].player1.isAttacking = false;
                                    pairs[0].player1.isCharging = false;
                                    pairs[0].player1.hitboxActive = false;
                                    pairs[0].player1.currentUpperBodyFrame = 0;
                                    pairs[0].player1.currentUpperBodyAnimation = pairs[0].player1.direction == 1 ? pairs[0].player1.inputVertical + 1 : pairs[0].player1.inputVertical + 13;
                                    pairs[0].player1.sprite.setFillColor(sf::Color::Black);
                                    pairs[0].player1.AddForce((tempVel2 - tempVel1 + pairs[0].player2.attackVelocity * pairs[0].player2.direction) * pairs[0].player2.mass);
                                }
                                pairs[0].player2.velocity = pairs[0].player2.direction * -1 * (pairs[0].player2.attackVelocity / 2);
                                ImpactFrame = 3;
                            }
                            pairs[0].player2.hitboxActive = false;
                        }
                        else if (swordHit)
                        {
                            pairs[0].player1.hitboxActive = false;
                            pairs[0].player2.hitboxActive = false;
                            pairs[0].player1.AddForce(((tempVel1 * pairs[0].player1.direction + pairs[0].player1.attackVelocity) * pairs[0].player2.direction * pairs[0].player1.mass + ((tempVel2 * pairs[0].player2.direction + pairs[0].player2.attackVelocity) * pairs[0].player2.direction) * pairs[0].player2.mass) / 2);
                            pairs[0].player2.AddForce(((tempVel1 * pairs[0].player1.direction + pairs[0].player1.attackVelocity) * pairs[0].player1.direction * pairs[0].player1.mass + ((tempVel2 * pairs[0].player2.direction + pairs[0].player2.attackVelocity) * pairs[0].player1.direction) * pairs[0].player2.mass) / 2);
                            ImpactFrame = 3;
                        }

                        if (!pairs[0].player1.dead && !pairs[0].player2.dead && !training)
                        {
                            pairs[0].frame++;
                            dataLine2 =
                                std::to_string(pairs[0].player2.distToEdge2) + "|" +
                                std::to_string(pairs[0].player2.distToEdge1) + "|" +
                                std::to_string(pairs[0].player1.distToEdge2) + "|" +
                                std::to_string(pairs[0].player1.distToEdge1) + "|" +
                                std::to_string(pairs[0].player1.position - pairs[0].player2.position) + "|" +
                                std::to_string(pairs[0].player2.dodges) + "|" +
                                std::to_string(pairs[0].player1.dodges) + "|" +
                                std::to_string((double)pairs[0].player2.blocking) + "|" +
                                std::to_string((double)pairs[0].player1.blocking) + "|" +
                                std::to_string((double)pairs[0].player2.firstActiveAttackFrame - pairs[0].player2.attackFrame) + "|" +
                                std::to_string((double)pairs[0].player1.firstActiveAttackFrame - pairs[0].player1.attackFrame) + "|" +
                                std::to_string((double)pairs[0].player2.firstActiveGrabFrame - pairs[0].player2.grabFrame) + "|" +
                                std::to_string((double)pairs[0].player1.firstActiveGrabFrame - pairs[0].player1.grabFrame) + "|" +
                                std::to_string(pairs[0].player2.charge) + "|" +
                                std::to_string(pairs[0].player1.charge) + "|" +
                                std::to_string(pairs[0].player2.velocity) + "|" +
                                std::to_string(pairs[0].player1.velocity) + "|" +
                                std::to_string(pairs[0].frame) + "|"
                                ;
                            dataLine1 =
                                std::to_string(pairs[0].player1.distToEdge1) + "|" +
                                std::to_string(pairs[0].player1.distToEdge2) + "|" +
                                std::to_string(pairs[0].player2.distToEdge1) + "|" +
                                std::to_string(pairs[0].player2.distToEdge2) + "|" +
                                std::to_string(pairs[0].player1.position - pairs[0].player2.position) + "|" +
                                std::to_string(pairs[0].player1.dodges) + "|" +
                                std::to_string(pairs[0].player2.dodges) + "|" +
                                std::to_string((double)pairs[0].player1.blocking) + "|" +
                                std::to_string((double)pairs[0].player2.blocking) + "|" +
                                std::to_string((double)pairs[0].player1.firstActiveAttackFrame - pairs[0].player1.attackFrame) + "|" +
                                std::to_string((double)pairs[0].player2.firstActiveAttackFrame - pairs[0].player2.attackFrame) + "|" +
                                std::to_string((double)pairs[0].player1.firstActiveGrabFrame - pairs[0].player1.grabFrame) + "|" +
                                std::to_string((double)pairs[0].player2.firstActiveGrabFrame - pairs[0].player2.grabFrame) + "|" +
                                std::to_string(pairs[0].player1.charge) + "|" +
                                std::to_string(pairs[0].player2.charge) + "|" +
                                std::to_string(pairs[0].player1.velocity) + "|" +
                                std::to_string(pairs[0].player2.velocity) + "|" +
                                std::to_string(pairs[0].frame) + "|"
                                ;
                        }
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
                for (int i = 0; i < pairs.size(); i++)
                {
                    if (pairs[0].player1.isAttacking)
                    {
                        window.draw(pairs[0].player2.sprite);
                        window.draw(pairs[0].player2.legsSprite);
                        window.draw(pairs[0].player2.upperBodySprite);
                        window.draw(pairs[0].player2.upperBodyColorSprite);
                        window.draw(pairs[0].player1.sprite);
                        window.draw(pairs[0].player1.legsSprite);
                        window.draw(pairs[0].player1.upperBodySprite);
                        window.draw(pairs[0].player1.upperBodyColorSprite);
                        if (ImpactFrame != 0 && !((pairs[0].player1.currentUpperBodyAnimation >= 9 && pairs[0].player1.currentUpperBodyAnimation <= 11) || pairs[0].player1.currentUpperBodyAnimation >= 21))
                        {
                            window.draw(pairs[0].player1.slashEffectsSprite);
                        }
                    }
                    else
                    {
                        window.draw(pairs[0].player1.sprite);
                        window.draw(pairs[0].player1.legsSprite);
                        window.draw(pairs[0].player1.upperBodySprite);
                        window.draw(pairs[0].player1.upperBodyColorSprite);
                        window.draw(pairs[0].player2.sprite);
                        window.draw(pairs[0].player2.legsSprite);
                        window.draw(pairs[0].player2.upperBodySprite);
                        window.draw(pairs[0].player2.upperBodyColorSprite);
                        if (ImpactFrame != 0 && !((pairs[0].player2.currentUpperBodyAnimation >= 9 && pairs[0].player2.currentUpperBodyAnimation <= 11) || pairs[0].player2.currentUpperBodyAnimation >= 21))
                        {
                            window.draw(pairs[0].player2.slashEffectsSprite);
                        }
                    }
                    for (int i = 0; i < 3; i++)
                    {
                        window.draw(pairs[0].player1.Stamina[i]);
                        window.draw(pairs[0].player2.Stamina[i]);
                    }
                }
                window.display();
            }
            first = true;
            window.clear(sf::Color(51, 173, 255, 0));
            while (view.getCenter().y != -128)
            {
                window.clear(sf::Color(51, 173, 255, 0));
                view.setCenter(view.getCenter().x, view.getCenter().y - 2);
                window.setView(view);
                window.draw(titleSprite);
                window.draw(platformSprite);
                window.draw(vsHumanSprite);
                window.draw(vsCPUSprite);
                window.draw(settingsSprite);
                window.draw(CPULevelSprite);
                window.draw(CPULevelsSprite);
                window.draw(selectorSprite);
                window.draw(player1.sprite);
                window.draw(player1.legsSprite);
                window.draw(player1.upperBodySprite);
                window.draw(player1.upperBodyColorSprite);
                window.draw(player2.sprite);
                window.draw(player2.legsSprite);
                window.draw(player2.upperBodySprite);
                window.draw(player2.upperBodyColorSprite);
                for (int i = 0; i < 3; i++)
                {
                    window.draw(player1.Stamina[i]);
                    window.draw(player2.Stamina[i]);
                }
                window.display();
            }
            player1.TrueReset();
            player2.TrueReset();
            test->Reset();
        }

        if (selectedButton == 2 && sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
        {
            globalTime.restart();
            Settings = true;
            CPULevelSprite.setColor(sf::Color::White);
            CPULevelsSprite.setColor(sf::Color::White);

            vsHumanSprite.setColor(sf::Color::Transparent);
            vsCPUSprite.setColor(sf::Color::Transparent);
            settingsSprite.setColor(sf::Color::Transparent);
            selectedButton = 0;
        }

        if (Settings)
        {
            if (selectedButton == 0)
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && globalTime.getElapsedTime().asSeconds() > 0.2f)
                {
                    if (selectedDifficulty != 7)
                    {
                        selectedDifficulty++;
                        CPULevelsRectSource.left += 74;
                        CPULevelsSprite.setTextureRect(CPULevelsRectSource);
                        delete test;
                        switch (selectedDifficulty)
                        {
                        case 0:
                            test = new HardCodedCPUVariable(200);
                            break;
                        case 1:
                            test = new HardCodedCPUVariable(100);
                            break;
                        case 2:
                            test = new HardCodedCPUVariable(50);
                            break;
                        case 3:
                            test = new HardCodedCPUVariable(10);
                            break;
                        case 4:
                            test = new HardCodedCPUVariable(1);
                            break;
                        case 5:
                            test = new HardCodedCPUImpossiable();
                            break;
                        case 6:
                            break;
                        case 7:
                            break;
                        }
                    }
                    globalTime.restart();
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && globalTime.getElapsedTime().asSeconds() > 0.2f)
                {
                    if (selectedDifficulty != 0)
                    {
                        selectedDifficulty--;
                        CPULevelsRectSource.left -= 74;
                        CPULevelsSprite.setTextureRect(CPULevelsRectSource);
                    }
                    globalTime.restart();
                }
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace))
            {
                Settings = false;
                CPULevelSprite.setColor(sf::Color::Transparent);
                CPULevelsSprite.setColor(sf::Color::Transparent);

                vsHumanSprite.setColor(sf::Color::White);
                vsCPUSprite.setColor(sf::Color::White);
                settingsSprite.setColor(sf::Color::White);
                selectedButton = 0;
            }
        }

        selectorSprite.setPosition(190, selectedButton * 40 - 156);

        window.draw(titleSprite);
        window.draw(platformSprite);
        window.draw(vsHumanSprite);
        window.draw(vsCPUSprite);
        window.draw(settingsSprite);
        window.draw(CPULevelSprite);
        window.draw(CPULevelsSprite);
        window.draw(selectorSprite);
        window.display();
    }
    trainingData2.close();
}
