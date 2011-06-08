#ifndef MIDASSOCKET_H_

// The internals of the MidasServer and MidasTalker share a lot of
// code (especially to do primitive reads and primitive writes to the
// socket).  This module encapsulates all that code.

#include <unistd.h>
#include <netinet/in.h>
#include <signal.h>
#include "ocsynchronizer.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "chooseser.h"

// Force the struct hostent into the global environment, even with just
// a silly typedef
typedef struct hostent for_sill;

PTOOLS_BEGIN_NAMESPACE 

// Forward
class StreamDataEncoding;


// Do you use two sockets or 1 socket for full duplex communication?
// Some very old versions (VMWARE) only supported single duplex sockets,
// and so full duplex sockets had to be emulated with 2 sockets.
enum SocketDuplex_e {
  SINGLE_SOCKET = 0,
  DUAL_SOCKET   = 1
};


// This is a system structure for managing sockaddr from accepts.  It
// also shows how "non-Val" things can be converted to and from Vals.
// THIS WORKS BECAUSE SOCKADDR can be passed by value.
struct SockAddr_ {

  // Basic data of struct
  struct sockaddr addr;
#if defined(OSF1_)
  int             addrlen;
#else
  socklen_t       addrlen;
#endif

  SockAddr_ () : addrlen(sizeof(addr)) { }

  // Allow users to construct a SockAddr_ from Val, if legal
  SockAddr_ (const Val& sa)
  {
    if (sa.tag=='a') {
      OCString* sp = (OCString*) &sa.u.a;
      OCString& s = *sp;
      memcpy(&addr, &s[0], s.length());
      addrlen = s.length();
    } else {
      throw runtime_error("Don't know how to convert:"+string(sa)+" to SockADdr_");
    }
  }

  // Allow users to construct a Val from a SockAddr_
  operator Val () const 
  { // Just bit-blit into a string
    Val retval;
    string s((char*)&addr, addrlen);
    retval = s;
    return retval;
  }

}; // SockAddr_


// Dumb helper routine for debugging
inline void debugArray_ (Array<char>& buffer, const char* routine)
{
  cerr << "****" << routine << "****" << endl;
  for (int ii=0; ii<int(buffer.length()); ii++) {
    char c = buffer[ii];
    if (isprint(c)) cerr << c;
    else {
      char s[4] = { 0,0,0,0 };
      s[0] = 'x';
      int g = (c >> 4);
      s[1] = g["0123456789ABCDEF"];
      s[2] = (c &0x0F)["0123456789ABCDEF"];
      s[3] = 0;
      string hexer = s;
      cerr << "\\0" << hexer;
    }
  }
}


// A base class with some primitive I/O facilities for MidasServer and
// MidasTalker
class MidasSocket_ {
 public:

  // Create MidasSocket_ that either Python Pickles or sends strings
  // as is. This also chooses whether or not to use Numeric or Arrays
  // in Array serialization.  For backwards compat, AS_LIST is 1
  // (being true), AS_NUMERIC is 0 (being false) and the new
  // AS_PYTHON_ARRAY is 2.

  // Adaptive means that the MidasSocket keeps track of the type of
  // serialization used in the messages sent/received over the
  // socket.  If possible, we will talk to a server or client in
  // the serialization they talked to us.  If there is ever a doubt
  // as to what serialization to use, the serialization parameters
  // passed (in the constructor) here are used.  If adapative is
  // sent to false, then ALWAYS use the serialization parameters
  // given here: This is probably most important if you are using
  // SERIALIZE_NONE

  // In UNIX systems, issues with the socket cause a UNIX signal to
  // occur: we almost always want that signal ignored because we can
  // basically detect the socket going away.
  MidasSocket_ (Serialization_e serialization=SERIALIZE_P0,
		ArrayDisposition_e disposition=AS_LIST,
		bool ignore_sigpipe=true,
		bool adaptive=true) :
    adaptive_(adaptive),
    serialization_(serialization),
    arrayDisposition_(disposition),
    compatibilityMode_(false)
  {
    if (ignore_sigpipe) installSIGPIPE_ignore();
  }

