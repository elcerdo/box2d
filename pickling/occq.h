#ifndef OCCQ_H_

// Includes

#include "ocval.h"
#include "octhread.h"

OC_BEGIN_NAMESPACE

// A circular Queue with synchronization
class CQ {
 public:

  // Create a circular queue of the appropriate length
  CQ (int len) :
    q_(len),
    m_(),    // Initialize mutex:  ORDER DEPENDENCY
    full_(m_),  // --Have condvars full and empty share mutex,
    empty_(m_)  // --both associated with same q
  { }

  // Enqueue the packet, blocks if not enough room
  void enq (const Val& data)
  {
    // Can't enque if nothing there ... block
    m_.lock();
    while (q_.full()) {
      full_.wait();
    }
    q_.put(data);
    empty_.broadcast();
    m_.unlock();
  }

  // Dequeue, blocks if no room
  Val deq ()
  {
    // Can't deque if nothing there ... block
    m_.lock();
    while (q_.empty()) {
      empty_.wait();
    }
    Val data = q_.get();
    full_.broadcast();
    m_.unlock();

    return data;
  }

 protected:
  CircularBuffer<Val> q_;
  Mutex m_;        // Protects q:  used with both full_ and empty_ below
  CondVar full_;   // Mark when queue is full, so have to wait to put
  CondVar empty_;  // Mark when queue is empty, so have to wait to get

}; // CQ

OC_END_NAMESPACE

#define OCCQ_H_
#endif // OCCQ_H_
