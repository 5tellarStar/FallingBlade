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
	double rewards1;
	double rewards2;
	float closestDist;
	float fastestVelocity1;
	float fastestVelocity2;
	BaseCharacter player1;
	BaseCharacter player2;
	NeuralNetwork ai1;
	NeuralNetwork ai2;
};
