#pragma once

#include <iostream>
#include <Windows.h>
#include <vector>

struct Scoord {
public:
	int X, Y;
	Scoord() : X(-1), Y(-1) {}
	Scoord(int _x, int _y) : X(_x), Y(_y) {}
	bool operator==(const Scoord& right) { return X == right.X && Y == right.Y;  }
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
		if(score) tail_end_step.push_back(Tail.end()[-1]);
		direction_step.push_back(dir);
		Input(_input_direction);
		Logic();
	}
	void backstep(const int _prev_direction) {
		if (gameOver) gameOver = false;
		//Input(_prev_direction);
		if (!step_counter) {
			throw std::invalid_argument("no steps before");
		}
		/*if (dir != direction_step.end()[-1]) {
			throw std::invalid_argument("wrong last action");
		}*/
		//dir = direction_step.end()[-1];
		//fruktic
		

		//hvostic
		if (frut_step.end()[-1] == Head) {
			Frut = frut_step.end()[-1];
			frut_step.pop_back();

			Tail.pop_back();
			--score;

			steps_without_frut = steps_per_frut.end()[-1];
		}
		else {
			--steps_without_frut;
		}
		move_snake_backward();

		
		dir = direction_step.end()[-1];

		--step_counter;
		tail_end_step.pop_back();
		direction_step.pop_back();
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
	bool is_done() {
		return gameOver;
	}
	int snake_len() {
		return Tail.size();
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
	Scoord Head;
	Scoord Frut;
	int score = 0;
	std::vector<Scoord> Tail;
	bool gameOver;
	enum mDirection { UP, RIGHT, LEFT, DOWN };
	mDirection dir;

	//backstep
	int step_counter = 0;
	std::vector<Scoord> tail_end_step;
	std::vector<Scoord> frut_step;
	std::vector<mDirection> direction_step;

	//anti circle
	bool is_circle_restriced = true;
	const int max_availible_steps;
	int steps_without_frut = 0;
	std::vector<int> steps_per_frut;

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
			frut_step.push_back(Frut);
			steps_per_frut.push_back(steps_without_frut);
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
	
	bool is_tail(const Scoord& Dot) {
		for (auto& i : Tail)
			if (Dot == i)
				return true;
		return false;
	}
	bool is_gameOver() {
		if (is_circle_restriced && max_availible_steps < steps_without_frut) {
			gameOver = true;
		}
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
	void move_snake_backward() {
		if (score) {
			for (int i = 0; i < Tail.size() - 1; ++i) {
				Tail[i] = Tail[i + 1];
			}
			Tail.end()[-1] = tail_end_step.end()[-1];
		}
		switch (dir)
		{
		case UP:
			Head.Y++;
			break;
		case RIGHT:
			Head.X--;
			break;
		case LEFT:
			Head.X++;
			break;
		case DOWN:
			Head.Y--;
			break;
		}
	}
};

