
// ///////////////////////////////////////////// Include Files

// Currently only support inclusion template model
// #  include "ocworkercoordinatort.h"

OC_BEGIN_NAMESPACE

// ///////////////////////////////////////////// WorkerCoordinatorT Methods

template <class T>
OC_INLINE WorkerCoordinatorT<T>::~WorkerCoordinatorT () 
{
  // When this destructs, make sure he cleans up all his workers
  // correctly.
  removeAllWorkers();
}


template <class T>
OC_INLINE void WorkerCoordinatorT<T>::startAndSynchronizeAllWorkers () 
{
  int_u4 number_of_workers = workerThreads_.length();
  int_u4 worker;

  // Tell all the threads that we have work ready to go!
  for (worker=0; worker<number_of_workers; worker++) {
    
    // Get the single worker we are dealing with for this track
    SynchronizedWorker& w = *(workerThreads_[worker]);


    w.workReady_.lock();      // Must lock M2Mutex before bcast
    w.workReady_() = true;
    w.workReady_.signal();  // Tell other thread "ready to go"
    w.workReady_.unlock();    // Unlock 
  }


  // The threads have been told to work.  Now, we want for them
  // to all finish.

  
  // Barrier.  Wait until each track signals that it's done.  This is
  // a simple read barrier.  Once they have all been set, the threads
  // doing the work are done and we can reset the barrier in the
  // thread of the co-ordinating thread (namely, the thread animating
  // this routine).
  for (worker=0; worker<number_of_workers; worker++) {
        
    SynchronizedWorker& w = *(workerThreads_[worker]);

    w.barrierReached_.lock();       // Must lock M2Mutex before posixWait

    while (!w.barrierReached_())
      w.barrierReached_.wait(); // Wait for each thread to reach barrier

    // Once reached, can reset it and unlock the mutex
    w.barrierReached_() = false;
    w.barrierReached_.unlock();          
  }

}


template <class T>
OC_INLINE void WorkerCoordinatorT<T>::removeAllWorkers ()
{
  // Make sure have deleted that last bunch of workers properly.  To
  // do this, we tell each worker to "finish what it's doing", then
  // synchronize and wait until they all reach  
  int_u4 last_worker_count = workerThreads_.length();
  int_u4 worker;
  if (last_worker_count) {
    for (worker=0; worker<last_worker_count; worker++) {

      // Tell the worker it's done
      SynchronizedWorker& w = *(workerThreads_[worker]);
      w.doneExecuting_ = true;
    }

    // Synchronize with the threads
    startAndSynchronizeAllWorkers();

    // ... and make sure everyone has stopped:  Only necessary
    // if we are running DETACHED
    //////for (worker=0; worker<last_worker_count; worker++) {
    //////  SynchronizedWorker& w = *(workerThreads_[worker]);
    //////  w.waitup();
    ////// }
                           
    workerThreads_.clearAndDestroy();
  }
}

OC_END_NAMESPACE

