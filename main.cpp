#include <iostream>
#include <random>
#include <string>
#include <time.h>
#include "MarbleBag.h"
#include "queue.h"

void testMarbles() {
	srand((unsigned int)time(nullptr));
	MarbleBag<std::string> bag = MarbleBag<std::string>(3000000000);

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

int main(void) {
	Queue<int> *queue = new NodeQueue<int>();

	for (int i = 0; i < 20; i++) {
		queue->enqueue(i);
	}

	std::cout << queue->peak() << std::endl;

	for (int i = 0; i < 5; i++) {
		queue->dequeue();
		queue->enqueue(i);
		std::cout << queue->peak() << std::endl;
	}

}
