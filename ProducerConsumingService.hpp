//
// Created by bandyer on 05/07/18.
//

#ifndef PRODUCINGSERVICE_H
#define PRODUCINGSERVICE_H

#include <thread>
#include "Producer.hpp"
#include "Consumer.hpp"

namespace pattern {

template<typename T>
class ProducerConsumingService : public Producer<T> {
 public:
  enum Mode { OnConstruction, Later };
  explicit ProducerConsumingService(Consumer<T>* consumer = nullptr, Mode mode = OnConstruction)
	  : keep_consumer_running{false}, consumer_{consumer} {
    if (mode == OnConstruction) {
	  StartConsumingService();
	}
  }

  void StartConsumingService() {
	std::lock_guard<std::mutex> lk(consuming_thread_mutex);
	if(!consuming_thread) {
	  keep_consumer_running = true;
	  consuming_thread = std::make_unique<std::thread>(consumer_runner);
	}
  }

  void setConsumer(Consumer<T>* consumer) {
	std::lock_guard<std::mutex> lk(consumer_mutex);
	ProducerConsumingService::consumer_ = consumer;
  }

  void StopConsumingService() {
	std::lock_guard<std::mutex> lk(consuming_thread_mutex);
	if (consuming_thread) {
	  keep_consumer_running = false;
	  consuming_thread->join();
	}
  }

 protected:
  void consumer_runner() {
    while (keep_consumer_running) {
	  std::unique_lock<std::mutex> locker(log_list_mutex);
	  cv.wait(locker, [this]() { return !log_list.empty(); });
	  auto item = std::move(log_list.front());
	  log_list.pop_front();
	  locker.unlock();
	  std::lock_guard<std::mutex> lk(consumer_mutex);
	  consumer_->Consume(item);
	}
  }

 protected:
  Consumer<T>* consumer_;
  std::mutex consumer_mutex;
  bool keep_consumer_running;
  std::unique_ptr<std::thread> consuming_thread;
  std::mutex consuming_thread_mutex;
};

}

#endif // PRODUCINGSERVICE_H
