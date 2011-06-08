#ifndef OCSTRINGTOOLS_H_

#include "ocport.h"
#include "ocarray.h"

OC_BEGIN_NAMESPACE 

// Helper function for Depickling
inline void CopyPrintableBufferToArray (const char* pb, size_t pb_bytes,
					Array<char>& a)
{
  char hex[] = "0123456789abcdef";
  char singlebyte[] = "\n\\\r\t\'\"";
  char       code[] = "n\\rt'\"";
  
  int int_bytes = int(pb_bytes);
  for (int ii=0; ii<int_bytes;) {
    if (pb[ii]=='\\' && ii+1<int_bytes) { // non-printable, so was escaped
      char* where = strchr(code, pb[ii+1]);
      if (where) {
	a.append(singlebyte[where-code]); 
	ii+=2;
	continue;
      } else if (ii+3<int_bytes && pb[ii+1]=='x') { 
	a.append(  
	  (strchr(hex, pb[ii+2])-hex)*16 + 
	  (strchr(hex, pb[ii+3])-hex)
	  );
	ii+=4;
	continue;
      } else {
	throw string("Malformed Numeric vector string:"+
		     string(pb, pb_bytes)+" ... Error happened at:");
		     // IntToString(ii));
      }
      
    } else { // printable, so take as is
      a.append(pb[ii++]);
    }
  }
}

// Helper Function for Pickling
inline void PrintBufferToArray (const char* vbuff, int bytes, Array<char>& a)
{
  char singlebyte[] = "\n\\\r\t\'\"";
  char       code[] = "n\\rt'\"";
  char        hex[] = "0123456789abcdef";
 
  for (int ii=0; ii<bytes; ii++) {
    unsigned char c = vbuff[ii];
    char* where = strchr(singlebyte, c);
    if (c!='\0' && where) {  // Awkward escape sequence?
      a.append('\\');
      a.append(code[where-singlebyte]);
    } else if (isprint(c)) { // Is printable? As as you are
      a.append(c); 
    } else { // Not strange, not printable, hex escape sequence it!
      a.append('\\'); a.append('x');
      int high_nibble = c >> 4; 
      char msn = hex[high_nibble]; 
      a.append(msn);
      
      int low_nibble = c & 0x0F; 
      char lsn = hex[low_nibble];
      a.append(lsn);
    }
  }
  // a.append('\0'); // Since this is a C-Ctyle printable string
}


// Return the "image" of this string, i.e., what it would look
// like if we had to enter it manually (with quotes and escaped
// sequences). 
inline string Image (const string& orig, bool with_quotes=false)
{
  Array<char> a(orig.length());
  if (with_quotes) a.append('\'');
  PrintBufferToArray(orig.data(), orig.length(), a);
  if (with_quotes) a.append('\'');
  return string(a.data(), a.length());
}

// When using STL string, sometimes revert to OCString for certain
// (like shared memory)
#if !defined(OC_USE_OC_STRING) 
inline string Image (const OCString& orig, bool with_quotes=false)
{
  Array<char> a(orig.length());
  if (with_quotes) a.append('\'');
  PrintBufferToArray(orig.data(), orig.length(), a);
  if (with_quotes) a.append('\'');
  return string(a.data(), a.length());
} 
#endif

// Deimage: Take a string that was input (with all of it's quotes and
// escape characters) and turn it into the string in memory with hex
// escapes turned into real things.
inline string DeImage (const string& orig, bool with_quotes=false)
{
  Array<char> a(orig.length());
  if (with_quotes) { // quotes are on front and back
    CopyPrintableBufferToArray(orig.data()+1, orig.length()-2, a);
  } else { // no quotes on front and back
    CopyPrintableBufferToArray(orig.data(), orig.length(), a);
  }
  return string(a.data(), a.length());
}

inline void indentOut_ (ostream& os, int indent)
{
  string s;
  for (int ii=0; ii<indent; ii++) s+=' ';
  os << s;
}

OC_END_NAMESPACE 

#define OCSTRINGTOOLS_H_
#endif
