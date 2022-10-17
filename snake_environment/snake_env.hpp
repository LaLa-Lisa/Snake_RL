#pragma once

#include <iostream>
#include <Windows.h>
#include <vector>


class snake_env {
public:
	snake_env(int _wight, int _heght): wight(_wight), heght(_heght)
	{
		reset();
	}
	void reset() {
		gameOver = false;
		tailN = 0;
		sorce = 0;
		TailX.resize(wight * heght, -1);
		TailY.resize(wight * heght, -1);
		x = wight / 2;
		y = heght / 2;
		while (true) {
			FrutX = rand() % (heght - 2) + 1;
			FrutY = rand() % (wight);
			if (FrutX == x && FrutY == y) continue;
			break;
		}
		dir = UP;
	}
	void console_render() {
		setcur(0, 0);
		for (int i = 0; i < heght; i++) {
			std::cout << "#";
		}
		std::cout << std::endl;
		for (int i = 0; i < wight; i++) {
			for (int j = 0; j < heght - 1; j++) {
				if (j == 0) std::cout << "#";
				else if (j == x && i == y) std::cout << "O";
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

				if (j == heght - 2) std::cout << "#";

			}
			std::cout << std::endl;
		}
		for (int i = 0; i < heght; i++) {
			std::cout << "#";
		}
		std::cout << std::endl;
		std::cout << "Sorce:" << sorce << std::endl;
		std::cout << "FrutCoords:" << FrutX << " " << FrutY << "\n";
	}
	void step(const int _input_direction) {
		Input(_input_direction);
		Logic();
	}
	std::vector<double> observe() {
		double sensor1 = (x - 1);
		double sensor2 = (y);
		double sensor3 = (heght - 2 - x);
		double sensor4 = (wight - y - 1);
		double diag1 = 0;
		double diag2 = 0;
		double diag3 = 0;
		double diag4 = 0;
		for (int i = 0; i < min(sensor1, sensor2); i++)
			for (int k = 0; k < tailN; k++)
				if (x - i == TailX[k] && y - i == TailY[k]) {
					diag1 = 1;
					i = 10000;
					break;
				}
		for (int i = 0; i < min(sensor2, sensor3); i++)
			for (int k = 0; k < tailN; k++)
				if (x + i == TailX[k] && y - i == TailY[k]) {
					diag2 = 1;
					i = 10000;
					break;
				}
		for (int i = 0; i < min(sensor3, sensor4); i++)
			for (int k = 0; k < tailN; k++)
				if (x + i == TailX[k] && y + i == TailY[k]) {
					diag3 = 1;
					i = 10000;
					break;
				}
		for (int i = 0; i < min(sensor4, sensor1); i++)
			for (int k = 0; k < tailN; k++)
				if (x - i == TailX[k] && y + i == TailY[k]) {
					diag4 = 1;
					i = 10000;
					break;
				}
		double sensor5 = -1;
		double sensor6 = -1;
		double sensor7 = -1;
		double sensor8 = -1;
		if (x == FrutX) (y > FrutY) ? sensor6 = y - FrutY + 1 : sensor8 = FrutY - y + 1;
		if (y == FrutY) (x > FrutX) ? sensor5 = x - FrutX + 1 : sensor7 = FrutX - x + 1;

		for (int i = 0; i < tailN; ++i) {
			if (x == TailX[i]) { (y > TailY[i]) ? sensor2 = min(sensor2, (y - TailY[i]) - 1) : sensor4 = min(sensor4, (TailY[i] - y) - 1); }
			if (y == TailY[i]) (x > TailX[i]) ? sensor1 = min(sensor1, (x - TailX[i]) - 1) : sensor3 = min(sensor3, (TailX[i] - x) - 1);
		}
		std::vector<double> ans;
		switch (dir)
		{
		case UP:
			ans = { sensor1, sensor2, sensor3, sensor5, sensor6, sensor7, diag4, diag1, diag2, diag3 };
			break;
		case RIGHT:
			ans = { sensor2, sensor3, sensor4, sensor6, sensor7, sensor8, diag1, diag2, diag3, diag4 };
			break;
		case LEFT:
			ans = { sensor4, sensor1, sensor2, sensor8, sensor5, sensor6, diag2, diag3, diag4, diag1 };
			break;
		case DOWN:
			ans = { sensor3, sensor4, sensor1, sensor7, sensor8, sensor5, diag3, diag4, diag1, diag2 };
			break;
		}
		ans.push_back((double)tailN);
		return ans;
	}
	bool is_done() {
		return gameOver;
	}

	int snake_len() {
		return tailN;
	}

private:
	const int wight, heght;
	int x, y, FrutX, FrutY;
	int sorce = 0;
	int tailN = 0;
	std::vector<int> TailX, TailY;
	bool gameOver;
	enum mDirection { UP, RIGHT, LEFT, DOWN };
	mDirection dir;

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
			sorce += 1;
			while (true) {
				FrutX = rand() % (heght - 2) + 1;
				FrutY = rand() % (wight);
				if (FrutX == x && FrutY == y) continue;
				for (int k = 0; k < tailN + 1; k++)
					if (FrutX == TailX[k] && FrutY == TailY[k])
						continue;
				break;
			}
			tailN++;
		}
		int headX = x;
		int headY = y;
		for (int i = tailN - 1; i > 0; i--) {
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
		if (x == heght - 1) gameOver = true;
		if (y == wight) gameOver = true;
		if (x <= 0) gameOver = true;
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