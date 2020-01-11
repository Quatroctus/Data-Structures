#include <iostream>
#include <random>
#include <string>
#include <time.h>
#include "MarbleBag.h"

int main(void) {
	srand((unsigned int) time(nullptr));
	MarbleBag<std::string> bag = MarbleBag<std::string>(100000000);

	std::string MARBLES[3] = { "red", "green", "blue" };

	for (unsigned int i = 0; i < 3; i++) {
		bag.drop(MARBLES[rand() % 3]);
	}

	std::string marble = bag.grab();
	std::cout << marble << std::endl;

	marble = bag.grab();
	std::cout << marble << std::endl;

	marble = bag.grab();
	std::cout << marble << std::endl;

}
