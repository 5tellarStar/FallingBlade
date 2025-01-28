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
bool training = true;
double highestReward = -1;
int frame = 0;
int selectedButton = 0;
int selectedDifficulty = 0;
bool Settings = false;
std::vector<TrainingPair> pairs;
NeuralNetwork bestAi(std::vector<int>{13, 20, 20, 20, 7});

std::default_random_engine rnd{ std::random_device{}() };
std::uniform_real_distribution<double> dist(-0.2, 0.2);

int main()
{
    for (int i = 0; i < 3; i++)
    {
        pairs.push_back({ true,0,0,512,0,0,BaseCharacter(false),BaseCharacter(true),NeuralNetwork(std::vector<int>{13, 20, 20, 20, 7}),NeuralNetwork(std::vector<int>{13, 20, 20, 20, 7})});
        pairs[i].ai1.Randomize();
        pairs[i].ai2.Randomize();
    }
    bestAi.Randomize();

    auto window = sf::RenderWindow{ { 512u, 128u }, "Falling Blade" };
    BaseCharacter player1(false);
    BaseCharacter player2(true);

    CPU* test = new HardCodedCPUVariable(2);

    sf::Clock globalTime;
    sf::Clock trainingTime;

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
    sf::IntRect CPULevelsRectSource = sf::IntRect(74*0, 0, 74, 17);
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
            if (!(selectedButton == 2|| (Settings && selectedButton == 0)))
            {
                selectedButton++;
            }
            globalTime.restart();
        }



        if ((selectedButton == 0 || selectedButton == 1)&& sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) && !Settings)
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
                        if (event.key.code == player1.up)
                            player1.upIsPressed = true;
                        if (event.key.code == player1.down)
                            player1.downIsPressed = true;
                        if (event.key.code == player1.right)
                            player1.rightIsPressed = true;
                        if (event.key.code == player1.left)
                            player1.leftIsPressed = true;
                        if (event.key.code == player1.attack)
                            player1.attackIsPressed = true;
                        if (event.key.code == player1.dodge)
                            player1.dodgeIsPressed = true;
                        if (event.key.code == player1.skill)
                            player1.skillIsPressed = true;

                        if (event.key.code == player2.up)
                            player2.upIsPressed = true;
                        if (event.key.code == player2.down)
                            player2.downIsPressed = true;
                        if (event.key.code == player2.right)
                            player2.rightIsPressed = true;
                        if (event.key.code == player2.left)
                            player2.leftIsPressed = true;
                        if (event.key.code == player2.attack)
                            player2.attackIsPressed = true;
                        if (event.key.code == player2.dodge)
                            player2.dodgeIsPressed = true;
                        if (event.key.code == player2.skill)
                            player2.skillIsPressed = true;
                    }
                    if (event.type == sf::Event::KeyReleased)
                    {
                        if (event.key.code == player1.up)
                            player1.upIsPressed = false;
                        if (event.key.code == player1.down)
                            player1.downIsPressed = false;
                        if (event.key.code == player1.right)
                            player1.rightIsPressed = false;
                        if (event.key.code == player1.left)
                            player1.leftIsPressed = false;
                        if (event.key.code == player1.attack)
                            player1.attackIsPressed = false;
                        if (event.key.code == player1.dodge)
                            player1.dodgeIsPressed = false;
                        if (event.key.code == player1.skill)
                            player1.skillIsPressed = false;

                        if (event.key.code == player2.up)
                            player2.upIsPressed = false;
                        if (event.key.code == player2.down)
                            player2.downIsPressed = false;
                        if (event.key.code == player2.right)
                            player2.rightIsPressed = false;
                        if (event.key.code == player2.left)
                            player2.leftIsPressed = false;
                        if (event.key.code == player2.attack)
                            player2.attackIsPressed = false;
                        if (event.key.code == player2.dodge)
                            player2.dodgeIsPressed = false;
                        if (event.key.code == player2.skill)
                            player2.skillIsPressed = false;
                    }
                }

                    if (!player1.dead && !player2.dead)
                    {
                        player1.Input();
                        player2.Input();
                    }
                    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) && player1.currentLegsFrame == 0 && player1.currentUpperBodyFrame == 0 && player2.currentLegsFrame == 0 && player2.currentUpperBodyFrame == 0)
                    {
                        player1.Reset();
                        player2.Reset();
                        test->Reset();
                        frame = 0;
                    }

                    if (globalTime.getElapsedTime().asSeconds() > (1.f / 24.f) && ImpactFrame == 0)
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
                        if (!player1.dead && !player2.dead && fightingCPU)
                        {
                            player2.Input((test->inputs(player1, player2,frame)));
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
                                player2.AddForce((-player1.direction) * 8);
                            }
                            else
                            {
                                if (player1.attackState == player2.blocking && player2.canBlock)
                                {

                                    player1.upperBodyRectSource.left -= 127;
                                    player1.upperBodyColorSprite.setTextureRect(player1.upperBodyRectSource);
                                    player1.upperBodySprite.setTextureRect(player1.upperBodyRectSource);
                                    player1.currentUpperBodyAnimation += 3;
                                    player1.currentUpperBodyFrame = 0;
                                    player2.sprite.setFillColor(sf::Color::Green);
                                    player2.AddForce((player1.attackVelocity / 2) * player1.mass * player1.direction);
                                    player2.blocked = true;
                                }
                                else
                                {
                                    player2.canTurn = true;
                                    player2.canAttack = true;
                                    player2.canBlock = true;
                                    player2.charge = 0;
                                    player2.isAttacking = false;
                                    player2.isCharging = false;
                                    player2.hitboxActive = false;
                                    player2.currentUpperBodyFrame = 0;
                                    player2.currentUpperBodyAnimation = player2.direction == 1 ? player2.inputVertical + 1 : player2.inputVertical + 13;
                                    player2.sprite.setFillColor(sf::Color::Black);
                                    player2.AddForce((tempVel1 - tempVel2 + player1.attackVelocity * player1.direction) * player1.mass);
                                }
                                player1.velocity = player1.direction * -1 * (player1.attackVelocity/2);
                                ImpactFrame = 3;
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
                                if (player2.attackState == player1.blocking && player1.canBlock)
                                {
                                    player2.upperBodyRectSource.left -= 127;
                                    player2.upperBodyColorSprite.setTextureRect(player2.upperBodyRectSource);
                                    player2.upperBodySprite.setTextureRect(player2.upperBodyRectSource);
                                    player2.currentUpperBodyAnimation += 3;
                                    player2.currentUpperBodyFrame = 0;
                                    player1.sprite.setFillColor(sf::Color::Green);
                                    player1.AddForce((player2.attackVelocity / 2) * player2.mass * player2.direction);
                                    player1.blocked = true;
                                }
                                else
                                {
                                    player1.canTurn = true;
                                    player1.canAttack = true;
                                    player1.canBlock = true;
                                    player1.charge = 0;
                                    player1.isAttacking = false;
                                    player1.isCharging = false;
                                    player1.hitboxActive = false;
                                    player1.currentUpperBodyFrame = 0;
                                    player1.currentUpperBodyAnimation = player1.direction == 1 ? player1.inputVertical + 1 : player1.inputVertical + 13;
                                    player1.sprite.setFillColor(sf::Color::Black);
                                    player1.AddForce((tempVel2 - tempVel1 + player2.attackVelocity * player2.direction) * player2.mass);
                                }
                                player2.velocity = player2.direction * -1 * (player2.attackVelocity/2);
                                ImpactFrame = 3;
                            }
                            player2.hitboxActive = false;
                        }
                        else if (swordHit)
                        {
                            player1.hitboxActive = false;
                            player2.hitboxActive = false;
                            player1.AddForce(((tempVel1 * player1.direction + player1.attackVelocity) * player2.direction * player1.mass + ((tempVel2 * player2.direction + player2.attackVelocity) * player2.direction) * player2.mass)/2);
                            player2.AddForce(((tempVel1 * player1.direction + player1.attackVelocity) * player1.direction * player1.mass + ((tempVel2 * player2.direction + player2.attackVelocity) * player1.direction) * player2.mass)/2);
                            ImpactFrame = 3;
                        }

                        if (!player1.dead && !player2.dead)
                        {
                            frame++;
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
                                std::to_string(player1.velocity) + "|" +
                                std::to_string(frame) + "|"
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
                                std::to_string(player2.velocity) + "|" +
                                std::to_string(frame) + "|"
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
                    if (player1.isAttacking)
                    {
                        window.draw(player2.sprite);
                        window.draw(player2.legsSprite);
                        window.draw(player2.upperBodySprite);
                        window.draw(player2.upperBodyColorSprite);
                        window.draw(player1.sprite);
                        window.draw(player1.legsSprite);
                        window.draw(player1.upperBodySprite);
                        window.draw(player1.upperBodyColorSprite);
                        if (ImpactFrame != 0 && !((player1.currentUpperBodyAnimation >= 9 && player1.currentUpperBodyAnimation <= 11) || player1.currentUpperBodyAnimation >= 21))
                        {
                            window.draw(player1.slashEffectsSprite);
                        }
                        for (int i = 0; i < 3; i++)
                        {
                            window.draw(player1.Stamina[i]);                            
                            window.draw(player2.Stamina[i]);
                        }
                    }
                    else
                    {
                        window.draw(player1.sprite);
                        window.draw(player1.legsSprite);
                        window.draw(player1.upperBodySprite);
                        window.draw(player1.upperBodyColorSprite);
                        window.draw(player2.sprite);
                        window.draw(player2.legsSprite);
                        window.draw(player2.upperBodySprite);
                        window.draw(player2.upperBodyColorSprite);
                        if (ImpactFrame != 0 && !((player2.currentUpperBodyAnimation >= 9 && player2.currentUpperBodyAnimation <= 11) || player2.currentUpperBodyAnimation >= 21))
                        {
                            window.draw(player2.slashEffectsSprite);
                        }
                        for (int i = 0; i < 3; i++)
                        {
                            window.draw(player1.Stamina[i]);
                            window.draw(player2.Stamina[i]);
                        }
                    }
                window.display();
            }
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
                            test = new HardCodedCPUVariable(1000);
                            break;
                        case 1:
                            test = new HardCodedCPUVariable(5);
                            break;
                        case 2:
                            test = new HardCodedCPUVariable(3);
                            break;
                        case 3:
                            test = new HardCodedCPUVariable(2);
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
