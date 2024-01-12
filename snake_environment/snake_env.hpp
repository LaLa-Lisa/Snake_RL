#pragma once

#include <iostream>
#define NOMINMAX
#include <Windows.h>
#include <vector>
#include <cmath>

#ifdef SFML_USE
#include <SFML/Graphics.hpp>
#endif

#define PI 3.14159265

struct Scoord {
public:
	int X, Y;
	Scoord() : X(-1), Y(-1) {}
	Scoord(int _x, int _y) : X(_x), Y(_y) {}
	bool operator==(const Scoord& right) const { return X == right.X && Y == right.Y;  }
	/*Scoord& operator=(Scoord& right)
	{
		X = right.X; 
		Y = right.Y;
		return *this;
	}*/
	Scoord& operator=(Scoord right)
	{
		X = right.X;
		Y = right.Y;
		return *this;
	}
};

class snake_env {
public:
	snake_env(int _wight, int _heght)
		: wight(_wight), 
		heght(_heght), 
		max_availible_steps(_wight * _heght * 2)
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

		std::cout << "obs2: ";
		for (auto& i : observe2()) std::cout << i << " ";
		std::cout << std::endl;

	}
	void render() {
		console_render();
		#ifdef SFML_USE
		if (is_sfml_draw) draw_screen();
		#endif
	}
	void step(const int _input_direction) {
		++step_counter;
		Input(_input_direction);
		Logic();
	}
	
	#ifdef SFML_USE
	void set_screen(sf::RenderWindow* _screen) {
		is_sfml_draw = true;
		sreen = _screen;
	}

	void draw_screen() {
		if (!is_sfml_draw) throw;
		int block_size = (int)sreen->getSize().x / heght;

		sf::Event event;
		while (sreen->pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				sreen->close();
		}

		sreen->clear();
		

		//drawing tail
		for (int i = 0; i < Tail.size(); ++i) {
			sf::RectangleShape tail_block;
			tail_block.setSize(sf::Vector2f(block_size, block_size));
			tail_block.setOutlineColor(sf::Color(87, 132, 102));
			if(i % 2) tail_block.setFillColor(sf::Color(127, 198, 78));
			else tail_block.setFillColor(sf::Color(127, 160, 86));
			tail_block.setOutlineThickness(2);
			tail_block.setPosition(block_size * Tail[i].X, block_size * Tail[i].Y);
			sreen->draw(tail_block);
		}

		//drawing furt
		sf::RectangleShape frut;
		frut.setSize(sf::Vector2f(block_size, block_size));
		frut.setFillColor(sf::Color(128, 0, 128));
		frut.setPosition(block_size * Frut.X, block_size * Frut.Y);
		sreen->draw(frut);

		//drawing head
		sf::RectangleShape head;
		head.setSize(sf::Vector2f(block_size, block_size));
		head.setFillColor(sf::Color(128, 128, 0));
		head.setOutlineColor(sf::Color(255, 215, 0));
		head.setOutlineThickness(10);
		head.setPosition(block_size * Head.X, block_size * Head.Y);
		sreen->draw(head);

		sreen->display();
	}
	#endif
	//std::vector<double> observe() {
	//	double sensor1 = Head.X;
	//	double sensor2 = Head.Y;
	//	double sensor3 = heght - Head.X - 1;
	//	double sensor4 = wight - Head.Y - 1;
	//	double diag1 = -1;
	//	double diag2 = -1;
	//	double diag3 = -1;
	//	double diag4 = -1;
	//	for (int i = 0; i < min(sensor1, sensor2); ++i)
	//		if (is_tail(Scoord(Head.X - i, Head.Y - i))) {
	//			diag1 = i;
	//			break;
	//		}
	//		
	//	for (int i = 0; i < min(sensor2, sensor3); ++i)
	//		if (is_tail(Scoord(Head.X + i, Head.Y - i))) {
	//			diag2 = i;
	//			break;
	//		}
	//
	//	for (int i = 0; i < min(sensor3, sensor4); ++i)
	//		if (is_tail(Scoord(Head.X + i, Head.Y + i))) {
	//			diag3 = i;
	//			break;
	//		}
	//	for (int i = 0; i < min(sensor4, sensor1); ++i)
	//		if (is_tail(Scoord(Head.X - i, Head.Y + i))) {
	//			diag4 = i;
	//			break;
	//		}
	//
	//	double sensor5 = -1;
	//	double sensor6 = -1;
	//	double sensor7 = -1;
	//	double sensor8 = -1;
	//	if (Head.X == Frut.X) (Head.Y > Frut.Y) ? sensor6 = Head.Y - Frut.Y - 1 : sensor8 = Frut.Y - Head.Y - 1;
	//	if (Head.Y == Frut.Y) (Head.X > Frut.X) ? sensor5 = Head.X - Frut.X - 1 : sensor7 = Frut.X - Head.X - 1;
	//	double sensor9 = -1;
	//	double sensor10 = -1;
	//	double sensor11 = -1;
	//	double sensor12 = -1;
	//	for (int i = 0; i < Tail.size(); ++i) {
	//		if (Head.X == Tail[i].X) (Head.Y > Tail[i].Y) ? sensor10 =  min(max(sensor10, sensor2), Head.Y - Tail[i].Y - 1) : sensor12 = min(max(sensor12, sensor4), Tail[i].Y - Head.Y - 1);
	//		if (Head.Y == Tail[i].Y) (Head.X > Tail[i].X) ? sensor9 = min(max(sensor9, sensor1), Head.X - Tail[i].X - 1) : sensor11 = min(max(sensor11, sensor3), Tail[i].X - Head.X - 1);
	//	}
	//	std::vector<double> ans;
	//	switch (dir)
	//	{
	//	case UP:
	//		ans = { sensor1, sensor2, sensor3, sensor5, sensor6, sensor7, sensor9, sensor10, sensor11, diag4, diag1, diag2, diag3 };
	//		break;
	//	case RIGHT:
	//		ans = { sensor2, sensor3, sensor4, sensor6, sensor7, sensor8, sensor10, sensor11, sensor12, diag1, diag2, diag3, diag4 };
	//		break;
	//	case LEFT:
	//		ans = { sensor4, sensor1, sensor2, sensor8, sensor5, sensor6, sensor12, sensor9, sensor10, diag2, diag3, diag4, diag1 };
	//		break;
	//	case DOWN:
	//		ans = { sensor3, sensor4, sensor1, sensor7, sensor8, sensor5, sensor11, sensor12, sensor9, diag3, diag4, diag1, diag2 };
	//		break;
	//	}
	//	//ans.push_back((double)tailN);
	//	for (auto& i : direction_output()) {
	//		ans.push_back((double)i);
	//	}
	//	return ans;
	//}

	std::vector<double> observe_hard() const {
		double NON = -1;
		double s_wall_1 = Head.X;
		double s_wall_2 = Head.Y;
		double s_wall_3 = heght - Head.X - 1.0;
		double s_wall_4 = wight - Head.Y - 1.0;
		double s_diag_wall_1 = std::min(s_wall_1, s_wall_2);
		double s_diag_wall_2 = std::min(s_wall_2, s_wall_3);
		double s_diag_wall_3 = std::min(s_wall_3, s_wall_4);
		double s_diag_wall_4 = std::min(s_wall_4, s_wall_1);

		double tempNON = 1000;
		double s_tail_1 = tempNON;
		double s_tail_2 = tempNON;
		double s_tail_3 = tempNON;
		double s_tail_4 = tempNON;
		for (int i = 0; i < Tail.size(); ++i) {
			if (Head.X == Tail[i].X)
				(Head.Y > Tail[i].Y)
				?
				s_tail_2 = std::min(s_tail_2, (double)abs(Head.Y - Tail[i].Y) - 1)
				: 
				s_tail_4 = std::min(s_tail_4, (double)abs(Head.Y - Tail[i].Y) - 1);

			if (Head.Y == Tail[i].Y) 
				(Head.X > Tail[i].X) 
				? 
				s_tail_1 = std::min(s_tail_1, (double)abs(Head.X - Tail[i].X) - 1)
				: 
				s_tail_3 = std::min(s_tail_3, (double)abs(Head.X - Tail[i].X) - 1);
		}
		if (s_tail_1 == tempNON) s_tail_1 = NON;
		if (s_tail_2 == tempNON) s_tail_2 = NON;
		if (s_tail_3 == tempNON) s_tail_3 = NON;
		if (s_tail_4 == tempNON) s_tail_4 = NON;

		double s_diag_tail_1 = NON;
		double s_diag_tail_2 = NON;
		double s_diag_tail_3 = NON;
		double s_diag_tail_4 = NON;
		double s_diag_fruit_1 = NON;
		double s_diag_fruit_2 = NON;
		double s_diag_fruit_3 = NON;
		double s_diag_fruit_4 = NON;
		for (int i = 0; i <= std::min(s_wall_1, s_wall_2); ++i) {
			if (s_diag_tail_1 == NON && is_tail(Scoord(Head.X - i, Head.Y - i))) {
				s_diag_tail_1 = i - 1;
				//break;
			}
			if (Head.X - i == Frut.X && Head.Y - i == Frut.Y) {
				s_diag_fruit_1 = i - 1;
			}
		}

		for (int i = 0; i <= std::min(s_wall_2, s_wall_3); ++i) {
			if (s_diag_tail_2 == NON && is_tail(Scoord(Head.X + i, Head.Y - i))) {
				s_diag_tail_2 = i - 1;
				//break;
			}
			if (Head.X + i == Frut.X && Head.Y - i == Frut.Y) {
				s_diag_fruit_2 = i - 1;
			}
		}

		for (int i = 0; i <= std::min(s_wall_3, s_wall_4); ++i) {
			if (s_diag_tail_3 == NON && is_tail(Scoord(Head.X + i, Head.Y + i))) {
				s_diag_tail_3 = i - 1;
				//break;
			}
			if (Head.X + i == Frut.X && Head.Y + i == Frut.Y) {
				s_diag_fruit_3 = i - 1;
			}
		}
		for (int i = 0; i <= std::min(s_wall_4, s_wall_1); ++i) {
			if (s_diag_tail_4 == NON && is_tail(Scoord(Head.X - i, Head.Y + i))) {
				s_diag_tail_4 = i - 1;
				//break;
			}
			if (Head.X - i == Frut.X && Head.Y + i == Frut.Y) {
				s_diag_fruit_4 = i - 1;
			}
		}



		double s_fruit_1 = NON;
		double s_fruit_2 = NON;
		double s_fruit_3 = NON;
		double s_fruit_4 = NON;
		if (Head.X == Frut.X) (Head.Y > Frut.Y) ? s_fruit_2 = Head.Y - Frut.Y - 1 : s_fruit_4 = Frut.Y - Head.Y - 1;
		if (Head.Y == Frut.Y) (Head.X > Frut.X) ? s_fruit_1 = Head.X - Frut.X - 1 : s_fruit_3 = Frut.X - Head.X - 1;

		
		std::vector<double> ans;
		ans = {
			s_wall_1, s_wall_2, s_wall_3, s_wall_4,
			s_diag_wall_1, s_diag_wall_2, s_diag_wall_3, s_diag_wall_4,
			s_tail_1, s_tail_2, s_tail_3, s_tail_4,
			s_diag_tail_1, s_diag_tail_2, s_diag_tail_3, s_diag_tail_4,
			s_fruit_1, s_fruit_2, s_fruit_3, s_fruit_4,
			s_diag_fruit_1, s_diag_fruit_2, s_diag_fruit_3, s_diag_fruit_4
		};

		for (int i = 0; i < 12; i++) {
			if (ans[i] == NON)
				ans[i] = 0;
			else
				ans[i] = 1.0 / (ans[i] + 1);
		}
		for (int i = 12; i < ans.size(); ++i) {
			if (ans[i] == NON)
				ans[i] = 0;
			else
				ans[i] = 1.0;
		}
		
		
		
		//ans.push_back((double)tailN);
		for (auto& i : direction_output()) {
			ans.push_back(i);
		}
		for (auto& i : tail_direction()) {
			ans.push_back(i);
		}
		return ans;
	}

	/*std::vector<double> observe_light() const{
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
	}*/

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
	int steps_counter() const {
		return step_counter;
	}
	int snake_len() const {
		return Tail.size();
	}
	double reward() const {
		return step_counter + (pow(2, score) + 500 * pow(score, 2.1)) - (pow(0.25 * step_counter, 1.3) * pow(score, 1.2));
	}
	int score_val() const {
		return score;
	}
	int steps_without_fruit() const {
		return this->steps_without_frut;
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
	std::vector<double> direction_output() const {
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
	std::vector<double> tail_direction() const {
		if (!Tail.size()) return { 0, 0, 0, 0 };
		Scoord pred_last;
		Scoord last;
		if (Tail.size() == 1) {
			pred_last = Head;
			last = Tail[0];
		}
		else {
			pred_last = Tail.end()[-2];
			last = Tail.end()[-1];
		}


		if (pred_last.X != last.X && pred_last.Y != last.Y) throw;
		if (pred_last.X == last.X && pred_last.Y == last.Y) throw;
		if (pred_last.X == last.X) {
			if (pred_last.Y > last.Y) {
				return { 0, 0, 0, 1 };
			}
			else {
				return { 1, 0, 0, 0 };
			}
		}
		else {
			if (pred_last.X > last.X) {
				return { 0, 1, 0, 0 };
			}
			else {
				return { 0, 0, 1, 0 };
			}
		}
		return { 0, 0, 0, 0 };
	}

	int snake_len() {
		return tailN;
	}
	double reward() {
		return score;
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

	#ifdef SFML_USE
		sf::RenderWindow* sreen;
		bool is_sfml_draw = false;
	#endif

	int step_counter = 0;

	//anti circle
	bool is_circle_restriced = true;
	const int max_availible_steps;
	int steps_without_frut = 0;

	mDirection Input(const int way) {
		switch (way)
		{
		case 0:
			if (dir != DOWN) dir = UP;
			break;
		case 1:
			if (dir != UP) dir = DOWN;
			break;
		case 2:
			if (dir != RIGHT) dir = LEFT;
			break;
		case 3:
			if (dir != LEFT) dir = RIGHT;
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
			// end game if snake has max size
			if (Tail.size() + 1 == heght * wight) {
				gameOver = true;
				return;
			}
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
				if (obs.first) sens_data[2 * i] = std::max(sens_data[2 * i], obs.first);
				if (obs.second) sens_data[2 * i + 1] = std::max(sens_data[2 * i + 1], obs.second);
			}
		}
	}
};

