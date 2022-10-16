#pragma once

#include <iostream>
#include <fstream>
#include <algorithm>
#include <ctime>
#include <random>
#include <vector>

namespace additionl_staff {
	const double pi = 3.14159265358979323846;

	int factorial(int n) {
		if (n == 1 || n == 0) return 1;
		return factorial(n - 1) * n;
	}

	bool next_permutation(std::vector<double>& a)
	{
		int n = a.size();
		int j = n - 2;
		while (j != -1 && a[j] >= a[j + 1]) j--;
		if (j == -1)
			return false; // ?????? ???????????? ???
		int k = n - 1;
		while (a[j] >= a[k]) k--;
		std::swap(a[j], a[k]);
		int l = j + 1, r = n - 1; // ????????? ?????????? ????? ??????????????????
		while (l < r)
			std::swap(a[l++], a[r--]);
		return true;
	}

	int inversions(std::vector<double>& a)
	{
		int count = 0;
		for (int i = 0; i < a.size() - 1; i++)
		{
			for (int j = i + 1; j < a.size(); j++)
			{
				if (a[i] > a[j])
					count++;
			}
		}
		return count;
	}
}

class LMatrix {
public:
	//row number  
	int n = 0;
	//column number 
	int m = 0;
	//contains matrix data 
	std::vector<std::vector<double>> data;

	LMatrix() = default;
	LMatrix(int _n, int _m) : n(_n), m(_m)
	{
		if (n == 0 || m == 0) throw std::invalid_argument("Matix size is 0\n");
		data.resize(n);
		for (auto& i : data) i.resize(m);
	}

	//initialization
	void init(int _n, int _m)
	{
		if (_n == 0 || _m == 0) throw "Matix size is 0\n";
		n = _n;
		m = _m;
		data.resize(n);
		for (auto& i : data) i.resize(m);
	}

	LMatrix operator=(LMatrix right) {
		//self copy avoiding 
		if (this == &right) {
			return *this;
		}

		n = right.n;
		m = right.m;
		data = right.data;
		return *this;
	}
	LMatrix operator*(LMatrix& right) {
		if (m != right.n) throw "(matrix_mult) are you stupid?\n";

		LMatrix temp(n, right.m);

		for (int i = 0; i < n; i++)
			for (int j = 0; j < right.m; j++) {
				temp.data[i][j] = 0;
				for (int k = 0; k < m; k++)
				{
					temp.data[i][j] += data[i][k] * right.data[k][j];
				}
			}
		return temp;
	}
	LMatrix operator*(double right) {

		LMatrix temp(n, m);

		for (int i = 0; i < n; i++)
			for (int j = 0; j < m; j++)
			{
				temp.data[i][j] = data[i][j] * right;
			}
		return temp;
	}
	LMatrix operator+(LMatrix right) {
		if (n != right.n || m != right.m) throw "Matrix sizes are different\n";

		LMatrix temp(n, m);
		for (int i = 0; i < n; i++)
			for (int j = 0; j < m; j++)
			{
				temp.data[i][j] = data[i][j] + right.data[i][j];
			}
		//this->Delete();
		return temp;
	}
	LMatrix operator-(LMatrix right) {
		if (n != right.n || m != right.m) throw "Matrix sizes are different\n";

		LMatrix temp(n, m);
		for (int i = 0; i < n; i++)
			for (int j = 0; j < m; j++)
			{
				temp.data[i][j] = data[i][j] - right.data[i][j];
			}
		//this->Delete();
		return temp;
	}

	double& operator() (int i, int j) {
		if (i < 0 || i >= n) throw std::invalid_argument("wrong i");
		if (j < 0 || j >= m) throw std::invalid_argument("wrong j");
		return this->data[i][j];
	}


	//transpose 
	LMatrix T() {
		LMatrix temp(m, n);

		for (int i = 0; i < m; i++)
			for (int j = 0; j < n; j++)
			{
				temp.data[i][j] = data[j][i];
			}
		return temp;
	}

