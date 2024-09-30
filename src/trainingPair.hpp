#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <array>
#include <list>
#include <vector>
#include "baseCharacter.hpp"
#include "neuralNetwork.hpp"

#pragma once

class TrainingPair
{
public:
    bool done = false;
    double rewards = 0;
	BaseCharacter player1 = BaseCharacter(false);
	BaseCharacter player2 = BaseCharacter(true);
    NeuralNetwork ai = NeuralNetwork(std::vector<int>{13, 20, 20, 20, 7});
	TrainingPair()
	{

	}

private:

};

