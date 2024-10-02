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


struct TrainingPair
{
	bool done;
	double rewards;
	BaseCharacter player1;
	BaseCharacter player2;
	NeuralNetwork ai;
};
