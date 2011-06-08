#ifndef M2PYTHONPICKLER_H_

#include <stdio.h>
#include <ctype.h>
#include "m2pythontools.h"

// Distinguish between M2k and OpenContainers usage
#if defined(OC_USE_OC) || defined(OC_NEW_STYLE_INCLUDES)
PTOOLS_BEGIN_NAMESPACE
#endif

// Classes for helping with Pickling and Depickling.  The way Vectors
// changes serialization depending on if the Package on the other side
// supports the Python package "Numeric" which has built-in arrrays.
// If Numeric is not used, Vectors are turned into Python Lists.

// Helper class for PM Context Stack: as objects are pushed and
// popped, this is the stack.  We have an extra piece of information
// so we can note pops_ and memoize correctly
template <class OBJ>
struct PStack_ { 
  PStack_ (const OBJ& v) :
    memo_number(-1),
    object(v)
  { }
  int memo_number;  // -1 means never memoized, otherwise offset into 
  OBJ  object;         // the actual object on the stack

}; // PStack_

// If we have a pointer to an OBJ in a PStack instance, return
// a pointer to the PStack instance.
template <class OBJ>
inline PStack_<OBJ>* ConvertFromObjPtrToPStackPtr (OBJ* obj_ptr)
{
  static PStack_<OBJ> dummy = OBJ();  // Just kept around for address arithmetic
  static char* dummy_front = (char*) &dummy;
  static char* dummy_inside = (char*) &dummy.object;
  static int diff = dummy_inside - dummy_front;
  
  char* obj = (char*) obj_ptr;
  char* pst = obj - diff;
  PStack_<OBJ>* ret_val = (PStack_<OBJ>*) pst;
  // NOTE: we could use offsetof, but I have found it to be non-portable
  // (i.e., some compilers don't support it).
  return ret_val;
}



// Helper class for memoization: Sometimes we store the values by
// pointer, sometimes by value.
template <class OBJ>
struct MemoInfo_ {
  MemoInfo_ (OBJ* o) : object_ptr(o==0? &object : o) { }

  OBJ* object_ptr;  // points to the actual full stack if item hasn't been pop:
                    // ALWAYS VALID: sometimes points to object (below),
                    // sometimes points to item on Pickle Stack

  OBJ object;       // Full object once been copied

  // If the objectPtr points the object inside, in the copy, it
  // continues to do that.  Other wise, the object_ptr points to
  // whatever it originally pointed to.
  MemoInfo_ (const MemoInfo_<OBJ>& rhs) :
    object(rhs.object)
  { if (&rhs.object == rhs.object_ptr) {
      object_ptr = & object;
    } else {
      object_ptr = rhs.object_ptr;
    }
  }
};  // MemoInfo_


// M2kand MITE Arrays DO NOT (currently) support SWAP!
// We have to work around this: WE NEED SWAP so that the
// new memos stay in place
#if !defined(OC_USE_OC)

template <class T>
void Swapping (T& lhs, T& rhs)
{
  T temp = lhs;
  lhs = rhs;
  rhs = temp;
}

template <class T>
class ArraySupportsSwapping : public Array<T> {
  public:

  // Don't use swap, because we will add it to MITE at some point
  void swapping (ArraySupportsSwapping<T>& rhs)
  {
    Swapping(this->length_, rhs.length_);
    Swapping(this->capac_, rhs.capac_);
    Swapping(this->useNewAndDelete_, rhs.useNewAndDelete_);
    Swapping(this->data_, rhs.data_);
  }

}; // ArraySupportsSwapping
#endif



// ///// PythonDepickerA 
// Class to do all the unpickling, but I/O mechanism is left to
// subclass.
template <class OBJ>
class PythonDepicklerA {

 public:
  
  PythonDepicklerA (bool with_numeric_package, bool use_proxies=true);
  virtual ~PythonDepicklerA ();
  
  // Load a pickled OBJ from the given 
  OBJ load ();

