#include <iostream>
#include <conio.h>
#include "LGenetic.h"
#include "LMatrix.hpp"
#include "snake_env.hpp"



int input_size = 6;
int output_size = 4;
const int layer1_size = input_size;
const int layer2_size = 20;
const int layer3_size = output_size;
bool show = false;


template <typename T, typename A>
int arg_max(std::vector<T, A> const& vec) {
	return static_cast<int>(std::distance(vec.begin(), max_element(vec.begin(), vec.end())));
}

template <typename T, typename A>
int arg_min(std::vector<T, A> const& vec) {
	return static_cast<int>(std::distance(vec.begin(), min_element(vec.begin(), vec.end())));
}


double loss(std::vector<double>& x, std::string file_validation) {

	return 0;
}

auto neiron_forward(LMatrix& inVec, LMatrix& weits1, LMatrix& weits2) {
	LMatrix first, second;
	first = inVec * weits1;
	first.sigmoid();
	second = first * weits2;
	second.sigmoid();
	auto rez = second.getLine();
	double sum = 0;
	for (auto i : rez) sum += exp(i);
	for (auto& i : rez) i = exp(i) / sum;
	return rez;
}


double fitness(std::vector<double>& x) {
	LMatrix weits1(layer1_size, layer2_size);
	LMatrix weits2(layer2_size, layer3_size);
	std::vector<double> line1;
	std::copy(x.begin(), x.begin() + layer1_size * layer2_size, std::back_inserter(line1));
	std::vector<double> line2;
	std::copy(x.begin() + layer1_size * layer2_size, x.end(), std::back_inserter(line2));
	weits1.setLine(line1);
	weits2.setLine(line2);

	snake_env Env(20, 20);
	int steps = 0;

	while (!Env.is_done()) {
		++steps;
		if (steps > 400) break;
		/*if (show) {
			Env.console_render();

			Sleep(3000);
		}*/
		LMatrix sample(1, layer1_size);
		std::vector<double> observation = Env.observe();
		std::vector<double> input;
		std::copy(observation.begin(), observation.begin() + input_size, std::back_inserter(input));
		if (show) {
			Env.console_render();
			std::cout << "observation: ";
			for (auto& i : observation) std::cout << i << " ";
			std::cout << std::endl;
			std::cout << "input: ";
			for (auto& i : input) std::cout << i << " ";
			std::cout << std::endl;
			Sleep(300);
		}
		sample.setLine(input);
		auto ans = neiron_forward(sample, weits1, weits2);
		Env.step(arg_max(ans));
	}
	double sorce = Env.reward();
	//return -(((double)steps + (pow(2, sorce) + 500 * pow(sorce, 2.1)) - (0.25 * pow(steps, 1.3) * pow(sorce, 1.2))) * 100);
	return -(Env.reward() + steps);
}


void do_it() {
	int gene_length = layer1_size * layer2_size + layer2_size * layer3_size;
	int pop_size = 512;
	LGenetic Model
	(
		pop_size,
		gene_length,
		fitness
	);

	Model.rand_population_normal();
	Model.set_crossover(LGenetic::SPBX);
	Model.set_mutation(LGenetic::AM);
	Model.file_validation = "heh";
	Model.set_loss(loss);
	Model.learn(500);


	auto best = Model.best_gene();
	system("cls");
	show = true;
	fitness(best);
	loss(best, "heh");
	show = false;
}

int main() {
	clock_t start = clock();

	do_it();
	clock_t now = clock();
	std::cout << (double)(now - start) / CLOCKS_PER_SEC << " sec\n";
	return 0;
}

