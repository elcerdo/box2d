#ifndef VALREADER_H_

// NOTE: Due to weird include dependencies, this file includes nothing
// itself.  If you wish to use it, make sure you include ocval.h
// first.

// A class to help read "human-readable" strings of Vals (Tabs, Arrs,
// numbers, Numeric arrays).  Format is straight-forward:
//
//   ValReader v(" { 'a':'nice', 'table':1 }");
//   Tab t;
//   v.expectTab(t);
//
// From Files (streams):
//   StreamValReader sv(istream& is);
//   Tab t;
//   sv.expectTab(t);
//
// The constructors of Tab and Arr have been augmented so as to allow
// easier construction:
//   
//   Tab t = "{ 'a':1, 'b':2, 'c':3, 'list':[1,2,3] }";
//   Arr a = "[ 'a', 2, 3.3, (1+2j), { 'empty':'ish' } ]";

#include "ocstringtools.h"
#include "ocreader.h"
#include "ocnumerictools.h"
#include <limits> // for Nan and Inf and -inf

OC_BEGIN_NAMESPACE

// Abstract base class: All the code for parsing the letters one by
// one is here.  The code for actually getting the letters (from a
// string, stream, etc.) defers to the derived class.

class ValReaderA { 

  // Indicate which special value we have
  enum ValReaderEnum_e { VR_NOT_SPECIAL, VR_NAN, VR_INF, VR_NEGINF }; 

 public: 

  ValReaderA (ReaderA* adopted_reader) :
    reader_(adopted_reader) { }

  virtual ~ValReaderA () { delete reader_; }


  // Expect any number (including complex)
  void expectNumber (Val& n)
  {
    consumeWS_();
    int c=peekChar_();
    if (c=='(') {
      expectComplex_(n);
      return;
    }
    expectSimpleNumber(n);
  }

  // Expect any "non-complex" number, i.e., any positive or negative
  // int float or real: just not any complex numbers with ()!

  void expectSimpleNumber (Val& n)
  {
    consumeWS_();

    // Get the integer part, if any, of the number
    ValReaderEnum_e special = VR_NOT_SPECIAL;
    Str integer_part = getSignedDigits_('.', &special);
    switch (special) { // will print as nan, -inf and inf
    case VR_INF: { 
      real_8 inf = numeric_limits<double>::infinity();
      n = inf;
      return;
    } 
    case VR_NEGINF: { 
      real_8 ninf = -numeric_limits<double>::infinity();
      n = ninf;
      return;
    } 
    case VR_NAN: { 
      real_8 nan = numeric_limits<double>::quiet_NaN();
      n = nan;
      return;
    }
    case VR_NOT_SPECIAL: default: break; // really, just fall through
    }

    // Get the fractional part, if any
    Str fractional_part;
    int c = peekChar_();
    if (c=='.') {     
      c = getChar_(); // consume the '.'
      fractional_part = "."+getDigits_();
      if (fractional_part.length()==1) { 
	const int i_len = integer_part.length();
	if (i_len==0 || (i_len>0 && !isdigit(integer_part[i_len-1]))) {
	  syntaxError_("Expecting some digits after a decimal point, but saw '"+saw_(peekChar_())+"'");
	}
      }
      c = peekChar_();
    }

    // Get the exponent part, if any
    Str exponent_part;
    if (c=='e' || c=='E') {
      c = getChar_();  // consume the 'e'
      exponent_part = "e"+getSignedDigits_(' ');
      if (exponent_part.length()==1) { // only an e
	char t[2]; t[0] = peekChar_(); t[1]='\0';
	syntaxError_("Expected '+', '-' or digits after an exponent, but saw '"+saw_(peekChar_())+"'");
      }
      c = peekChar_();
    }

    // At this point, we are (mostly) finished with the number, and we
    // have to build the proper type of number.
    if (fractional_part.length()>0 || exponent_part.length()>0) {
      // If we have either a fractional part or an exponential part,
      // then we have a floating point number
      Str stringized_number = integer_part+fractional_part+exponent_part;
      Val inside = stringized_number; 
      real_8 num = inside; // Convert out of Val to change from string 
      n = num;
      return;
    }
    
    // Well, no fractional part or exponential.  There had better be
    // some digits!
    if (integer_part.length()==0 || 
	(integer_part.length()>0 && 
	 !isdigit(integer_part[integer_part.length()-1])))
      syntaxError_("Expected some digits for a number");
	
    c=peekChar_();
    Val v = integer_part;
    if (c=='l' || c=='L') { // Okay, it's a long
      getChar_();  // consume long
      if (integer_part[0]=='-') {
	int_n long_int = v;
	n = long_int;
	return;
      } else {
	int_un long_int = v;
	n = long_int;
	return;
      } 
    } else { // plain integer
      convertInt_(v); // assumes some int, with tstring inside v
      n = v;
      return;
    }
  }