  // Inspector/Mutator for the compatibility mode: by default, Tups
  // and OTab (introduced in PicklingTools 1.2.0) aren't available, so
  // we have to "simulate" them with Arr and Tab (respectively).
  bool compatibility () const { return compat_; }
  void compatibility (bool disallow_tup_and_otab) 
  { compat_=disallow_tup_and_otab; }
  
 protected:
  
  // Get a single character:  return EOF if hits EOF
  virtual int getChar_ () = 0;
  
  // Returns chars on input until we hit a newline, at which point it
  // replaces the newline with a \0 (giving a c-style string).  We
  // consume all the characters upto/including the newline.  (Most
  // pickled entries end with a newline).
  virtual char* getUntilNewLine_ (int& len) = 0;
  
  // Get a string
  virtual char* getString_ (int& len) = 0;
  

  // Pop the stop of the PM Context stack and return it.  When we pop,
  // if this value had been memoized, then we need to copy the "full
  // copy" back to the memoize area.
  OBJ pop_ ()
  { 
    OBJ ret; // Return Value Optimization

    // When we pop, if this item has a memoization, we copy the memo back
    // over to memo area
    int index_of_last = stack_.length()-1;
    PStack_<OBJ>& p = stack_[index_of_last];
    ret = p.object;

    // If this memoized, copy memo back to memo area
    int memo_number = p.memo_number;
    if (memo_number!=-1) {
      MemoInfo_<OBJ>& m = memo_[memo_number-1]; // Old memo: 1 based!!
      m.object = p.object;                    // copy full object
      m.object_ptr = &m.object;               // have pointer point HERE 
    }
    (void)stack_.removeLast(); 
    return ret;
  }

  // Look at the top of the PM Stack: "peek" into the top (or a few
  // down) from the top of the stack.  0 means the top, -1 means 1
  // from the top, etc.
  OBJ& top_ (int offset=0) 
  { 
    int index_of_entry = stack_.length()+offset-1;
    PStack_<OBJ>& p = stack_[index_of_entry];
    return p.object;
  }

  // Push an item on top of the PM Context stack.
  void push_ (const OBJ& v) 
  {
    // If a resize happens on the next append MOST MEMOS WILL BECOME
    // INVALID.  We have to copy the data and revalidate the memos.
    if (stack_.length() == stack_.capacity()) {
      // Make a copy, with bigger capacity so next append won't cause
      // resize.  We do this "manually" so we can have a copy of both
      // stacks at the same time to adjust memos correctly
      Array<PStack_<OBJ> > new_stack(stack_.length()*2); 
      int original_stack_len = stack_.length();
      for (int ii=0; ii<original_stack_len; ii++) {
	new_stack.append(stack_[ii]);
      }
      // Now, we have both stacks , so we can readjust memos
      PStack_<OBJ>* front_old_stack = &stack_[0];
      PStack_<OBJ>* front_new_stack = &new_stack[0];
      for (size_t jj=0; jj<memo_.length(); jj++) {
	MemoInfo_<OBJ>& m = memo_[jj];
	// Only have to readjust memo if points into stack
	// (not to itself)
	if (m.object_ptr != &m.object) { 
	  // Point to new_stack object instead
	  PStack_<OBJ> *old_stack_place = ConvertFromObjPtrToPStackPtr(m.object_ptr);
	  int diff = old_stack_place - front_old_stack;
	  PStack_<OBJ> *new_stack_place = front_new_stack + diff;
	  m.object_ptr = &new_stack_place->object;
	}
      }
      // Finally, install new bigger stack, where memos point to this
      // one! WE NEED SWAP so that the new memos stay in place
#if defined(OC_USE_OC)
      stack_.swap(new_stack);
#else 
      // M2kand MITE Arrays DO NOT (currently) support SWAP!
      // We have to work around this: 
      ArraySupportsSwapping<OBJ>* stackp=(ArraySupportsSwapping<OBJ>*)&stack_;
      ArraySupportsSwapping<OBJ>* new_stackp=(ArraySupportsSwapping<OBJ>*)&new_stack;
      stackp->swapping(*new_stackp);
#endif
    }
    // Big enough.
    stack_.append(PStack_<OBJ>(v)); 
  }
  
