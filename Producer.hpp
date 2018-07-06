//
// Created by Equod on 05/07/18.
//

#ifndef PATTERN_PRODUCER_H
#define PATTERN_PRODUCER_H

#include <mutex>
#include <condition_variable>
#include <list>
#include "Consumer.hpp"
#include "semaphore.hpp"

namespace pattern {

template<typename T>
class Producer {
 public:
  void produce(T item) {
	std::unique_lock<std::mutex> lock(log_list_mutex);
	log_list.emplace_back(item);
	lock.unlock();
	semaphore_.notify();
  }
 protected:
  std::list<T> log_list;
  std::mutex log_list_mutex;
  semaphore semaphore_;
};

}

#endif // PATTERN_PRODUCER_H