  // To preserve compatibility with pre-existing servers and talkers,
  // OTab->Tab, Tup->Arr, BigInt->Str.  You really only need this if
  // you plan to send those (OTab, Tup, BigInt) to multiple
  // servers/client where there's a mix of legacy and newer.
  // Otherwise, just don't send those data structure over.  TRUE means
  // convert OTab/Tup/BigInt, FALSE (default) is just try to pass
  // things on as-is (no conversions).
  void compatibilityMode (bool mode) { compatibilityMode_ = mode; }

  virtual ~MidasSocket_ () { }

 protected:

  // ///// Data Members

  bool adaptive_;  // If true: Prefer to just look at headers to see
                   // what kind of serialization to use.  When
                   // questions arise as to what serialization to use,
                   // use defaults below (supplied with constructor)

  Serialization_e serialization_;       // Serialization:M2k, Python, OC, None?
  ArrayDisposition_e arrayDisposition_; // How do arrays of POD serialize?

 
  bool compatibilityMode_; // TRUE if we convert OTab/Tup/BigInt->Tab/Arr/Str
                           // FALSE (default) if we pass as-is. 

  string header_;          // Header is PY00 for Non-Numeric, PYN0 for Numeric,
                           //           PYA0 for python Arrays
  Mutex gethostbynameLock_; // Lock to thread-safe gethostbyname_r


  Mutex conversationsLock_;  // Lock activity on conversations table 
  Tab conversations_;        // key: file descriptors value: list of 
                             //     (serialization, array_disposition)
  Tab readWriteAssociations_; // Be able to get from read fd to write fd
                              // Use lock for converstaions

  // make it so you can get from read fd to write fd and vice-versa
  void readWriteAssociate_(int read_fd, int write_fd)
  {
    ProtectScope ps(conversationsLock_);
    if (read_fd!=-1)
      readWriteAssociations_[read_fd] = write_fd;
    if (write_fd!=-1)
      readWriteAssociations_[write_fd] = read_fd;
  }

  // When the socket closes, take file desc out of the table and also
  // clear out the conversation record so we start fresh
  void readWriteDisassociate_ (int read_fd, int write_fd)
  {
    ProtectScope ps(conversationsLock_);
    if (readWriteAssociations_.contains(read_fd)) {
      readWriteAssociations_.remove(read_fd);
    }
    if (readWriteAssociations_.contains(write_fd)) {
      readWriteAssociations_.remove(write_fd);
    }
    if (conversations_.contains(read_fd)) {
      conversations_.remove(read_fd);
    }
  }

  // Choose what header to create with given serialization choices
  static string createHeader_ (Serialization_e serialization, 
			       ArrayDisposition_e array_disposition)
  {
    string header;
    switch (array_disposition) {
    case AS_NUMERIC:       header = "PYN0";  break;
    case AS_LIST:          header = "PY00";  break;
    case AS_PYTHON_ARRAY:  header = "PYA0";  break;
    default: throw runtime_error("Unknown array disposition");
    }
    switch (serialization) {
    case SERIALIZE_P0:     header[3] = '0'; break;
    case SERIALIZE_P2:     header[3] = '2'; break;
    case SERIALIZE_P2_OLD: header[3] = '-'; break;
    case SERIALIZE_NONE:   header    = ""; break;
    case SERIALIZE_M2K:    header    = "M2BD"; break; 
    case SERIALIZE_OC:     header    = "OC00"; break;
    default: throw runtime_error("Unknown serialization");
    }
    return header;
  }
 

