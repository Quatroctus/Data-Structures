#include <iostream>
#include <random>
#include <string>
#include <time.h>
#include "MarbleBag.h"
#include "list.h"
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

void testNodeQueue() {
	Queue<int>* queue = new NodeQueue<int>();

	for (int i = 0; i < 20; i++) {
		queue->enqueue(i);
	}

	std::cout << "After Initial: " << queue->peak() << std::endl;

	for (int i = 0; i < 5; i++) {
		queue->dequeue();
		std::cout << "After dequeue: " << queue->peak() << std::endl;
		queue->enqueue(i);
		std::cout << "After enqueue: " << queue->peak() << std::endl;
	}
}

void testArrayQueue() {
	Queue<int> *queue = new ArrayQueue<int>();

	for (int i = 0; i < 20; i++) {
		queue->enqueue(i);
	}

	std::cout << "After Initial: " << queue->peak() << std::endl;

	for (int i = 0; i < 5; i++) {
		queue->dequeue();
		std::cout << "After dequeue: " << queue->peak() << std::endl;
		queue->enqueue(i);
		std::cout << "After enqueue: " << queue->peak() << std::endl;
	}
}

void testArrayList() {
	List<int> *list = new ArrayList<int>();
	for (int i = 0; i < 10; i++)
		list->append(i);

	list->insert(1000, 4);

	for (int i = 0; i < list->size(); i++)
		std::cout << "Index: " << i << " Value: " << list->get(i) << std::endl;

	list->remove(6);
	for (int i = 0; i < list->size(); i++)
		std::cout << "Index: " << i << " Value: " << list->get(i) << std::endl;

	std::cout << list->size() << std::endl;
}

int main(void) {
	
}