  // Read a string from the current place in the input
  void expectStr (Str& s)
  { 
    consumeWS_();

    char quote_mark = peekNWSChar_();
    if (quote_mark!='\'' && quote_mark!='"') {
      syntaxError_("A string needs to start with ' or \"");
    }

    expect_(quote_mark); // Start quote

    // Read string, keeping all escapes, and let DeImage handle escapes
    Array<char> a(80);
    for (int c=getChar_(); c!=quote_mark; c=getChar_()) {
      if (c==EOF) syntaxError_("Unexpected EOF inside of string");
      a.append(c);
      if (c=='\\') { // escape sequence
	int next = getChar_(); // Avoid '
	if (next==EOF) syntaxError_("Unexpected EOF inside of string");
	a.append(next);
      } 
    }    
    string temp = string(a.data(), a.length());
    string ss = DeImage(temp, false); // Do escapes 
    s = Str(ss.data(), ss.length());
  }


  // Expect Table on the input
  void expectTab (Tab& table) { expectSomeTable_(table, "{"); }

  // Expect an Arr
  void expectArr (Arr& a) { expectSomeCommaList_(a, '[', ']'); }


  // Read in a Numeric Array
  void expectNumericArray (Val& a)
  {
    consumeWS_();
    expect_("array([");
    a = Array<int_4>(); // By default, int array

    // Special case, empty list
    char peek = peekNWSChar_();
    if (peek!=']') {
      
      Val n;
      expectNumber(n); 
	
      if (n.tag=='l') readNumericArray_<int_4>(a, n);
      else if (n.tag=='D') readNumericArray_<complex_16>(a, n);
      else if (n.tag=='d') readNumericArray_<real_8>(a, n);
      else syntaxError_("Only support Numeric arrays of cx16, real_8, and int_4");
    }
    expect_(']');
    peek=peekNWSChar_();
    if (peek==')') {
      expect_(')');
      return;
    }
    // Otherwise, expecting a type tag
    expect_(',');
    Str typetag;
    expectStr(typetag);
    expect_(')');

    // Now convert if it makes sense.
    if (typetag.length()!=1) syntaxError_("Expected single char type tag");
    // Convert the array to the given type
    char numeric_tag = typetag[0];
    char oc_tag = NumericTagToOC(numeric_tag);
    ConvertArray(a, oc_tag);
    
  }


  // Expect one of two syntaxes:
  // o{ 'a':1, 'b':2 }
  //    or
  // OrderedDict([('a',1),('b',2)])
  void expectOTab (OTab& o) 
  {
    char c = peekNWSChar_();
    // Short syntax
    if (c=='o') { // o{ } syntax
      expectSomeTable_(o, "o{");
      return;
    } 
    // Long syntax
    expect_("OrderedDict(");
    Arr kvpairs;
    // We let the Arr and Tup parse themselves correctly, but since we
    // are using swapKeyValueInto, this just plops them into the table
    // pretty quickly, without excessive extra deep copying.
    expectArr(kvpairs);
    const int len = int(kvpairs.length());
    for (int ii=0; ii<len; ii++) {
      Tup& t = kvpairs[ii];
      o.swapInto(t[0], t[1]);
    } 
    expect_(')');
  }

  // Know a Tup is coming: starts with '(' and ends with ')'
  void expectTup (Tup& u) { expectSomeCommaList_(u.impl(), '(', ')'); } 

