#include "pch.h"
#include <iostream>
#include <thread>
#include <queue>
#include <atomic>
#include <mutex>

int main2() {
	size_t count = 0;

	//atomic structures provide the possibility to thread safe change the value
	//because they use atomic processor instructions inside.
	std::atomic<bool> done(false);

	//When several threads try to work with the same resource (items and count in this example) then possible it can generate the
	//race between them. As a result, they can see the data in some inconsistency state. For example, one thread could modify
	//the value but another thread will not see it. To avoid this problem we can use mutexes. If one thread uses some value
	//under the mutex lock then it means that any other thread can't be able to work with this value until the first thread
	//will not release the mutex (mutex is automatically released when lock goes out of scope in C++)
	std::mutex mutex;

	std::queue<int> items;

	std::thread producer([&]() {
		for (int i = 0; i < 10000; ++i) {
			//mutex protects the items and count 
			std::lock_guard<std::mutex> guard(mutex);
			items.push(i);
			count++;
		}
		//no need to protect writing in done because it's atomic structure
		done = true;
	});

	std::thread consumer([&]() {
		while (true) {
			//store the current done value.
			//no need to protect writing in done because it's atomic structure
			bool isDone = done;

			//here we protect the count and items
			std::lock_guard<std::mutex> guard(mutex);
			while (!items.empty()) {
				items.pop();
				count--;
			}

			//if done was true and we get all values from items then break from the while loop
			if (isDone)
				break;
		}
	});

	producer.join();
	consumer.join();
	std::cout << count << std::endl;

	return 0;
}