	//determenation
	double det() {

		if (n != m) throw std::invalid_argument("matrix are not quadric");

		double res = 0;
		std::vector<double> line(n);
		for (int i = 0; i < n; ++i)
			line[i] = i;

		int N_k;
		for (int i = 0; i < additionl_staff::factorial(n); ++i) {
			double tmp_res = 1;
			for (int j = 0; j < n; ++j) {
				tmp_res *= data[j][line[j]];
			}

			N_k = additionl_staff::inversions(line);

			res += (N_k % 2 ? -1 : 1) * tmp_res;
			additionl_staff::next_permutation(line);
		}

		return res;
	}

	//inverse matrix 
	LMatrix inverse() {
		if (n != m) throw std::invalid_argument("matrix are not quadric");

		LMatrix temp = *this;
		temp.console_out();
		LMatrix E(n, m);
		for (int i = 0; i < n; ++i)
			for (int j = 0; j < m; ++j) {
				E(i, j) = i == j ? 1 : 0;
			}

		for (int i = 0; i < m; ++i) {
			for (int j = i + 1; j < n; ++j) {
				double mult = temp(j, i) / temp(i, i);
				for (int k = 0; k < m; ++k) {
					temp(j, k) -= mult * temp(i, k);
					E(j, k) -= mult * E(i, k);
				}
			}
		}
		for (int i = m - 1; i >= 0; --i) {
			for (int j = i - 1; j >= 0; --j) {
				double mult = temp(j, i) / temp(i, i);
				for (int k = 0; k < m; ++k) {
					temp(j, k) -= mult * temp(i, k);
					E(j, k) -= mult * E(i, k);
				}
			}
		}
		for (int i = 0; i < n; ++i) {
			double div = temp(i, i);
			for (int k = 0; k < m; ++k) {
				temp(i, k) /= div;
				E(i, k) /= div;
			}
		}
		return E;
	}

	//read from stream
	void read(std::ifstream& stream) {
		for (int i = 0; i < n; i++)
			for (int j = 0; j < m; j++)
			{
				stream >> data[i][j];
			}
	}

	//write in console 
	void console_out() {
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < m; j++)
			{
				std::cout << data[i][j] << " ";
			}
			std::cout << "\n";
		}
		std::cout << "\n";
	}


	void Wrand() {
		//srand(time(0));
		//std::default_random_engine generator;
		//std::uniform_int_distribution<int> distribution(0, 100);
		for (auto& i : data)
			for (auto& j : i)
				j = (double)(rand() % 100) / 100 - 0.5;
		//j = (double)(distribution(generator)) / 100 - 0.5;
	}

	//applies sigmoid to all elements
	void sigmoid() {
		for (auto& i : data)
			for (auto& j : i)
				j = 1 / (1 + exp(-j));
	}

	//makes line from all matrix elements 
	std::vector<double> getLine() const {
		if (n == 0 || m == 0) throw std::invalid_argument("empty matrix");
		std::vector<double> ans(n * m);
		for (int i = 0; i < n; ++i)
			for (int j = 0; j < m; ++j)
				ans[i * m + j] = this->data[i][j];
		return ans;
	}

	void setLine(std::vector<double>& inVector) {
		if (n * m != inVector.size()) throw std::invalid_argument("wrong size");
		for (int i = 0; i < n; ++i)
			for (int j = 0; j < m; ++j)
				this->data[i][j] = inVector[i * m + j];
	}

	void SetToIdentity()
	{
		//if (!IsSquare())
		//	throw std::invalid_argument("Cannot form an identity matrix that is not square.");

		for (int row = 0; row < n; ++row)
		{
			for (int col = 0; col < m; ++col)
			{
				if (col == row)
					data[row][col] = 1.0;
				else
					data[row][col] = 0.0;
			}
		}
	}

	void rand_symmetric() {
		for(int i = 0; i < n; ++i)
			for (int j = i; j < m; ++j) {
				double num = (double)(rand() % 100);
				data[i][j] = num;
				data[j][i] = num;
			}
	}
};