  // In general, a '(' starts either a complex number such as (0+1j)
  // or a tuple such as (1,2.2,'three'): This parses and places the
  // appropriate thing in the return value.
  void expectTupOrComplex (Val& v)
  {
    expect_('(');
    
    // Handle empty tuple
    int peek = peekNWSChar_();
    if (peek == EOF) syntaxError_("Saw EOF after seeing '('");
    if (peek==')') {
      v = Tup();
      expect_(')');
      return;
    }

    // Anything other than empty, has a first component
    Val first;
    expectAnything(first);
    
    // Well, plain jane number: if we see a + or -, it must be a complex
    peek = peekNWSChar_();
    if (OC_IS_NUMERIC(first) && !OC_IS_CX(first) && 
	(peek=='+' || peek=='-')) {
      expect_(peek);
      Val second;
      expectSimpleNumber(second);
      expect_("j)");
      if (first.tag=='f' || second.tag=='f') {
	v = complex_8(first, second);
      } else {
	v = complex_16(first, second);
      }
      return;
    }

    // If we don't see a plain number, it has to be a Tuple
    Tup& u = v = Tup(None); // just put something is first pos
    first.swap(u[0]);
    Array<Val>& a = u.impl();
    continueParsingCommaList_(a, '(', ')');
  }

  

  // We could be starting ANYTHING ... we have to dispatch to the
  // proper thing
  void expectAnything (Val& v)
  {
    char c = peekNWSChar_();
    switch (c) {
    case '{' : { v = Tab(); Tab& table = v; expectTab(table);       break; }
    case '[' : { v = Arr(); Arr& arr = v;   expectArr(arr);         break; }
    case '(' : {                            expectTupOrComplex(v);  break; }
    case '\'':
    case '"' : {            Str s = "";     expectStr(s); v=s;      break; }
    case 'a' : {                            expectNumericArray(v);  break; }
    case 'N' : { v = Val(); expect_("None");                        break; }
    case 'T' : { v = true;  expect_("True");                        break; }
    case 'F' : { v = false; expect_("False");                       break; }
    case 'o' : // o{ and OrderedDict start OTab
    case 'O' : { v = OTab(); OTab& ot = v;  expectOTab(ot);         break; } 
    default:   { v = Val();                 expectNumber(v);        break; }
    }
    
  }


 protected:


  // Helper Methods
  void syntaxError_ (const string& s) 
  {
    reader_->syntaxError(s);
  }

  string saw_ (int cc)
  {
    if (cc==EOF) return "EOF";
    char s[2] = { 0 }; 
    s[0] = cc;
    return string(s);
  }

  void expect_ (char expected)
  {
    // Single character token
    int get      = getNWSChar_();
    if (get!=expected) {
      string get_string, expected_string;
      if (get==EOF) get_string="EOF"; else get_string=get;
      expected_string = expected;
      syntaxError_("Expected:'"+expected_string+"', but saw '"+get_string+"' "
		   "on input");
    }
  }

  void expect_ (const string& s)
  {
    int s_len = s.length();
    for (int ii=0; ii<s_len; ii++) 
      expect_(s[ii]);
  }

  // Expect a complex number:  assumes it will have form (#+#j)
  void expectComplex_ (Val& n)
  {
    complex_16 result;
    expect_('(');
    Val real_part, imag_part;
    expectNumber(real_part);
    char c=peekChar_();
    if (c=='+' || c=='-') {
      expectNumber(imag_part);
    }
    expect_("j)");

    result.re = real_part;
    result.im = imag_part;
    n = result;
  }

  // From current point of input, consume all digits until
  // next non-digit.  Leaves non-digit on input
  Str getDigits_ ()
  {
    Str digits;
    while (1) {
      int c = peekChar_();
      if (c==EOF) 
	break;
      if (isdigit(c)) {
	digits+=char(c);
	getChar_();
      }
      else 
	break;
    }
    return digits;
  }

  // From current point in input, get the signed sequence of 
  // digits
  Str getSignedDigits_ (char next_marker, ValReaderEnum_e* inf_nan_okay=0)
  {
    // Get the sign of the number, if any
    char c=peekChar_();
    Str sign;

    // Add ability to recognize nan, inf, and -inf
    if (inf_nan_okay) {
      if (c=='n') { // expecting nan
	expect_("nan");
	*inf_nan_okay = VR_NAN;
	return "nan";
      } else if (c=='i') {
	expect_("inf");
	*inf_nan_okay = VR_INF;
	return "inf";
      }  
    }

    char saw_sign = ' ';
    if (c=='+'||c=='-') {
      saw_sign = c;
      sign = c;
      getChar_();    // consume the sign
      c=peekChar_(); // .. and see what's next
    }

    if (inf_nan_okay) {    
      if (saw_sign!=' ' && c=='i') { // must be -inf
	expect_("inf");
	if (saw_sign=='-') {
	  *inf_nan_okay = VR_NEGINF;
	  return "-inf";
	} else { // if (saw_sign=='+') {
	  *inf_nan_okay = VR_INF;
	  return "+inf";
	}
      }
    }
    
    // Assertion: passed sign, now should see number or .
    if (!isdigit(c)&&c!=next_marker) {
      syntaxError_("Expected numeric digit or '"+saw_(next_marker)+"' for number, but saw '"+saw_(c)+"'");
    }
    
    Str digits = sign+getDigits_();
    return digits;
  }
  
