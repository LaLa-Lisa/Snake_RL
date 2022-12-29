#define PYPLT
#include <iostream>
#include <conio.h>
#include <functional>
#include "snake_env.hpp"
#include "NeuralN.hpp"
#include "LGenetic.hpp"




template <typename T, typename A>
int arg_max(std::vector<T, A> const& vec) {
	return static_cast<int>(std::distance(vec.begin(), max_element(vec.begin(), vec.end())));
}

template <typename T, typename A>
int arg_min(std::vector<T, A> const& vec) {
	return static_cast<int>(std::distance(vec.begin(), min_element(vec.begin(), vec.end())));
}

const int g_N = 10;
const snake_env g_Env(g_N, g_N);
//const NeuralN g_MyNet(
//					{ (int)g_Env.observe_hard().size(), 20, 12, 4 }, 
//					{ activation_type::RELU, activation_type::RELU, activation_type::RELU }
//					  );
const NeuralN g_MyNet(
	{ (int)g_Env.observe_hard().size(), 4 },
	{ activation_type::SIGMOID }
);



double fitness(const std::vector<double>& x) {
	//srand(0);
	auto local_Net = g_MyNet;
	local_Net.read_weitghs(x);

	snake_env Env = g_Env;

	while (!Env.is_done()) {
		auto res = local_Net.forward(Env.observe_hard());

		Env.step(arg_max(res));
	}
	return -Env.snake_len();
}


double show(const std::vector<double>& x) {
	srand(10);
	auto local_Net = g_MyNet;
	local_Net.read_weitghs(x);
	local_Net.write_weitghs("best.txt");
	/*auto a = std::ifstream("best.txt");
	local_Net.read_weitghs(a);*/

	snake_env Env = g_Env;
	system("cls");
	while (!Env.is_done()) {
		auto res = local_Net.forward(Env.observe_hard());

		Env.step(arg_max(res));

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

		//Sleep(100);
	}
	
	return Env.snake_len();
}

int main() {
	LGenetic model(2000, g_MyNet.paramsNumber(), fitness);
	model.rand_population_uniform();
	model.set_crossover(LGenetic::SPBX);
	model.set_mutation(LGenetic::AM);
	model.enable_multiprocessing(2);
	model.enable_avarage_fitness(15);
	model.learn(100);
	model.show_plt_avarage();
	auto best = model.best_gene();

	/*std::ifstream fin("best.txt");
	for (int i = 0; i < g_MyNet.paramsNumber(); ++i) 
		fin >> best[i];*/
	
	while (true) {
		int a;
		std::cin >> a;
		show(best);
	}

	return 0;
}
