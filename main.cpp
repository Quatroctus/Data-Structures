#include <iostream>
#include <random>
#include <string>
#include <time.h>
#include "MarbleBag.h"
#include "list.h"
#include "queue.h"

void testMarbles() {
	srand((unsigned int)time(nullptr));
	MarbleBag<std::string> bag = MarbleBag<std::string>(100);

	std::string MARBLES[3] = { "red", "green", "blue" };

	for (int i = 0; i < 3; i++) {
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
	Queue<int> *queue = new NodeQueue<int>();

	for (int i = 0; i < 20; i++) {
		queue->enqueue(i);
	}

	std::cout << "peek After Initial Queuing: " << queue->peek() << std::endl;

	for (int i = 0; i < 5; i++) {
		std::cout << "Dequeued: " << queue->dequeue() << std::endl;
		std::cout << "peek After Dequeue: " << queue->peek() << std::endl;
		std::cout << "Queuing was " << (queue->enqueue(i) ? "successful." : "failure") << std::endl;
		std::cout << "peek After enqueue: " << queue->peek() << std::endl;
	}
	delete queue;
}

void testArrayQueue() {
	Queue<int> *queue = new ArrayQueue<int>();

	for (int i = 0; i < 20; i++) {
		queue->enqueue(i);
	}

	std::cout << "peek After Initial Queuing: " << queue->peek() << std::endl;

	for (int i = 0; i < 5; i++) {
		std::cout << "Dequeued: " << queue->dequeue() << std::endl;
		std::cout << "peek After Dequeue: " << queue->peek() << std::endl;
		std::cout << "Queuing was " << (queue->enqueue(i) ? "successful." : "failure") << std::endl;
		std::cout << "peek After enqueue: " << queue->peek() << std::endl;
	}
	delete queue;
}

void testArrayList() {
	List<int> *list = new ArrayList<int>();
	for (int i = 0; i < 10; i++)
		list->append(i);

	std::cout << "Inserting at 4 was: " << (list->insert(1000, 4) ? "successful." : "failure.") << std::endl;
	std::cout << "Inserting at 12 was: " << (list->insert(3000, 12) ? "successful." : "failure.") << std::endl;
	std::cout << "Inserting at 11 was: " << (list->insert(2000, 11) ? "successful." : "failure.") << std::endl;

	for (int i = 0; i < list->size(); i++)
		std::cout << "Index: " << i << " Value: " << list->get(i) << std::endl;

	std::cout << "Removed " << list->remove(6) << " at index 6." << std::endl;

	for (int i = 0; i < list->size(); i++)
		std::cout << "Index: " << i << " Value: " << list->get(i) << std::endl;

	std::cout << "Size: " << list->size() << std::endl;

	delete list;
}

int main(void) {
	std::cout << "Testing Node Queue" << std::endl;
	testNodeQueue();
	std::cout << std::endl << std::endl << "Testing Array Queue" << std::endl;
	testArrayQueue();
}
