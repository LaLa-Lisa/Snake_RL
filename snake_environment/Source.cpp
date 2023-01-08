#define PYPLT
#define SFML_USE
#include <iostream>
#include <conio.h>
#include <functional>
#include "snake_env.hpp"
#include "NeuralN.hpp"
#include "LGenetic.hpp"
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
					{ activation_type::SIGMOID }
					  );
const NeuralN g_MyNet_2(
	{ (int)g_Env.observe_hard().size() + 1, 1 },
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
	std::pair<double, std::vector<double>> evaluate(const int action) const override {
		if (sn_env.score_() == sn_env.wight * sn_env.heght)
			return { 999999, {999999, 999999, 999999, 999999} };

		if (sn_env.is_done())
			return { -1, {-1, -1, -1, -1} };
		//return (double)sn_env.score_() - (double)sn_env.steps_without_fruit() / 10;
		auto obs = sn_env.observe_hard();
		//auto choosed_actions = find_2_argmax(nn_static.forward(obs));
		//obs.push_back((double)sn_env.score_() / 36);
		return { 1, nn.forward(obs) };
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
	auto local_Net = g_MyNet;
	local_Net.read_weitghs(x);
	//int r = rand() % 8 + 5;
	snake_env Env = g_Env;

	while (!Env.is_done()) {
		Final_env final_env(Env, local_Net, local_Net_static);
		MCTS mcts(50, final_env);
		auto res = mcts.run();

		//auto res = local_Net.forward(Env.observe_hard());

		Env.step(get_max_action(res));
	}
	return -Env.score_();
}

double fitness_n_times(const std::vector<double>& x) {
	const int n = n;
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

double show(const std::vector<double>& x, int p) {
	//srand(0);
	//auto local_Net = g_MyNet;
	//local_Net.read_weitghs(x);
	//local_Net.write_weitghs("best.txt");
	/*auto a = std::ifstream("best.txt");
	local_Net.read_weitghs(a);*/
	auto a = std::ifstream("best_latest_.txt130__");
	auto local_Net_static = g_MyNet;
	//local_Net_static.read_weitghs(a);
	auto local_Net = g_MyNet;
	local_Net.read_weitghs(a);

	snake_env Env(p,p);

#ifdef SFML_USE
	sf::RenderWindow window(sf::VideoMode(30 * g_N, 30 * g_N), "Snake game");
	Env.set_screen(&window);
#endif
	int e = 0;
	while (!Env.is_done()) {
		e++;
		Final_env final_env(Env, local_Net, local_Net_static);
		MCTS mcts(50, final_env);
		auto res = mcts.run();

		//auto res = local_Net.forward(Env.observe_hard());

		Env.step(get_max_action(res));
		Env.render();
		std::cout << mcts.max_deep_valye << '\n';
	}
	//std::cout << e << "\n\n";
	//std::cout << p << ' ' << (double)Env.score_() / (p * p) << '|';
	return Env.score_();
}

void training(int times) {
	LGenetic model(128, g_MyNet.paramsNumber(), fitness);
	model.rand_population_uniform();
	model.set_crossover(LGenetic::SPBX);
	model.set_mutation(LGenetic::AM);
	model.enable_multiprocessing(10);
	model.enable_avarage_fitness(5);
	model.learn(times);
#ifdef PYPLT
	model.show_plt_avarage();
	model.show_plt_avarage2();
#endif

	auto best = model.best_gene();

	while (true) {
		int a;
		std::cin >> a;
		std::cin >> a;
		std::cin >> a;
		show({}, 10);
		//double summ = 0;
		//for (int i = 0; i < 1000; ++i) {
		//	double res_0 = 0;
		//	double res_1 = 0;
		//	res_0 += (double)show({}, 6) / 36;
		//	res_0 += (double)show({}, 8) / 64;
		//	res_0 += (double)show({}, 10) / 100;
		//	res_0 += (double)show({}, 12) / 144;
		//	res_0 /= 4;
		//	res_1 += (double)show({}, 5) / 25;
		//	res_1 += (double)show({}, 7) / 49;
		//	res_1 += (double)show({}, 9) / 81;
		//	res_1 += (double)show({}, 11) / 121;
		//	res_1 /= 4;
		//	//for (int j = 5; j < 13; ++j) {
		//		//res += (double)show({}, j) / (j*j);
		//	//}
		//	double res = (res_0 + res_1) / 2;
		//	//auto res = show({}, 5);
		//	summ += res;
		//	std::cout <<'\n' << i + 1 << '\t' << res << '\t' << summ / (i + 1) << " Even " << res_0 << " Odd " << res_1 << '\n';
		//}
	}
}

int main() {
	training(2000);

	return 0;
}
