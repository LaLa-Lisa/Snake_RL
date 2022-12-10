#pragma once

#include <iostream>
#include <Windows.h>
#include <vector>
#include <cmath>

#define PI 3.14159265

struct Scoord {
public:
	int X, Y;
	Scoord() : X(-1), Y(-1) {}
	Scoord(int _x, int _y) : X(_x), Y(_y) {}
	bool operator==(const Scoord& right) const { return X == right.X && Y == right.Y;  }
	Scoord& operator=(Scoord& right)
	{
		X = right.X; 
		Y = right.Y;
		return *this;
	}
};

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
		score = 0;
		Head.X = wight / 2;
		Head.Y = heght / 2;
		Tail.reserve(wight * heght);
		spawn_frut();
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
				if (j == Head.X && i == Head.Y) std::cout << "O";
				else if (j == Frut.X && i == Frut.Y) std::cout << "F";
				else if (is_tail(Scoord(j, i))) std::cout << "o";
				else std::cout << " ";
			}
			std::cout << "#" << std::endl;
		}
		for (int i = 0; i < heght + 2; i++) {
			std::cout << "#";
		}
		std::cout << std::endl;
		std::cout << "Sorce:" << score << " and Tail length: " << snake_len() << std::endl;
		std::cout << "FrutCoords:" << Frut.X << " " << Frut.Y << std::endl;
		std::cout << "Direction: " << direction() << "       " << std::endl;
		std::cout << "TailX: ";
		for (auto& i : Tail) std::cout << i.X << " ";
		std::cout << std::endl << "TailY: ";
		for (auto& i : Tail) std::cout << i.Y << " ";
		std::cout << std::endl;

	}
	void step(const int _input_direction) {
		++step_counter;
		Input(_input_direction);
		Logic();
	}
	
	std::vector<double> observe() {
		double sensor1 = Head.X;
		double sensor2 = Head.Y;
		double sensor3 = heght - Head.X - 1;
		double sensor4 = wight - Head.Y - 1;
		double diag1 = -1;
		double diag2 = -1;
		double diag3 = -1;
		double diag4 = -1;
		for (int i = 0; i < min(sensor1, sensor2); ++i)
			if (is_tail(Scoord(Head.X - i, Head.Y - i))) {
				diag1 = i;
				break;
			}
			
		for (int i = 0; i < min(sensor2, sensor3); ++i)
			if (is_tail(Scoord(Head.X + i, Head.Y - i))) {
				diag2 = i;
				break;
			}
	
		for (int i = 0; i < min(sensor3, sensor4); ++i)
			if (is_tail(Scoord(Head.X + i, Head.Y + i))) {
				diag3 = i;
				break;
			}
		for (int i = 0; i < min(sensor4, sensor1); ++i)
			if (is_tail(Scoord(Head.X - i, Head.Y + i))) {
				diag4 = i;
				break;
			}
	
		double sensor5 = -1;
		double sensor6 = -1;
		double sensor7 = -1;
		double sensor8 = -1;
		if (Head.X == Frut.X) (Head.Y > Frut.Y) ? sensor6 = Head.Y - Frut.Y - 1 : sensor8 = Frut.Y - Head.Y - 1;
		if (Head.Y == Frut.Y) (Head.X > Frut.X) ? sensor5 = Head.X - Frut.X - 1 : sensor7 = Frut.X - Head.X - 1;

		double sensor9 = -1;
		double sensor10 = -1;
		double sensor11 = -1;
		double sensor12 = -1;
		for (int i = 0; i < Tail.size(); ++i) {
			if (Head.X == Tail[i].X) (Head.Y > Tail[i].Y) ? sensor10 =  min(max(sensor10, sensor2), Head.Y - Tail[i].Y - 1) : sensor12 = min(max(sensor12, sensor4), Tail[i].Y - Head.Y - 1);
			if (Head.Y == Tail[i].Y) (Head.X > Tail[i].X) ? sensor9 = min(max(sensor9, sensor1), Head.X - Tail[i].X - 1) : sensor11 = min(max(sensor11, sensor3), Tail[i].X - Head.X - 1);
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

	std::vector<double> observe_light() const{
		double sensor1 = Head.X;
		double sensor2 = Head.Y;
		double sensor3 = heght - Head.X - 1;
		double sensor4 = wight - Head.Y - 1;
		
		double sensor5 = -1;
		double sensor6 = -1;
		double sensor7 = -1;
		double sensor8 = -1;
		if (Head.X == Frut.X) (Head.Y > Frut.Y) ? sensor6 = Head.Y - Frut.Y - 1 : sensor8 = Frut.Y - Head.Y - 1;
		if (Head.Y == Frut.Y) (Head.X > Frut.X) ? sensor5 = Head.X - Frut.X - 1 : sensor7 = Frut.X - Head.X - 1;

		double sensor9 = -1;
		double sensor10 = -1;
		double sensor11 = -1;
		double sensor12 = -1;
		for (int i = 0; i < Tail.size(); ++i) {
			if (Head.X == Tail[i].X) (Head.Y > Tail[i].Y) ? sensor10 = min(max(sensor10, sensor2), Head.Y - Tail[i].Y - 1) : sensor12 = min(max(sensor12, sensor4), Tail[i].Y - Head.Y - 1);
			if (Head.Y == Tail[i].Y) (Head.X > Tail[i].X) ? sensor9 = min(max(sensor9, sensor1), Head.X - Tail[i].X - 1) : sensor11 = min(max(sensor11, sensor3), Tail[i].X - Head.X - 1);
		}
		std::vector<double> ans;
		switch (dir)
		{
		case UP:
			ans = { sensor1, sensor2, sensor3, sensor5, sensor6, sensor7, sensor9, sensor10, sensor11 };
			break;
		case RIGHT:
			ans = { sensor2, sensor3, sensor4, sensor6, sensor7, sensor8, sensor10, sensor11, sensor12 };
			break;
		case LEFT:
			ans = { sensor4, sensor1, sensor2, sensor8, sensor5, sensor6, sensor12, sensor9, sensor10 };
			break;
		case DOWN:
			ans = { sensor3, sensor4, sensor1, sensor7, sensor8, sensor5, sensor11, sensor12, sensor9 };
			break;
		}

		for (auto& i : direction_output()) {
			ans.push_back((double)i);
		}
		return ans;
	}

	std::vector<double> observe2() const {
		std::vector<double> angles = { PI / 4, 3 * PI / 4 };
		std::vector<double> sensors(angles.size() * 2);
		for (auto& i : Tail) {
			check_sensors(Head, i, sensors, angles);
		}
		return sensors;
	}
	bool is_done() const {
		return gameOver;
	}
	int snake_len() const {
		return Tail.size();
	}
	double reward() const {
		return score;
	}

	std::string direction() const {
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
	std::vector<int> direction_output() const {
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
	Scoord Head;
	Scoord Frut;
	int score = 0;
	std::vector<Scoord> Tail;
	bool gameOver;
	enum mDirection { UP, RIGHT, LEFT, DOWN };
	mDirection dir;

	int step_counter = 0;

	//anti circle
	bool is_circle_restriced = true;
	const int max_availible_steps;
	int steps_without_frut = 0;

	mDirection Input(const int way) {
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
		return dir;
	}
	void Logic() {
		if (Head == Frut) {
			++score;
			steps_without_frut = 0;
			spawn_frut();
			Tail.push_back(Scoord()); //korotkii kostil
		}
		else {
			++steps_without_frut;
		}
		move_snake_forward();
		gameOver = is_gameOver();
	}
	void setcur(int a, int b) {
		COORD coord;
		coord.X = a;
		coord.Y = b;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
	}

	bool spawn_frut() {
		while (true) {
			Frut.X = rand() % heght;
			Frut.Y = rand() % wight;
			if (Frut == Head) continue;
			if (is_tail(Frut)) continue;
			break;
		}
		return true;
	}
	
	bool is_tail(const Scoord& Dot) const {
		for (auto& i : Tail)
			if (Dot == i)
				return true;
		return false;
	}
	bool is_gameOver() const {
		if (is_circle_restriced && max_availible_steps < steps_without_frut) return true;
		if (Head.X == heght) return true;
		if (Head.Y == wight) return true;
		if (Head.X < 0) return true;
		if (Head.Y < 0) return true;
		if (is_tail(Head)) return true;
		return false;
	}

	void move_snake_forward() {
		for (int i = Tail.size() - 1; i > 0; --i) {
			Tail[i] = Tail[i - 1];
		}
		if(score) Tail[0] = Head;

		switch (dir)
		{
		case UP:
			Head.Y--;
			break;
		case RIGHT:
			Head.X++;
			break;
		case LEFT:
			Head.X--;
			break;
		case DOWN:
			Head.Y++;
			break;
		}
	}

	bool is_under_line(const int k, const int b, const Scoord& Dot) const {
		return k * Dot.X + b > Dot.Y;
	}
	bool is_cross(const int k, const int b, const Scoord& Dot) const {
		bool UL = is_under_line(k, b, Dot);
		bool UR = is_under_line(k, b, Scoord(Dot.X + 1, Dot.Y));
		bool DL = is_under_line(k, b, Scoord(Dot.X, Dot.Y + 1));
		bool DR = is_under_line(k, b, Scoord(Dot.X + 1, Dot.Y + 1));
		return !((UL == UR) && (UR == DL) && (DL == DR));
	}
	double distance(const Scoord& Dot1, const Scoord& Dot2) const {
		return sqrt((Dot1.X - Dot2.X) * (Dot1.X - Dot2.X) + (Dot1.Y - Dot2.Y) * (Dot1.Y - Dot2.Y));
	}
	std::pair<double, double> sensor(const Scoord& Start, const Scoord& Dot, const int alpha) const {
		double k = tan(alpha);
		double c_x = (double)Start.X + 1 / 2;
		double c_y = (double)Start.Y + 1 / 2;
		double b = c_y - k * c_x;

		double obs = 0;
		if (!is_cross(k, b, Dot)) {
			obs = distance(Scoord(c_x, c_y), Scoord(Dot.X + 1 / 2, Dot.Y + 1 / 2));
		}
		if (alpha > PI / 4 && alpha < 3 * PI / 4) {
			if (c_y < Dot.Y + 1 / 2)
				return std::make_pair(0, obs);
			else
				return std::make_pair(obs, 0);
		}
		else {
			if (c_x < Dot.X + 1 / 2)
				return std::make_pair(0, obs);
			else
				return std::make_pair(obs, 0);
		}
		return std::make_pair(0, 0);
	}
	void check_sensors(const Scoord& Start, const Scoord& Dot, std::vector<double>& sens_data, std::vector<double>& sens_angles) const {
		bool binary = true;
		if (sens_data.size() != 2 * sens_angles.size()) throw;
		for (int i = 0; i < sens_angles.size(); ++i) {
			std::pair<double, double> obs = sensor(Start, Dot, sens_angles[i]);
			if (binary) {
				if (obs.first) sens_data[2 * i] = 1;
				if (obs.second) sens_data[2 * i + 1] = 1;
			}
			else {
				if (obs.first) sens_data[2 * i] = max(sens_data[2 * i], obs.first);
				if (obs.second) sens_data[2 * i + 1] = max(sens_data[2 * i + 1], obs.second);
			}
		}
	}
};

