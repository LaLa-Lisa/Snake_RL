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
const NeuralN g_MyNet({ (int)g_Env.observe_hard().size(), 20, 12, 4 }, { activation_type::RELU, activation_type::RELU, activation_type::RELU });



double fitness(const std::vector<double>& x) {
	//srand(0);
	auto local_Net = g_MyNet;
	local_Net.read_weitghs(x);

	snake_env Env = g_Env;

	while (!Env.is_done()) {
		auto res = local_Net.forward(Env.observe_hard());

		Env.step(argmax(res));
	}
	return -Env.snake_len();
}

double fitness_x10(const std::vector<double>& x) {
	double res = 0;
	for (int i = 0; i < 10; ++i) {
		res += fitness(x);
	}
	return res / 10;
}

double loss(std::vector<double>& x, std::string s) {
	static double t1 = clock();
	double t2 = clock();
	return (t2 - t1) / CLOCKS_PER_SEC;
}

double show(const std::vector<double>& x) {
	srand(02);
	auto local_Net = g_MyNet;
	local_Net.read_weitghs(x);
	local_Net.write_weitghs("best.txt");
	/*auto a = std::ifstream("best.txt");
	local_Net.read_weitghs(a);*/

	snake_env Env = g_Env;
	system("cls");
	while (!Env.is_done()) {
		auto res = local_Net.forward(Env.observe_hard());

		Env.step(argmax(res));

		Env.console_render();
		int ii = 0;
		for (auto& i : Env.observe_hard()) {
			std::cout << i << ' ';
			ii++;
			if (ii % 4 == 0) {
				if (ii % 8 == 0)
					std::cout << '\n';
				else 
					std::cout << '\t';
			}
		}

		Sleep(100);
	}
	
	return Env.snake_len();
}

int main() {
	LGenetic model(2000, g_MyNet.paramsNumber(), fitness_x10);
	model.rand_population_uniform();
	model.set_crossover(LGenetic::SPBX);
	model.set_mutation(LGenetic::AM);
	model.set_loss(loss);
	model.learn(2000);
	auto best = model.best_gene();
	
	while (true) {
		int a;
		std::cin >> a;
		show(best);
	}

	return 0;
}
