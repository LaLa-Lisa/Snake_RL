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
//		{ (int)g_Env.observe_hard().size(), 24, 12, 4 }, 
//		{ activation_type::RELU, activation_type::RELU, activation_type::SIGMOID }
//					  );
const NeuralN g_MyNet(
	{ (int)g_Env.observe_hard().size(), 4 },
	{ activation_type::SIGMOID }
);


double fitness(const std::vector<double>& x) {
	//srand(0);
	auto local_Net = g_MyNet;
	local_Net.read_weitghs(x);

	int ran = rand() % 5 + 1;
	int fild_size =  ran * 2 + 4;
	snake_env Env(fild_size, fild_size);

	while (!Env.is_done()) {
		auto res = local_Net.forward(Env.observe_hard());

		Env.step(arg_max(res));
	}
	return -Env.score_val() / (double)(fild_size * fild_size) * 100.0;
}
double fitness_with_size(const std::vector<double>& x, int fild_size = 5) {
	//srand(0);
	auto local_Net = g_MyNet;
	local_Net.read_weitghs(x);

	snake_env Env(fild_size, fild_size);

	while (!Env.is_done()) {
		auto res = local_Net.forward(Env.observe_hard());

		Env.step(arg_max(res));
	}
	return -Env.score_val() / (double)(fild_size * fild_size) * 100.0;
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

void training(int times) {
	LGenetic model(128, g_MyNet.paramsNumber(), fitness);
	model.rand_population_uniform();
	model.set_crossover(LGenetic::SPBX);
	model.set_mutation(LGenetic::AM);
	model.enable_multiprocessing(3);
	model.enable_avarage_fitness(15);
	model.learn(times);
#ifdef PYPLT
	model.show_plt_avarage();
#endif

	auto best = model.best_gene();

	while (true) {
		int a;
		std::cin >> a;
		show(best);
	}
}

void retraining(int times) {
	LGenetic model(128, g_MyNet.paramsNumber(), fitness);
	model.rand_population_uniform();
	auto gene = model.best_gene();

	std::ifstream fin("best.txt");
	for (int i = 0; i < g_MyNet.paramsNumber(); ++i)
		fin >> gene[i];
	model.read_gene(gene);
	model.set_crossover(LGenetic::SPBX);
	model.set_mutation(LGenetic::AM);
	model.enable_multiprocessing(3);
	model.enable_avarage_fitness(15);
	model.learn(times);
#ifdef PYPLT
	model.show_plt_avarage();
#endif

	auto best = model.best_gene();

	while (true) {
		int a;
		std::cin >> a;
		show(best);
	}
}

void test() {
	std::vector<double> gene(g_MyNet.paramsNumber());

	std::ifstream fin("best.txt");
	for (int i = 0; i < g_MyNet.paramsNumber(); ++i)
		fin >> gene[i];

	double summ = 0;
	for (int i = 0; i < 100; ++i) {
		double res = 0;
		for (int j = 5; j < 13; ++j) {
			res += (double)fitness_with_size(gene, j);
		}
		res /= 8;

		summ += res;
		std::cout << '\n' << i + 1 << '\t' << res << '\t' << summ / (i + 1) << '\n';
	}
}

int main() {
	
	training(100);
	//retraining(100);
	test();
	return 0;
}
