//
// Created by bandyer on 05/07/18.
//

#ifndef ERIZO_ALL_PRODUCER_H
#define ERIZO_ALL_PRODUCER_H

#include <mutex>
#include <condition_variable>
#include <list>
#include "Consumer.h"

namespace pattern {

template<typename T>
class Producer {
 public:
  void produce(T item) {
	std::unique_lock<std::mutex> lock(log_list_mutex);
	cv.wait(lock);
	log_list.emplace_back(item);
	lock.unlock();
	cv.notify_all();
  }
 protected:
  std::list<T> log_list;
  std::mutex log_list_mutex;
  std::condition_variable cv;
};

}

#endif //ERIZO_ALL_PRODUCER_H