  template <class T>
  void readNumericArray_ (Val& arr, const Val& first_value)
  { 
    arr = Array<T>();  // initialize the array
    Array<T>& a = arr; // .. then a reference to it
    T v = first_value;
    a.append(v);
    
    // Continue getting each item one at a time
    while (1) {
      char peek = peekNWSChar_();
      if (peek==',') {
	expect_(',');
	continue;
      } else if (peek==']') {
	break;
      } 

      Val n; 
      expectNumber(n);
      T v = n;
      a.append(v);
    }
  }

  // the string of +- and digits: convert to some int
  void convertInt_ (Val& v) 
  {
    // Strip into sign and digit part 
    char sign_part = '+';
    string digit_part = v;
    if (!isdigit(digit_part[0])) {
      sign_part = digit_part[0];
      digit_part = digit_part.substr(1);
    }
    const int len = digit_part.length();

    // Try to turn into int_4 in general, otherwise slowly move from
    // int_8 to real_8
    if (sign_part=='-') { // negative number
      static const string smallest_int_4 = "2147483648";
      static const string smallest_int_8 = "9223372036854775808";
      if (len<int(smallest_int_4.length()) ||
	  (len==int(smallest_int_4.length()) && digit_part<=smallest_int_4)) { 
	int_4 i4_val = v;
	v = i4_val;
      } else if (len<int(smallest_int_8.length()) ||
		(len==int(smallest_int_8.length()) && digit_part<=smallest_int_8)) {
	int_8 i8_val = v;
	v = i8_val;
      } else {
	int_n r8_val = v;
	v = r8_val;
      }

    } else { // positive number
      static const string biggest_int_4 = "2147483647";
      static const string biggest_int_8 = "9223372036854775807";
      static const string biggest_int_u8 = "18446744073709551615";
      if (len<int(biggest_int_4.length()) || 
	  (len==int(biggest_int_4.length()) && digit_part<=biggest_int_4)) {
	int_4 i4_val = v;
	v = i4_val;
      } else if (len<int(biggest_int_8.length()) ||
		 (len==int(biggest_int_8.length()) && digit_part<=biggest_int_8)) {
	int_8 i8_val = v;
	v = i8_val;
      } else if (len<int(biggest_int_u8.length()) ||
		(len==int(biggest_int_u8.length()) && digit_part<=biggest_int_u8)) {
	int_u8 iu8_val = v;
	v = iu8_val;
      } else {
	int_un r8_val = v;
	v = r8_val;
      }
    }
    
  }

  // Expect Table on the input
  template <class TABLE>
  void expectSomeTable_ (TABLE& table, const char* start_table)
  {
    expect_(start_table);
    // Special case, empty Table
    char peek = peekNWSChar_();
    if (peek!='}') {
      
      for (;;) { // Continue getting key value pairs
	{ 
	  Val key;   expectAnything(key);
	  expect_(':');
	  Val value; expectAnything(value); 
	  
	  table.swapInto(key, value); // Assumes we are done with key-value
	}
	
	int peek = peekNWSChar_();
	if (peek==',') {
	  expect_(',');         // Another k-v pair, grab comma and move on
	  peek = peekNWSChar_();
          if (peek=='}') break;  // CAN have extra , at the end of the table
	  continue;
	} else if (peek=='}') { // End of table
	  break;
	} else {
	  syntaxError_("Expecting a '}' or ',' for table, but saw '"+saw_(peek)+"'");
	}
      }
    }
    expect_('}');
  }

