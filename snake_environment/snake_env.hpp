#pragma once

#include <iostream>
#include <Windows.h>
#include <vector>


class snake_env {
public:
	snake_env(int _wight, int _heght): wight(_wight), heght(_heght), max_availible_steps(_wight * _heght * 2)
	{
		reset();
	}
	void reset() {
		gameOver = false;
		step_counter = 0;
		steps_without_frut = 0;
		tailN = 0;
		score = 0;
		TailX.resize(wight * heght, -1);
		TailY.resize(wight * heght, -1);
		x = wight / 2;
		y = heght / 2;
		while (true) {
			FrutX = rand() % heght;
			FrutY = rand() % wight;
			if (FrutX == x && FrutY == y) continue;
			break;
		}
		dir = UP;
	}
	void console_render() {
		setcur(0, 0);
		for (int i = 0; i < heght + 2; i++) {
			std::cout << "#";
		}
		std::cout << std::endl;
		for (int i = 0; i < wight; ++i) {
			std::cout << "#";
			for (int j = 0; j < heght; ++j) {
				if (j == x && i == y) std::cout << "O";
				else if (j == FrutX && i == FrutY) std::cout << "F";
				else {
					bool by = true;
					for (int k = 0; k < tailN; k++) {
						if (j == TailX[k] && i == TailY[k]) {
							std::cout << "o";
							by = false;
						}
					}
					if (by) std::cout << " ";
				}
			}
			std::cout << "#" << std::endl;
		}
		for (int i = 0; i < heght + 2; i++) {
			std::cout << "#";
		}
		std::cout << std::endl;
		std::cout << "Sorce:" << score << std::endl;
		std::cout << "FrutCoords:" << FrutX << " " << FrutY << std::endl;
		std::cout << "Direction: " << direction() << "       " << std::endl;
	}
	void step(const int _input_direction) {
		++step_counter;
		tail_end_step.push_back(std::make_pair(TailX[tailN - 1], TailY[tailN - 1]));
		score_step.push_back(score);
		direction_step.push_back(dir);
		frut_step.push_back(std::make_pair(FrutX, FrutY));
		Input(_input_direction);
		Logic();
	}
	void backstep(const int _prev_direction) {
		Input(_prev_direction);
		if (!step_counter) {
			throw std::invalid_argument("no steps before");
		}
		/*if (dir != direction_step.end()[-1]) {
			throw std::invalid_argument("wrong last action");
		}*/

		//hvostic
		FrutX = frut_step.end()[-1].first;
		FrutY = frut_step.end()[-1].second;

		//fruktic
		for (int i = 0; i < tailN; ++i) {
			TailX[i] = TailX[i + 1];
			TailY[i] = TailY[i + 1];
		}

		if (x == FrutX && y == FrutY) {
			--tailN;
			/*TailX.pop_back();
			TailY.pop_back();*/
		}
		else {
			TailX[tailN - 1] = tail_end_step.end()[-1].first;
			TailY[tailN - 1] = tail_end_step.end()[-1].second;
		}
		

		switch (dir)
		{
		case UP:
			y++;
			break;
		case RIGHT:
			x--;
			break;
		case LEFT:
			x++;
			break;
		case DOWN:
			y--;
			break;
		}
		dir = direction_step.end()[-1];

		score = score_step.end()[-1];

		--step_counter;
		tail_end_step.pop_back();
		score_step.pop_back();
		direction_step.pop_back();
		frut_step.pop_back();

	}
	std::vector<double> observe() {
		double sensor1 = x;
		double sensor2 = y;
		double sensor3 = heght - x - 1;
		double sensor4 = wight - y - 1;
		double diag1 = -1;
		double diag2 = -1;
		double diag3 = -1;
		double diag4 = -1;
		for (int i = 0; i < min(sensor1, sensor2); ++i)
			for (int k = 0; k < tailN; ++k)
				if (x - i == TailX[k] && y - i == TailY[k]) {
					diag1 = i;
					i = min(sensor1, sensor2);
					break;
				}
		for (int i = 0; i < min(sensor2, sensor3); ++i)
			for (int k = 0; k < tailN; ++k)
				if (x + i == TailX[k] && y - i == TailY[k]) {
					diag2 = i;
					i = min(sensor2, sensor3);
					break;
				}
		for (int i = 0; i < min(sensor3, sensor4); ++i)
			for (int k = 0; k < tailN; ++k)
				if (x + i == TailX[k] && y + i == TailY[k]) {
					diag3 = i;
					i = min(sensor3, sensor4);
					break;
				}
		for (int i = 0; i < min(sensor4, sensor1); ++i)
			for (int k = 0; k < tailN; ++k)
				if (x - i == TailX[k] && y + i == TailY[k]) {
					diag4 = i;
					i = min(sensor4, sensor1);
					break;
				}
		double sensor5 = -1;
		double sensor6 = -1;
		double sensor7 = -1;
		double sensor8 = -1;
		if (x == FrutX) (y > FrutY) ? sensor6 = y - FrutY - 1 : sensor8 = FrutY - y - 1;
		if (y == FrutY) (x > FrutX) ? sensor5 = x - FrutX - 1 : sensor7 = FrutX - x - 1;

		double sensor9 = -1;
		double sensor10 = -1;
		double sensor11 = -1;
		double sensor12 = -1;
		for (int i = 0; i < tailN; ++i) {
			if (x == TailX[i]) (y > TailY[i]) ? sensor10 =  min(max(sensor10, sensor2), y - TailY[i] - 1) : sensor12 = min(max(sensor12, sensor4), TailY[i] - y - 1);
			if (y == TailY[i]) (x > TailX[i]) ? sensor9 = min(max(sensor9, sensor1), x - TailX[i] - 1) : sensor11 = min(max(sensor11, sensor3), TailX[i] - x - 1);
		}
		std::vector<double> ans;
		switch (dir)
		{
		case UP:
			ans = { sensor1, sensor2, sensor3, sensor5, sensor6, sensor7, sensor9, sensor10, sensor11, diag4, diag1, diag2, diag3 };
			break;
		case RIGHT:
			ans = { sensor2, sensor3, sensor4, sensor6, sensor7, sensor8, sensor10, sensor11, sensor12, diag1, diag2, diag3, diag4 };
			break;
		case LEFT:
			ans = { sensor4, sensor1, sensor2, sensor8, sensor5, sensor6, sensor12, sensor9, sensor10, diag2, diag3, diag4, diag1 };
			break;
		case DOWN:
			ans = { sensor3, sensor4, sensor1, sensor7, sensor8, sensor5, sensor11, sensor12, sensor9, diag3, diag4, diag1, diag2 };
			break;
		}
		//ans.push_back((double)tailN);
		for (auto& i : direction_output()) {
			ans.push_back((double)i);
		}
		return ans;
	}
	bool is_done() {
		return gameOver;
	}