  // Choose the receive serialization.  If the user has choosen NOT to
  // use adaptive serialization, this forces the values from the
  // constructor to be used as the serialization choice.  Otherwise,
  // the serialization choice is based on the currently passed in
  // header.  The file descriptor is so we can RECORD the
  // serialization choice for the return conversation.
  Serialization_e chooseRecvSerialization_ (int read_fd, const string& hdr,
					    ArrayDisposition_e& array_dis)
  {
    // Force what we want to recv, even if the header lies
    if (!adaptive_) {
      array_dis = arrayDisposition_;
      return serialization_;
    }
    
    // Otherwise, use the header to guide us
    Serialization_e serialization = SERIALIZE_NONE;
    array_dis = AS_LIST; // Really doesn't matter, just not unititalized!

    switch (hdr[0]) {
    case 'O': // hopefully OC
      if (hdr[1]=='C') {
	serialization = SERIALIZE_OC;
      }
      break;

    case 'M': // hopefully M2K
      if (hdr[1]=='2' && hdr[2]=='B' && hdr[3]=='D') {
	serialization = SERIALIZE_M2K;
      }
      break;

    case 'P': // hopefully Python of some flavor
      if (hdr[1]=='Y') {
	// Choose serialization
	switch (hdr[3]) {
	case '0': serialization = SERIALIZE_P0;     break;
	case '2': serialization = SERIALIZE_P2;     break;
	case '-': serialization = SERIALIZE_P2_OLD; break;
	}
	// Choose Array Disposition
	switch (hdr[2]) {
	case 'N': array_dis = AS_NUMERIC;       break;
	case 'A': array_dis = AS_PYTHON_ARRAY;  break;
	case '0': 
	default:  array_dis = AS_LIST;          break;
	}
      }
      break;

    default:  // hopefully raw
      break;     // NONE When you don't know what to do, rather than throw 
    }
   
    // We've recieved a header, log this information so that
    // a send will serialize the way it was received
    Arr options("[0,0]");
    options[0] = int(serialization);
    options[1] = int(array_dis);
    { 
      ProtectScope ps(conversationsLock_);
      conversations_[read_fd] = options;
    }

    return serialization;
  }

  // Choose the kind of serialization to use for a send.  To send, we
  // either have to default to what was in the constructor if we've
  // never seen any information, or use the conversation information
  // we have.
  virtual Serialization_e chooseSendSerialization_ (int write_fd,
						    ArrayDisposition_e& array_dis)
  {
    Serialization_e serialization = serialization_;

    // IF adaptive: Check the table to see if there is already a
    // conversation with the server.
    bool contains = false;
    int read_fd;
    Arr how_server_talked_to_us;
    if (adaptive_) {
      ProtectScope ps(conversationsLock_);
      //if (!readWriteAssociations_.contains(write_fd)) return;// Early return
      read_fd = readWriteAssociations_(write_fd); // ... exception if not there
      contains = conversations_.contains(read_fd);
      if (contains) {
	how_server_talked_to_us = conversations_(read_fd);
      }
    }

    // Adaptive...do we have a header from this so we know what to send? 
    if (adaptive_ && contains) {
      serialization = Serialization_e(int(how_server_talked_to_us[0]));
      array_dis     = ArrayDisposition_e(int(how_server_talked_to_us[1]));
    }

    // Not adaptive, just force the serialization
    else {     
      serialization = serialization_;
      array_dis     = arrayDisposition_;
    }

    return serialization;
  }

  // Blocking call to get next Val off of socket.
  Val recvBlocking_ (int fd) 
  {
    Val retval;

    // Preamble: number of bytes to read (doesn't include 4 bytes hdr, next)
    int_u4 bytes_to_read = 0;
    readExact_(fd, (char*)&bytes_to_read, sizeof(bytes_to_read));
    bytes_to_read = ntohl(bytes_to_read);

    // Read the header and decide what kind of serialization we have
    char rep[4] = { 0 };
    readExact_(fd, &rep[0], sizeof(rep)); 
    string hdr(rep, sizeof(rep));
    ArrayDisposition_e array_disposition;
    Serialization_e serialization = chooseRecvSerialization_(fd, hdr, 
							    array_disposition);

    // Set-up array to read into
    Array<char> buffer(bytes_to_read+1); // Extra 1 space for zero for M2k
    buffer.expandTo(bytes_to_read);
    buffer.data()[bytes_to_read] = '\0'; // zero for M2k, doesn't hurt others
    
    // Handle reading the different types of headers
    int correction = 0;
    MachineRep_e endian = NativeEndian();
    switch (serialization) {
    case SERIALIZE_M2K :   correction = handleReadingM2kHdr_(fd, rep, 
							     endian); break;
    case SERIALIZE_OC:
    case SERIALIZE_P0:
    case SERIALIZE_P2:
    case SERIALIZE_P2_OLD: correction = 0;  break;
    case SERIALIZE_NONE: // In case we read 4 bytes prematurely
      memcpy(buffer.data(), rep, sizeof(rep)); 
      correction = sizeof(rep);
      break;
    default: throw runtime_error("unknown serial:"+Stringize(serialization));
    };
    readExact_(fd, buffer.data()+correction, bytes_to_read-correction);
    unpackageData_(buffer, serialization, array_disposition,
		   retval, endian);
    return retval;
  }

