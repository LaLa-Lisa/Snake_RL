#include <iostream>
#include <conio.h>
#include "snake_env.hpp"

int main() {

	snake_env Env(20, 20);

	while (!Env.is_done()) {
		Env.console_render();
		double input = -1;
		if (_kbhit()) {
			switch (_getch())
			{
			case 'w':
				input = 0;
				break;
			case 's':
				input = 1;
				break;
			case 'd':
				input = 2;
				break;
			case 'a':
				input = 3;
				break;
			default:
				break;
			}
		}
		Env.step(input);
		Sleep(300);
	}

	return 0;
}