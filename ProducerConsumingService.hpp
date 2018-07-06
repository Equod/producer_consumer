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
  struct StartMode{ enum Mode { OnConstruction, Later }; };
  explicit ProducerConsumingService(Consumer<T>* consumer = nullptr,
  	typename StartMode::Mode mode = StartMode::OnConstruction)
	  : keep_consumer_running{false}, consumer_{consumer} {
    if (mode == StartMode::OnConstruction) {
	  StartConsumingService();
	}
  }

  virtual ~ProducerConsumingService() {
	StopConsumingService();
  }

  void StartConsumingService() {
	std::lock_guard<std::mutex> lk(consuming_thread_mutex);
	if(!consuming_thread) {
	  keep_consumer_running = true;
	  consuming_thread = std::make_unique<std::thread>(consumer_runner, this);
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
	  Producer<T>::semaphore_.notify();
	  consuming_thread->join();
	}
  }

 protected:
  static void consumer_runner(ProducerConsumingService* this_service) {
    while (this_service->keep_consumer_running) {
	  this_service->log_list_mutex.lock();
	  if(!this_service->log_list.empty()) {
		auto item = std::move(this_service->log_list.front());
		this_service->log_list.pop_front();
		this_service->log_list_mutex.unlock();
		std::lock_guard<std::mutex> lk(this_service->consumer_mutex);
		this_service->consumer_->Consume(item);
	  } else {
		this_service->log_list_mutex.unlock();
		this_service->semaphore_.wait();
	  }
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