	int snake_len() {
		return tailN;
	}
	double reward() {
		return score;
	}
	std::string direction() {
		switch (dir)
		{
		case UP:
			return "UP";
			break;
		case RIGHT:
			return "RIGHT";
			break;
		case LEFT:
			return "LEFT";
			break;
		case DOWN:
			return "DOWN";
			break;
		}
		return "STOP";
	}

	std::vector<int> direction_output() {
		switch (dir)
		{
		case UP:
			return { 1, 0, 0, 0 };
			break;
		case RIGHT:
			return { 0, 1, 0, 0 };
			break;
		case LEFT:
			return { 0, 0, 1, 0 };
			break;
		case DOWN:
			return { 0, 0, 0, 1 };
			break;
		}
		return { 0, 0, 0, 0 };
	}

private:
	const int wight, heght;
	int x, y, FrutX, FrutY;
	int score = 0;
	int tailN = 0;
	std::vector<int> TailX, TailY;
	bool gameOver;
	enum mDirection { UP, RIGHT, LEFT, DOWN };
	mDirection dir;

	//backstep
	int step_counter = 0;
	std::vector<std::pair<int, int>> tail_end_step;
	std::vector<std::pair<int, int>> frut_step;
	std::vector<int> score_step;
	std::vector<mDirection> direction_step;

	//anti circle
	bool is_circle_restriced = true;
	const int max_availible_steps;
	int steps_without_frut = 0;

	void Input(const int way) {
		switch (dir)
		{
		case UP:
			if (way == 0) dir = UP;
			if (way == 1) dir = LEFT;
			if (way == 2) dir = RIGHT;
			break;
		case DOWN:
			if (way == 0) dir = DOWN;
			if (way == 1) dir = RIGHT;
			if (way == 2) dir = LEFT;
			break;
		case LEFT:
			if (way == 0) dir = LEFT;
			if (way == 1) dir = DOWN;
			if (way == 2) dir = UP;
			break;
		case RIGHT:
			if (way == 0) dir = RIGHT;
			if (way == 1) dir = UP;
			if (way == 2) dir = DOWN;
			break;
		default:
			break;
		}
	}
	void Logic() {
		if (x == FrutX && y == FrutY) {
			++score;
			while (true) {
				FrutX = rand() % heght;
				FrutY = rand() % wight;
				if (FrutX == x && FrutY == y) continue;
				for (int k = 0; k < tailN + 1; k++)
					if (FrutX == TailX[k] && FrutY == TailY[k])
						continue;
				break;
			}
			++tailN;
			steps_without_frut = 0;
		}
		else {
			++steps_without_frut;
			if (is_circle_restriced && max_availible_steps < steps_without_frut) {
				gameOver = true;
			}
		}
		int headX = x;
		int headY = y;
		for (int i = tailN - 1; i > 0; --i) {
			TailX[i] = TailX[i - 1];
			TailY[i] = TailY[i - 1];
		}
		TailX[0] = headX;
		TailY[0] = headY;
		switch (dir)
		{
		case UP:
			y--;
			break;
		case RIGHT:
			x++;
			break;
		case LEFT:
			x--;
			break;
		case DOWN:
			y++;
			break;
		}
		if (x == heght) gameOver = true;
		if (y == wight) gameOver = true;
		if (x < 0) gameOver = true;
		if (y < 0) gameOver = true;
		for (int k = 1; k < tailN; k++) {
			if (x == TailX[k] && y == TailY[k]) gameOver = true;
		}
	}
	void setcur(int a, int b) {
		COORD coord;
		coord.X = a;
		coord.Y = b;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
	}
};

