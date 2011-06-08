
// ///////////////////////////////////////////// Include Files

#if defined(OC_FACTOR_INTO_H_AND_CC)
#  include "ocsynchronizedworker.h"
#endif 


OC_BEGIN_NAMESPACE

// ///////////////////////////////////////////// SynchronizedWorker Methods

// The Thread Main Loop
static void* SyncWorkerMainLoop (void* data) 
{
  SynchronizedWorker* sw = (SynchronizedWorker*)data;
  sw->process_();
  return NULL;
}


OC_INLINE SynchronizedWorker::SynchronizedWorker (const string& name,
					       bool shared_across_processes) :
  OCThread(name, false), // run so has to join
  doneExecuting_(false),
  workReady_(shared_across_processes),
  barrierReached_(shared_across_processes)
{
  start(SyncWorkerMainLoop, this); // Start the thread
}


OC_INLINE void SynchronizedWorker::process_ ()
{
  // Continue until 
  bool ack_done_exec = false;
  while (! ack_done_exec) {

    // Waiting for work to be ready
    {
      // Lock the mutex for this track/thread before waiting on it
      workReady_.lock();
      
      // Wait until there is some work for us to do or we're supposed
      // to exit, and then indicate the we got the work by setting it
      // back.
      while (! workReady_()) {
	volatile bool done = doneExecuting_;
	if (done) break;
	workReady_.wait();
      }
      workReady_() = false;
      ack_done_exec = doneExecuting_;
    
      workReady_.unlock();  // unlock so we can start over
    }


    // At this point, we are ready to do work.  Note that this is what
    // you want to override to do your work.
    if (! ack_done_exec) {  // only do work if not in final clean up mode
      try {
        dispatchWork_();
      } catch (const exception& e) {
        cerr << "SynchronizedWorker caught unhandled exception: " + Stringize(e.what());
      } catch (...) {
        cerr << "SynchronizedWorker caught unknown exception" << endl;
      }
    }


    // Indicate that we reached the barrier and are ready for more
    // work.
    {
      barrierReached_.lock();
      barrierReached_() = true;
      barrierReached_.signal();
      barrierReached_.unlock();  
    }

  } // End doneExecuting

}

OC_END_NAMESPACE