  // Helper blocking read method
  void readExact_ (int fd, char* buff, int bytes) 
  {
    // Sometimes reads get broken up... it's very rare (or
    // impossible?)  to get an empty read on a blocking call.  If we
    // get too many of them, then it probably means the socket went
    // away.
    int empty_retries = 1000;
    char* n_buff = buff;
    int bytes_to_read  = bytes;
    while (bytes_to_read) {
      int r = read(fd, n_buff, bytes_to_read);
      if (r<0) errout_("read");   
      // Instead of getting a SIGPIPE, when we ignore the SIGPIPE signal,
      // a number of "empty retries" is equivalent to the SIGPIPE, but then
      // at least we catch it.
      if (r==0 && --empty_retries<0) errout_("read: too many empty retries");
      bytes_to_read -= r;
      n_buff        += r;
    }
  }

  // Helper function to pack data (Pickled, raw, M2k, etc.), and
  // return the packed data in the given buffer: Kept for backwards
  // compatibility (but really just calls generic routines).
  void packageData_ (const Val& val, Serialization_e serialization, 
		     ArrayDisposition_e array_disposition,
		     Array<char>& buffer, MachineRep_e endian)
  {
    DumpValToArray(val, buffer, serialization, 
		   array_disposition, compatibilityMode_, endian);
  }
  void packageData_ (const Val& val, Serialization_e serialization, 
		     ArrayDisposition_e array_disposition,
		     Array<char>& buffer)
  {
    DumpValToArray(val, buffer, serialization, 
		   array_disposition, compatibilityMode_, NativeEndian());
  }
  
  // Helper function to unpackage the data for the different types of
  // serialization (Pickling, raw data, etc.). The unpackaging policy
  // is dictated by the arguments.
  void unpackageData_ (Array<char>& buffer, Serialization_e serialization, 
		       ArrayDisposition_e array_disposition,
		       Val& retval, MachineRep_e endian=MachineRep_EEEI)
  {
    LoadValFromArray(buffer, retval, serialization, 
		     array_disposition, compatibilityMode_, endian);
  }

  // Blocking call to send Val over socket.
  void sendBlocking_ (int fd, const Val& val)
  {
    // Pickle into buffer
    Array<char> buffer(1024);
    ArrayDisposition_e array_disposition;
    Serialization_e serialization = 
      chooseSendSerialization_(fd, array_disposition);
    // Package it up
    MachineRep_e endian=NativeEndian();
    packageData_(val, serialization, array_disposition,
		 buffer, endian);

    // Preamble: number of bytes to write
    int_u4 bytes_to_write = buffer.length();
    bytes_to_write = htonl(bytes_to_write);
    writeExact_(fd, (char*)&bytes_to_write, sizeof(bytes_to_write));

    // Headers support:  M2k is complex mess, Python is just 4 bytes
    if (serialization == SERIALIZE_M2K) {
      handleWritingM2kHdr_(fd); 
    } else if (serialization != SERIALIZE_NONE) {
      // Header (but only if serialized): 4 bytes of supports Numeric,version #
      string header = createHeader_(serialization, array_disposition); 
      writeExact_(fd, &header[0], 4);
    }

    // Write pickled buffer
    writeExact_(fd, buffer.data(), buffer.length());
  }


