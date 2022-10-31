#include <iostream>
#include <vector>

//#include "MCTS.hpp"
#include "MCTS_copy.hpp"

#include "snake_env.hpp"

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

int main() {
	double t1 = clock();
	int ii = time(0);
	srand(ii);
	snake_env env = snake_env(5, 5);
	snake_env env2 = env;
	//env.step(1);
	//env.step(0);
	//env.step(2);
	//env.backstep(2);
	//env.backstep(0);
	//env.backstep(1);
	//if (env != env2) {
		//throw "";
	//}
	double reward = 0;
	for (int i = 0; i < 300000; ++i) {
		//std::cout << reward << ' ';
		//srand(rand());
		if (!env.is_done()) {
			env.console_render();
		}
		else {
			std::cout << "sdfsadf";
			Sleep(5000);
			env.console_render();
			exit(0);
		}
		//Sleep(0);
		snake_env newEnv = env;
		MCTS mcts(1000, newEnv);
		auto res = mcts.run();

		for (auto& i : res) {
			std::cout << std::get<0>(i) << ',' << std::get<1>(i) << ',' << std::get<2>(i) << '\n';
		}
		//std::cout << '\n';

		int action = get_max_action(res);
		env.step(action);
		reward += env.reward() - reward;
	}
	std::cout << reward / 300 << '\n';

	std::cout << ((double)clock() - t1) / CLOCKS_PER_SEC;
	//system("pause");
	return 0;
}