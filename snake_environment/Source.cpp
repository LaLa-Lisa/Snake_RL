#include <iostream>
#include <conio.h>
#include <functional>
#include "snake_env.hpp"
#include "NeuralN.hpp"
#include "LGenetic.h"


int argmax(const std::vector<double>& v) {
	double m = -99999999;
	int res = -1;
	for (int i = 0; i < v.size(); ++i) {
		if (v[i] > m) {
			m = v[i];
			res = i;
		}
	}
	if (res == -1)
		throw "";
	return res;
}


const int g_N = 5;
const snake_env g_Env(g_N, g_N);
const NeuralN g_MyNet({ (int)g_Env.observe_light().size(), 50, 3 }, { activation_type::RELU, activation_type::SOFTMAX });


double fitness(const std::vector<double>& x) {
	//srand(0);
	auto local_Net = g_MyNet;
	local_Net.read_weitghs(x);

	snake_env Env = g_Env;

	int steps_limit = g_N * g_N;
	int prev_lenght = 0;
	while (!Env.is_done()) {
		steps_limit--;
		if (steps_limit <= 0)
			break;

		auto observe = Env.observe_light();
		if (observe.end()[-1] > prev_lenght) {
			steps_limit = 2 * g_N * g_N;
			prev_lenght = observe.end()[-1];
		}
		for (int i = 0; i < 6; ++i)
			observe[i] /= g_N;
		auto res = local_Net.forward(observe);

		int input = argmax(res);

		Env.step(input);
	}

	return -Env.snake_len();
}

double fitness_x10(const std::vector<double>& x) {
	double res = 0;
	for (int i = 0; i < 5; ++i) {
		res += fitness(x);
	}
	return res / 5;
}

double loss(std::vector<double>& x, std::string s) {
	static double t1 = clock();
	double t2 = clock();
	return (t2 - t1) / CLOCKS_PER_SEC;
}

double show(const std::vector<double>& x) {
	srand(0);
	auto local_Net = g_MyNet;
	local_Net.read_weitghs(x);
	local_Net.write_weitghs("best.txt");

	snake_env Env = g_Env;
	system("cls");
	int steps_counter = 0;
	while (!Env.is_done()) {
		steps_counter++;
		if (steps_counter > 100000)
			break;
		//Env.console_render();

		auto observe = Env.observe_light();
		for (auto& i : observe) {
			std::cout << i << ' ';
		}
		for (int i = 0; i < 6; ++i)
			observe[i] /= g_N;
		auto res = local_Net.forward(observe);

		int input = argmax(res);

		Env.step(input);
		Sleep(300);
		Env.console_render();
	}
	
	return Env.snake_len();
}

int main() {
	LGenetic model(64*4, g_MyNet.paramsNumber(), fitness_x10);
	model.set_crossover(LGenetic::SBX);
	model.set_mutation(LGenetic::AM);
	model.set_loss(loss);
	model.learn(1000);
	auto best = model.best_gene();
	show(best);

	return 0;
}
