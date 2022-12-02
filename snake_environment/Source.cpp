#include <iostream>
#include <conio.h>
#include <functional>
#include "snake_env.hpp"
#include "NeuralN.hpp"
#include "LGenetic.h"
#include "MCTS_copy.hpp"



int get_max_action(std::vector<std::tuple<int, double, int>>& res) {
	double max_v = -99999999999;
	int max_ind = -1;
	for (int i = 0; i < res.size(); ++i) {
		if (max_v < std::get<1>(res[i])) {
			max_v = std::get<1>(res[i]);
			max_ind = i;
		}
	}
	return std::get<0>(res[max_ind]);
}

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
const NeuralN g_MyNet({ (int)g_Env.observe_light().size(), 10, 1 }, { activation_type::SIGMOID, activation_type::SIGMOID });

class Final_env : public IEnviroment {
public:
	Final_env(snake_env _sn_env, const NeuralN& _nn) : sn_env(_sn_env), nn(_nn) {}

	void step(const int action) override {
		sn_env.step(action);
	}
	int actions_number() const override {
		//if (sn_env.is_done())
			//return 0;
		return 3;
	}
	double evaluate() const override {
		if (sn_env.is_done())
			return 0;
		//return sn_env.snake_len() + 1 - (double)sn_env.steps_without_fruit() / 10;
		auto obs = sn_env.observe_light();
		for (int i = 0; i < 9; ++i) {
			if (obs[i] != -1)
				obs[i] = 1;
			else
				obs[i] = 0;
		}
		
		return nn.forward(obs)[0];
	}
	std::shared_ptr<IEnviroment> clone() const override {
		return (std::shared_ptr<IEnviroment>)(new Final_env(*this));
	}

private:
	snake_env sn_env;
	const NeuralN& nn;
};

double fitness(const std::vector<double>& x) {
	//srand(0);
	auto local_Net = g_MyNet;
	local_Net.read_weitghs(x);

	snake_env Env = g_Env;

	while (!Env.is_done()) {
		Final_env final_env(Env, local_Net);
		MCTS mcts(100, final_env);
		auto res = mcts.run();

		Env.step(get_max_action(res));
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
	srand(0);
	auto local_Net = g_MyNet;
	local_Net.read_weitghs(x);
	local_Net.write_weitghs("best.txt");

	snake_env Env = g_Env;
	system("cls");
	while (!Env.is_done()) {
		Final_env final_env(Env, local_Net);
		MCTS mcts(100, final_env);
		auto res = mcts.run();

		Env.step(get_max_action(res));
		Env.console_render();
		Sleep(100);
	}
	
	return Env.snake_len();
}

int main() {
	LGenetic model(64, g_MyNet.paramsNumber(), fitness_x10);
	model.set_crossover(LGenetic::SBX);
	model.set_mutation(LGenetic::AM);
	model.set_loss(loss);
	model.learn(100);
	auto best = model.best_gene();
	show(best);

	return 0;
}