  // Helper, blocking write method
  void writeExact_ (int fd, char* buff, int bytes) 
  {
    // Sometimes writes get broken up... it's very rare (or
    // impossible?)  to get an empty write on a blocking call.  If we
    // get too many of them, then it probably means the socket went
    // away.
    int empty_retries = 1000;
    char* n_buff = buff;
    int bytes_to_write  = bytes;
    while (bytes_to_write) {
      int r = write(fd, n_buff, bytes_to_write);
      if (r<0) errout_("write");
      if (r==0 && --empty_retries<0) errout_("write: too many empty retries");
      bytes_to_write -= r;
      n_buff         += r;
    }
  }

  // This routine turns a m.n seconds into a timestruct with (seconds,
  // microseconds).  Also, if the timeout value is "None", the time
  // val struct us set to NULL meaning that select 
  struct timeval* convertTime_ (struct timeval* tv, Val timeout_in_seconds) 
  {
    // No timeout, set to null: This causes select to block indef.
    if (timeout_in_seconds == None) return NULL;

    // Convert timeout in m.n seconds to (seconds and microseconds)
    real_8 full_timeout = timeout_in_seconds;    
    tv->tv_sec  = long(full_timeout); // Get seconds
    real_8 t = (full_timeout-real_8(tv->tv_sec))*1e6;
    tv->tv_usec = long(t);            // Get microseconds
    
    return tv;
  }


  // Helper function: This function returns True immediately if the
  // given file descriptor (for a socket) is available to read without
  // blocking.  If the socket is NOT available, then the socket is
  // watched for timeout seconds: if something becomes available on the
  // socket in that time, this returns True, otherwise the entire
  // timeout passes and this returns False.  Note that this method
  // allows a user to 'poll' a read socket without having to do an
  // actual read.  A timeout of None is forever, a timeout of 0 is no
  // wait.
  bool dataReady_ (int read_fd, Val timeout_in_seconds=0);

  // All exceptions thrown by the MidasTalker are runtime_errors
  void errout_ (const string& message) 
  {
    string m = strerror(errno);
    throw runtime_error(message+":"+m);
  }

  // Definitely a UNIX thing: if you see a SIGPIPE signal, ignore it
  void installSIGPIPE_ignore ();

  // Select can return EINTR which is not truly a timeout,
  // so we need to have some code around select to handle this
  int mySelect_ (int nfds, fd_set* readfds, fd_set* writefds, fd_set* errfds,
		 struct timeval *timeout);

  // A truly re-entrant (or at least thread-safe) version of
  // gethostbyname_r
  struct hostent getHostByNameThreadSafe_ (const string& hostname, 
					   Array<char>& host_buff);  

  
  // Help someone open a socket on a named host
  enum HelpOpenSocket_e { HELP_CONNECT, HELP_BIND, HELP_NOTHING };
  void helpOpenSocket_ (int domain, int type, int protocol,
			HelpOpenSocket_e connect_or_bind,
			const string& name, int port,
			int* fd,struct sockaddr_in* addr,
			Val timeout=None);

  // Set socket to blocking or non-blocking: None means put it
  // in blocking mode, otherwise it goes into non-blocking mode
  void setblocking_ (int fd, Val timeout);

  // Helper function to allow connect to timeout quicker (the
  // standard timeout for connect seems to be minutes).  We do
  // this by setting the socket descriptor to NON-BLOCKING for the
  // duration of the connect call, do a select, then return back
  // to BLOCKING.  The current coding makes it so it defers the
  // timeout error to happen in socket init instead: this seems
  // more stable (and allows reconnects easier).  Returns 0 if
  // everything okay.
  int nonBlockingConnect_ (int* fdp, struct sockaddr_in* addr,
			   Val timeout_in_seconds);

  // Helper code used by M2k for binary serialization: put in .cc so
  // all .m2k issms isolated in .cc file.  

  // Straight from opalmsghdrnet code ... member so it gets readExact
  void readContinueStreamDataEncoding_ (int fd,
					StreamDataEncoding& sde,
					char* rep);
  // This code handles reading the complex M2k preambles.
  int handleReadingM2kHdr_ (int fd, char* rep,
			    MachineRep_e& endian);
  // This code handles writing the complex M2k preamble
  void handleWritingM2kHdr_ (int fd); 

}; // MidasSocket_


PTOOLS_END_NAMESPACE


#define MIDASSOCKET_H_
#endif // MIDASSOCKET_H_