  // Dispatches foe the different pickle tokens
  void ploadNONE_ ();
  void ploadGLOBAL_ ();
  void ploadINT_ ();
  void ploadLONG_ ();
  void ploadFLOAT_ ();
  void ploadSTRING_ ();
  void ploadMARK_ ();
  void pPUT_ ();
  void pGET_ ();
  void ploadLIST_ ();
  void ploadTUPLE_ ();
  void ploadREDUCE_ ();
  void pAPPEND_ ();
  void pSETITEM_ ();
  void pEMPTY_DICT_ ();
  void pDICT_ ();

  void reduceComplex_ (const OBJ& tuple);
  void reduceArrays_ (const OBJ& tuple);
  void reduceOrderedDict_ (const OBJ& tuple);

  // ///// Data Members 
  
  // The PM stack, as described by pickletools.  Every item on the
  // stack has "possibly" been memoized, which means when it is
  // popped, we need to copy the value out into the memo stack: that's
  // why we record the memo number along with the value (so we know
  // what its memo is).
  Array<PStack_<OBJ> > stack_; 

  // The "memo" (named values) as per pickletools.  More like a table
  // where we associate entries (memo numbers) with their Values.
  // While the values haven't been popped, the value is stored by
  // pointer.  When it is finally popped, its value is copied to the
  // memo stack.
  Array<MemoInfo_<OBJ> > memo_ ; 

  Array<int> mark_;  // remember where we start marks ... 
  bool withNumericPackage_; // Do we expect the other end to support Numeric?

  // So, previous incarnations ALWAYS returned full-copies.  With the
  // advent of Proxy support in OpenContainers, it is possible to
  // return a table which is a PROXY to a table, not a Tabel itself.
  // We at least give the user the option for both.  By default, it
  // returns the "new" way, with proxies.
  bool wantToProxyize_;

  // Allow PTOOLS to be backward compatible with previous versions
  // already installed: if true, all OTabs and Tups and serialized as
  // Tabs and Arr (resp.).  BY DEFAULT, we probably need to backward
  // compatible: the user must EXPLICITLY request OTab and Tup (we
  // don't want to break any old installs).
  // However, this may vary from install to install, so you can
  // set the "default" as a macro: -DOC_SERIALIZE_COMPAT=true
  // or -DOC_SERIALIZE_COMPAT=false.
  //
  // Of course, the version string from the server helps us...
  bool compat_;
}; // PythonDepicklerA 


// Unpickle data from a file
template <class OBJ>
class PythonDepickler : public PythonDepicklerA<OBJ> {
 public:

  enum { PMC_BUFF_EXPAND = 2048 };
  
  PythonDepickler (const string& name, bool with_numeric_package);
  virtual ~PythonDepickler ();

 protected:

  // Satsify interface
  virtual int getChar_ ();
  virtual char* getUntilNewLine_ (int& len);
  virtual char* getString_ (int& len);

  // ///// Data Members
  FILE* fp_;
  Array<char> buffer_; // Buffer for fgets

}; // PythonDepickler


// Depickle from a inplace memory buffer
template <class OBJ>
class PythonBufferDepickler : public PythonDepicklerA<OBJ> {
 public:

  // The buffer is NOT adopted, but referenced.  Note: The buffer MAY
  // BE modified while using it!  If you want to keep the buffer, make
  // sure you copy it out first.
  PythonBufferDepickler (int_u4 buff_len, char* buff, 
			 bool with_numeric_package);
  virtual ~PythonBufferDepickler ();

 protected:

  // Satsify interface
  virtual int getChar_ ();
  virtual char* getUntilNewLine_ (int& len);
  virtual char* getString_ (int& len);

  // ///// Data Members
  int_u4 buffLen_; // The length of the referenced buffer
  char*  buff_;    // The buffer:  referenced, NOT adopted
  int_u4 start_;   // Start of the buffer
  
}; // PythonBufferDepickler



// Class used for pickling data (protocol 0)
template <class OBJ>
class PythonPicklerA {
public:

  PythonPicklerA (bool with_numeric_package);

  // Your derived destructor MUST CALL putChar_(PY_STOP) so that it
  // puts the final stop mark in!
  virtual ~PythonPicklerA ();