  template <class OLIST> 
  void continueParsingCommaList_ (OLIST& a, char start, char end)
  {
    // Continue getting each item one at a time
    while (1) {         
      int peek = peekNWSChar_();
      if (peek==',') {
	expect_(',');
	peek = peekNWSChar_();
	if (peek==end) break;  // CAN have extra , at the end of the table
	
	a.append(Val());
	expectAnything(a[a.length()-1]); 
	
	continue;
      } else if (peek==end) {
	break;
      } else {
	char c[4] = { start, end, '\0' };
	syntaxError_("Problems parsing around '"+saw_(peek)+" for "+string(c));
      }
    }
    expect_(end);
  }

  template <class OLIST> 
  void expectSomeCommaList_ (OLIST& a, char start, char end)
  {
    expect_(start);
    
    // Special case, empty list
    char peek = peekNWSChar_();
    if (peek==end) {
      expect_(end);
    } else {
      // Continue getting each item one at a time, after gotten first
      a.append(Val());
      expectAnything(a[a.length()-1]); 
      continueParsingCommaList_(a, start, end);
    }
  }
  

  // Dispatch for input
  int getNWSChar_ ()  { return reader_->getNWSChar_(); }
  int peekNWSChar_ () { return reader_->peekNWSChar_(); }
  int getChar_ ()     { return reader_->getChar_(); }
  int peekChar_ ()    { return reader_->peekChar_(); }
  int consumeWS_ ()   { return reader_->consumeWS_(); }

  // Defer IO to another class.  All sorts of discussion on why
  // didn't we inherit, etc.  Look at the Design Patterns book.
  ReaderA* reader_; 

}; // ValReaderA


// The ValReader reads Vals from strings.  The most common usage is to
// use this to read literal constants for the constructors of Tab and
// Arr.
class ValReader : public ValReaderA {

 public:

  ValReader (Array<char>& a) :
    ValReaderA(new StringReader(a))
  { }

  ValReader (const char* s, int len=-1) :
    ValReaderA(new StringReader(s, len))
  { }


 protected:

}; // ValReader


// Read a val from a stream
class StreamValReader : public ValReaderA {

 public:

  // Open the given file, and attempt to read Vals out of it
  StreamValReader (istream& is) :
    ValReaderA(new StreamReader(is))
  { }

}; // StreamValReader


// Read the given Tab from a TEXT file: if there are any problems,
// throw a runtime_error indicating we had trouble reading the file,
// or a logic_error if the input is malformed.
inline void ReadTabFromFile (const string& filename, Tab& t)
{
  ifstream ifs(filename.c_str());
  if (ifs.good()) {
    StreamValReader sv(ifs);
    sv.expectTab(t);
  } else {
    throw runtime_error("Trouble reading file:"+filename);
  }
}


// Write the given Tab to a TEXT file: if there are problems, throw a
// runtime_error indicating we had trouble writing out the file.
inline void WriteTabToFile (const Tab& t, const string& filename)
{
  ofstream ofs(filename.c_str());
  if (ofs.good()) {
    t.prettyPrint(ofs);
  } else {
    throw runtime_error("Trouble writing the file:"+filename);
  }
}

// Read the given Val from a TEXT file: if there are any problems,
// throw a runtime_error indicating we had trouble reading the file,
// or a logic_error if the input is malformed.
inline void ReadValFromFile (const string& filename, Val& v)
{
  ifstream ifs(filename.c_str());
  if (ifs.good()) {
    StreamValReader sv(ifs);
    sv.expectAnything(v);
  } else {
    throw runtime_error("Trouble reading file:"+filename);
  }
}


// Write the given Val to a TEXT file: if there are problems, throw a
// runtime_error indicating we had trouble writing out the file.
inline void WriteValToFile (const Val& v, const string& filename)
{
  ofstream ofs(filename.c_str());
  if (ofs.good()) {
    v.prettyPrint(ofs);
  } else {
    throw runtime_error("Trouble writing the file:"+filename);
  }
}


// Evaluate the given string (Val literal) and return the Val underneath
inline Val Eval (const string& code)
{
  Val v;
  ValReader c(code.data(), code.length());
  c.expectAnything(v);
  return v;
}

inline Val Eval (const char* code, int len=-1)
{
  Val v;
  ValReader c(code, len);
  c.expectAnything(v);
  return v;
}


OC_END_NAMESPACE


#define VALREADER_H_
#endif // VALREADER_H_


