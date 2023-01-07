#include <iostream>
#include <conio.h>
#include <functional>
#include "snake_env.hpp"
#include "NeuralN.hpp"
#include "LGenetic.h"
#include "MCTS_copy.hpp"




template <typename T, typename A>
int arg_max(std::vector<T, A> const& vec) {
	return static_cast<int>(std::distance(vec.begin(), max_element(vec.begin(), vec.end())));
}

template <typename T, typename A>
int arg_min(std::vector<T, A> const& vec) {
	return static_cast<int>(std::distance(vec.begin(), min_element(vec.begin(), vec.end())));
}

const int g_N = 6;
const snake_env g_Env(g_N, g_N);
NeuralN g_MyNet(
					{ (int)g_Env.observe_hard().size(), 4 }, 
					{ activation_type::RELU }
					  );
const NeuralN g_MyNet_2(
	{ (int)g_Env.observe_hard().size(), 1 },
	{ activation_type::RELU }
);

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

class Final_env : public IEnviroment {
public:
	Final_env(snake_env _sn_env, const NeuralN& _nn, const NeuralN& _nn_static) : sn_env(_sn_env), nn(_nn), nn_static(_nn_static) {}

	void step(const int action) override {
		sn_env.step(action);
	}
	int actions_number() const override {
		//if (sn_env.is_done())
			//return 0;
		return 4;
	}
	std::pair<double, std::vector<int>> evaluate(const int action) const override {
		if (sn_env.score_() == g_N * g_N)
			return { 99999, {0, 1, 2, 3} };

		if (sn_env.is_done())
			return { -1, {0, 1, 2, 3} };
		//return (double)sn_env.score_() - (double)sn_env.steps_without_fruit() / 10;
		auto obs = sn_env.observe_hard();
		//auto choosed_actions = find_2_argmax(nn_static.forward(obs));
		return { nn.forward(obs)[0], {0, 1, 2, 3} };
	}
	std::shared_ptr<IEnviroment> clone() const override {
		return (std::shared_ptr<IEnviroment>)(new Final_env(*this));
	}

	std::vector<int> find_2_argmax(const std::vector<double>& in_v) const {
		double M = -9999999;
		int p_M = -9999999;
		for (int k = 0; k < in_v.size(); ++k) {
			if (in_v[k] > M) {
				M = in_v[k];
				p_M = k;
			}
		}
		M = -9999999;
		int p_M_2 = -100;
		for (int k = 0; k < in_v.size(); ++k) {
			if (k != p_M && in_v[k] > M) {
				M = in_v[k];
				p_M_2 = k;
			}
		}
		return { p_M, p_M_2 };
	}

private:
	snake_env sn_env;
	const NeuralN& nn;
	const NeuralN& nn_static;
};

double fitness(const std::vector<double>& x) {
	//srand(0);
	const auto local_Net_static = g_MyNet;
	auto local_Net = g_MyNet_2;
	local_Net.read_weitghs(x);

	snake_env Env = g_Env;

	while (!Env.is_done()) {
		Final_env final_env(Env, local_Net, local_Net_static);
		MCTS mcts(100, final_env);
		auto res = mcts.run();

		//auto res = local_Net.forward(Env.observe_hard());

		Env.step(get_max_action(res));
	}
	return -Env.score_();
}

double fitness_n_times(const std::vector<double>& x) {
	const int n = 5;
	double res = 0;
	for (int i = 0; i < n; ++i) {
		res += fitness(x);
	}
	return res / n;
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
	//std::ifstream aaa("best_latest_static_.txt");
	//g_MyNet.read_weitghs(aaa);
	LGenetic model(128, g_MyNet.paramsNumber(), fitness_n_times);
	model.rand_population_uniform();
	model.set_crossover(LGenetic::SPBX);
	model.set_mutation(LGenetic::AM);
	model.set_loss(loss);
	/*std::vector<double> bob;
	std::ifstream a("best_latest_value_.txt");
	for (int i = 0; i < g_MyNet_2.paramsNumber(); ++i) {
		double b;
		a >> b;
		bob.push_back(b);
	}*/
	//model.pop[0] = bob;
	model.learn(5000);
	auto best = model.best_gene();

	//std::ifstream fin("best.txt");
	//for (int i = 0; i < g_MyNet.paramsNumber(); ++i) 
	//	fin >> best[i];
	
	while (true) {
		int a;
		std::cin >> a;
		show(best);
	}

	return 0;
}
