#pragma once

#include <vector>
#include <functional>

class LGenetic {
public:
	int mutation_rate = 50;
	std::string file_validation;

	LGenetic(int popSize, int genSize, std::function<double(std::vector<double>&)> _fitness_function);

	//��������� ����������
	enum Crossover { TPINVX, SPBX, SBX, DBX, OX, HX, AX };

	//��������� ���������
	enum Mutation { DRM, CM, HM, PM, AM };

	//��������� ��������� ��������
	enum Selection { FPS, TS };

	//������� ��������
	void learn(int generationsNum);

	//��������� ������� �������� ��������� ���
	std::vector<double> learn_a_lot(int generationsNum, int trysNum);

	//���������� ������ �����
	std::vector<double> best_gene();

	//��������� ���� ���������� ������� 
	void rand_population();
	void rand_population_normal();

	//��������� ���� ��������� �������������� ����� �� 1 �� n
	void rand_population_int_order();

	//������� ����������
	void set_crossover(Crossover);
	//������� �������
	void set_mutation(Mutation);
	//������� ��������
	void set_selection(Selection);

	void set_loss(std::function<double(std::vector<double>&, std::string)> _loss);

	void read_gene(std::vector<double> in_gene);

	//������� ��������� ��������� �� ���
	void set_population(std::vector<std::vector<double>> in_pop);

	void doPart(int from, int to, std::vector<double>& output);
	
	//
	void make_test(std::string file_name, int itarations, int repeat_times, std::vector<std::vector<std::vector<double>>>& init_pop);
private:
	std::vector<std::vector<double>> pop;
	std::vector<double> b_gene;

	int current_iteration;
	int max_iteration;
	//����-�������� �������� 
	void TPINV();
	void SPB();
	void SB();
	void DB();
	void H();
	void O();
	void A();

	void mutationG(std::vector<double>& x);
	void mutationP(std::vector<double>& x);
	void mutationA(std::vector<double>& x);
	void mutationAA(std::vector<double>& x, int genNum);
	void DR(std::vector<double>& x);
	void C(std::vector<double>& x);
	void D(std::vector<double>& x);
	void P(std::vector<double>& x);

	void sort();
	void fitness_proportionate_selection();
	void qsort(std::vector<double>& F, int n);

	std::function<double(std::vector<double>&)> fitness_function;
	std::function<void()> crossover = [this]() { this->SB(); };
	std::function<void()> selection = [this]() { this->sort(); };
	std::function<void(std::vector<double>&)> mutation = [this](std::vector<double>& x) { this->mutationA(x); };


	std::function<double(std::vector<double>&, std::string)> loss;
};