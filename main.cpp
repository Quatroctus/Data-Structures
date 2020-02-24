//#define USE_BITSET // Use a std::bitset to keep track of allocated and free locations.
#define ALLOCATION_SIZE 416 // Request an allocation chunk size.
#include "memory_manager.h"

#include "marble_bag.h"
#include "hash_map.h"
#include "lists.h"
#include "queue.h"
#include "tree.h"
#include <iostream>
#include <random>
#include <string>
#include <time.h>

void testMarbles() {
	srand((unsigned int) time(nullptr));
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

void testNodeList() {
	List<int> *list = new NodeList<int>();
	for (int i = 0; i < 10; i++) {
		list->append(i);
		std::cout << "Index: " << i << " Value: " << list->get(i) << std::endl;
	}

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

void testArrayList() {
	List<int> *list = new ArrayList<int>();
	for (int i = 0; i < 10; i++) {
		list->append(i);
		std::cout << "Index: " << i << " Value: " << list->get(i) << std::endl;
	}

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

void testSortedNodeList() {
	List<int> *list = new SortedNodeList<int>();

	std::cout << "First 3" << std::endl;

	list->append(100);
	list->append(55);
	list->append(10);

	for (int i = 0; i < list->size(); i++)
		std::cout << list->get(i) << std::endl;

	std::cout << "Next 7" << std::endl;

	for (int i = 0; i < 7; i++)
		list->append(i * 20);

	for (int i = 0; i < list->size(); i++)
		std::cout << list->get(i) << std::endl;

	delete list;
}

void testSortedArrayList() {
	List<int> *list = new SortedArrayList<int>();

	std::cout << "First 3" << std::endl;

	list->append(100);
	list->append(10);
	list->append(55);

	for (int i = 0; i < list->size(); i++)
		std::cout << list->get(i) << std::endl;

	std::cout << "Next 7" << std::endl;

	for (int i = 0; i < 7; i++)
		list->append(i * 20);

	for (int i = 0; i < list->size(); i++)
		std::cout << list->get(i) << std::endl;

	delete list;
}

void testBinarySearchTree() {
	Tree<int, BinarySearchTree<int>> *tree = new BinarySearchTree<int>(90);
	int values[10] = {10, 20, 0, 30, 40, 50, 60, 70, 80, 100};

	for (auto& val : values)
		tree->append(val);

	for (int i = 0; i < 11; i++)
		std::cout << "Value " << tree->get(i) << " at " << i << std::endl;
	
	std::cout << "Pre-Order" << std::endl;
	tree->print(std::cout, BinarySearchTree<int>::PRE_ORDER);
	std::cout << std::endl;

	std::cout << "Post-Order" << std::endl;
	tree->print(std::cout, BinarySearchTree<int>::POST_ORDER);
	std::cout << std::endl;

	std::cout << "In-Order" << std::endl;
	tree->print(std::cout, BinarySearchTree<int>::IN_ORDER);
	std::cout << std::endl;

	tree->remove(3); // Should remove the value 30 from the tree because it is at the 3rd index in sorted order.

	std::cout << "Remove position 3" << std::endl;
	tree->print(std::cout, BinarySearchTree<int>::IN_ORDER);
	std::cout << std::endl;

	tree->removeValue(50); // Should remove the value 50 from the tree.

	std::cout << "Remove value 50" << std::endl;
	tree->print(std::cout, BinarySearchTree<int>::IN_ORDER);
	std::cout << std::endl;

	tree->removeValue(90); // Remove the root with both left and right attachments. Should move the right to the root.

	std::cout << "Remove root: 90" << std::endl;
	tree->print(std::cout, BinarySearchTree<int>::IN_ORDER);
	std::cout << std::endl;

	tree->removeValue(100); // Remove the root with only a left. Should move the left to the root.

	std::cout << "Remove root: 100" << std::endl;
	tree->print(std::cout, BinarySearchTree<int>::IN_ORDER);
	std::cout << std::endl;

	tree->removeValue(80); // Remove a value at the end of a branch.

	std::cout << "Remove 80" << std::endl;
	tree->print(std::cout, BinarySearchTree<int>::IN_ORDER);
	std::cout << std::endl;

	// We have to manually call Tree::destroyTree() in order to free all the memory from this Tree.
	// Because we defined our tree as a recursive data-type. If we instead had a container which contained a BiNode *root we could just call delete tree to handle it.
	tree->destroyTree();
}

void testMemoryManager() {
	std::cout << START << std::endl;
	managed_ptr<int> ptr = MemoryManager::allocate<int>(100); // Should allocate 100 integers for each managed_ptr.
	managed_ptr<int> ptr2 = MemoryManager::allocate<int>(100);
	managed_ptr<int> ptr3 = MemoryManager::allocate<int>(120);
	ptr[0] = 10; // Just set some values...
	ptr[1] = 256;
	ptr2[0] = 1000;
	ptr2[1] = 1024;
	ptr3[0] = 100000;
	ptr3[1] = 4096;

	std::cout << ptr[0] << std::endl; // Print the values.
	std::cout << ptr[1] << std::endl;
	std::cout << ptr[99] << std::endl;
	std::cout << ptr << std::endl;
	std::cout << ptr2[0] << std::endl;
	std::cout << ptr2[1] << std::endl;
	std::cout << ptr2[99] << std::endl;
	std::cout << ptr2 << std::endl;
	std::cout << ptr3[0] << std::endl;
	std::cout << ptr3[1] << std::endl;
	std::cout << ptr3[119] << std::endl;
	std::cout << ptr3 << std::endl;

	ptr2.~managed_ptr(); // Destruct ptr2 to see if refCounts are working and memory deallocation is also working.
	ptr2 = MemoryManager::allocate<int>(100); // Should Allocate the same set of memory it had before.
	std::cout << ptr2[0] << std::endl; // Notice memory is not cleared out.
	std::cout << ptr2[1] << std::endl;
	std::cout << ptr2[2] << std::endl;
	std::cout << ptr2 << std::endl;

	MemoryManager::freeAllocations();
}

void testHashMap() {
	Map<std::string, int> *map = new HashMap<std::string, int>();

	map->put("hundred", 100);
	map->put("big", 102);

	Optional<int> val = map->query("hundred");
	if (val.exists)
		std::cout << "Value at 'hundred' is: " << val.value << std::endl;
	else
		std::cout << "No Value Found at key 'hundred'" << std::endl;
	val = map->query("big");
	if (val.exists)
		std::cout << "Value at 'big' is: " << val.value << std::endl;
	else
		std::cout << "No Value Found at key 'big'" << std::endl;
	val = map->query("alphabet");
	if (val.exists)
		std::cout << "Value at 'alphabet' is: " << val.value << std::endl;
	else
		std::cout << "No Value Found at key 'alphabet'" << std::endl;

	std::cout << "Map contains 'hundred': " << (map->contains("hundred") ? "true" : "false") << std::endl;
	std::cout << "Map contains 'big': " << (map->contains("big") ? "true" : "false") << std::endl;
	std::cout << "Map contains 'alphabet': " << (map->contains("alphabet") ? "true" : "false") << std::endl;

	val = map->remove("big");
	if (val.exists)
		std::cout << "Removed " << val.value << " found at key 'big'" << std::endl;
	else
		std::cout << "No Value Found for Removal with key: 'big'" << std::endl;

	std::cout << "Map contains 'hundred': " << (map->contains("hundred") ? "true" : "false") << std::endl;
	std::cout << "Map contains 'big': " << (map->contains("big") ? "true" : "false") << std::endl;
	std::cout << "Map contains 'alphabet': " << (map->contains("alphabet") ? "true" : "false") << std::endl;

	map->put("big", 102);
	map->put("alphabet", 26);

	val = map->query("alphabet");
	if (val.exists)
		std::cout << "Value at 'alphabet' is: " << val.value << std::endl;
	else
		std::cout << "No Value Found at key 'alphabet'" << std::endl;

	std::cout << "Map contains 'hundred': " << (map->contains("hundred") ? "true" : "false") << std::endl;
	std::cout << "Map contains 'big': " << (map->contains("big") ? "true" : "false") << std::endl;
	std::cout << "Map contains 'alphabet': " << (map->contains("alphabet") ? "true" : "false") << std::endl;

}

int main(void) {
	testHashMap();
	return 0;
}
