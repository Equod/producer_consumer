//
// Created by bandyer on 05/07/18.
//

#ifndef PATTERN_CONSUMER_H
#define PATTERN_CONSUMER_H

namespace pattern {

template<typename T>
class Consumer {
 public:
  virtual void Consume(T item) = 0;
};

}

#endif //ERIZO_ALL_CONSUMER_H
