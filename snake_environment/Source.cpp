#include <iostream>
#include <conio.h>
#include <functional>
#include "snake_env.hpp"
#include "NeuralN.hpp"
#include "LGenetic.h"

using a_t = activation_type;

double fitness(const std::vector<double>& x) {
	return 0;
}

int main() {

	const NeuralN MyNet_static({ 10, 25, 10, 5, 3 }, { a_t::SIGMOID, a_t::SIGMOID, a_t::SIGMOID, a_t::SIGMOID });
	MyNet_static.forward({ 0,0,0,0,0,0,0,0,0,0 });
	LGenetic model(64, 10000, fitness);


	snake_env Env(20, 20);

	while (!Env.is_done()) {
		Env.console_render();
		double input = -1;
		if (_kbhit()) {
			switch (_getch())
			{
			case 'w':
				input = 0;
				break;
			case 's':
				input = 1;
				break;
			case 'd':
				input = 2;
				break;
			case 'a':
				input = 3;
				break;
			default:
				break;
			}
		}
		Env.step(input);
		Sleep(300);
	}

	return 0;
}