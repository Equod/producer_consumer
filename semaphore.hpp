//
// Created by Equod on 06/07/18.
//

#ifndef PATTERN_SEMAPHORE_H
#define PATTERN_SEMAPHORE_H

#include <mutex>
#include <condition_variable>

namespace pattern {

class semaphore
{
 public:
  void notify() {
	std::unique_lock<decltype(mutex_)> lock(mutex_);
	++count_;
	condition_.notify_one();
  }

  void wait() {
	std::unique_lock<decltype(mutex_)> lock(mutex_);
	while(!count_) // Handle spurious wake-ups.
	  condition_.wait(lock);
	--count_;
  }

  bool try_wait() {
	std::unique_lock<decltype(mutex_)> lock(mutex_);
	if(count_) {
	  --count_;
	  return true;
	}
	return false;
  }

 private:
  std::mutex mutex_;
  std::condition_variable condition_;
  unsigned long count_ = 0; // Initialized as locked.
};

}

#endif // PATTERN_SEMAPHORE_H