  void dumpBool (const OBJ& val);
  void dumpVector (const OBJ& val, int int_handle=-1);
  void dumpString (const string& val, bool translate_escapes=true);
  void dumpTable (const OBJ& val, int int_handle=-1);
  void dumpOrderedDict (const OBJ& val, int int_handle=-1);
  void dumpTuple (const OBJ& val, int int_handle=-1);
  void dumpProxy (const OBJ& val);
  void dumpList (const OBJ& val, int int_handle=-1);
  void dumpNumber (const OBJ& val);

  // General Serializer
  void dump (const OBJ& val);

  // Mutator/Inspector pair for checking or setting the warning state
  bool warn () const { return warn_; }
  void warn (bool set_to) { warn_ = set_to; }

  // Inspector/Mutator for the compatibility mode: by default, Tups
  // and OTab (introduced in PicklingTools 1.2.0) aren't available, so
  // we have to "simulate" them with Arr and Tab (respectively).
  bool compatibility () const { return compat_; }
  void compatibility (bool disallow_tup_and_otab) 
  { compat_=disallow_tup_and_otab; }


protected:

  // Put the appropriate stuff onto the output
  virtual void putChar_ (char c) = 0;
  virtual void putStr_ (const char* s) = 0;
  virtual void putStr_ (const string& str) = 0;

  // Dump depending on how we interact with Numeric
  void dumpVectorAsList_ (const OBJ& v, int int_handle=-1);
  void dumpNumericArray_ (const OBJ& v, int int_handle=-1);

  // Helper function: when doing get or put, need to be able to output number
  void placeHandle_(int int_handle, char code);


  // ///// Data Members

  // Are we considering the Numeric package?
  bool withNumericPackage_;

  // Map between ptr_handles (inside the proxy) and int_handles (the
  // small int that goes into the serialization).
  HashTableT<void*, int, 8> handles_;

  // What would be the "next" handle if we had to get one.
  int currentIntHandle_;

  // Indicate whether or not we are being verbose with warnings
  bool warn_;

  // Allow PTOOLS to be backward compatible with previous versions
  // already installed: if true, all OTabs and Tups and serialized as
  // Tabs and Arr (resp.).  BY DEFAULT, we probably need to backward
  // compatible: the user must EXPLICITLY request OTab and Tup (we
  // don't want to break any old installs).
  // However, this may vary from install to install, so you can
  // set the "default" as a macro: -DOC_SERIALIZE_BACK_COMPAT=true
  // or -DOC_SERIALIZE_BACK_COMPAT=false.
  //
  // Of course, the version string from the server helps us...
  bool compat_;

}; // PythonPickler


// Class used for pickling data to a FILE
template <class OBJ>
class PythonPickler : public PythonPicklerA<OBJ> {
 public:

  PythonPickler (const string& name, bool with_numeric_package);
  virtual ~PythonPickler ();

protected:

  // Put the appropriate stuff onto the output
  virtual void putChar_ (char c);
  virtual void putStr_ (const char* s);
  virtual void putStr_ (const string& s);

  // ///// Data Members
  FILE* fp_;

}; // PythonPickler


// Class used for pickling data to a memory buffer
template <class OBJ>
class PythonBufferPickler : public PythonPicklerA<OBJ> {
 public:
  
  PythonBufferPickler (Array<char>& buffer, bool with_numeric_package);
  virtual ~PythonBufferPickler ();

  protected:

  // Put the appropriate stuff onto the output
  virtual void putChar_ (char c);
  virtual void putStr_ (const char *s);
  virtual void putStr_ (const string& s);

  // ///// Data Members
  Array<char>& buffer_;

}; // PythonBufferPickler

// Distinguish between M2k and OpenContainers usage
#if defined(OC_USE_OC) || defined(OC_NEW_STYLE_INCLUDES)
PTOOLS_END_NAMESPACE
#endif

#define M2PYTHONPICKLER_H_

#ifdef MIDAS_COMPILER_TEMPLATES
#  include "m2pythonpickler.cc"
#endif

#endif // M2PYTHONPICKLER_H_
