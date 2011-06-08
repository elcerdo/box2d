
#include "valpython.h"

PTOOLS_BEGIN_NAMESPACE

// For unpickling

// Turn a python type code into a contiguous memory piece and convert
// to appropriate byte order
Val MakeVector (const string& typecode, size_t elements,
		const char* mem, size_t bytes)
{
  // Error checking
  if (typecode.length()>1 || typecode.length()==0) {
    MakeException("Only expect 1 letter types for Depickling Numeric "
          "arrays.  Saw:"+typecode);
  }
  
  // Lookup the val typecode from the python typecode
  char python_typecode[]   = "dlFD";
  char* where = strchr(python_typecode, typecode[0]);
  if (!where) {
    MakeException("Unknown depickling typecode"+typecode);
  }
  char val_typecode[] = { 'd', 'l', 'F', 'D' };
  char val_type = val_typecode[where-python_typecode];

  // Create a val vector of the proper type and length: and populate.
  Val final_result;
  char* result_buff = 0;
  int result_element_bytes = 0, result_bytes = 0;
  bool is_cx = false;
  if (val_type == 'd') {
    final_result = Array<real_8>(elements);
    Array<real_8>& a = final_result;
    a.expandTo(elements);
    result_buff = (char*)a.data();
    result_element_bytes = sizeof(real_8);
    result_bytes = elements*result_element_bytes;
  } else if (val_type == 'l') {
    final_result = Array<int_4>(elements);
    Array<int_4>& a = final_result;
    a.expandTo(elements);
    result_element_bytes = sizeof(int_4);
    result_buff = (char*)a.data();
    result_bytes = elements*result_element_bytes;
  } else if (val_type == 'D') {
    is_cx = true;
    final_result = Array<complex_16>(elements);
    Array<complex_16>& a = final_result;
    a.expandTo(elements);
    result_element_bytes = sizeof(complex_16);
    result_buff = (char*)a.data();
    result_bytes = elements*result_element_bytes;
  } else if (val_type == 'F') {
    is_cx = true;
    final_result = Array<complex_8>(elements);
    Array<complex_8>& a = final_result;
    a.expandTo(elements);
    result_element_bytes = sizeof(complex_8);
    result_buff = (char*)a.data();
    result_bytes = elements*result_element_bytes;
  } else {
    char s[2]; s[0] = val_type; s[1] = 0;
    MakeException("Don't support Arrays of type:"+string(s));
  }
  CopyPrintableBufferToVector(mem, bytes, result_buff, result_bytes);

  // If this machine is big-endian, we need to normalized to little endian
  if (!IsLittleEndian()) {
    InPlaceReEndianize(result_buff, elements, result_element_bytes, is_cx);
  }
  return final_result;
}


// For pickling

char ChooseNumber (const Val& v, string& arg1, string& arg2) 
{
  char choose = 0;
  switch (v.tag) {
  case 's': // int_1
  case 'S': // int_u1
  case 'i': // int_2
  case 'I': // int_u2
  case 'l': // int_4
    {
      choose = 'i'; int_4 i4 = v; Val vi4 = i4;
      Str s = vi4;
      arg1 = string(s.c_str());
    }
    break;
  case 'x' : // int_8
    {
      choose = 'l'; int_8 i8 = v; Val vi8 = i8; 
      Str s = vi8;
      arg1 = string(s.c_str());
    }
    break;
  case 'L': // int_u4
  case 'X': // int_u8
    {
      choose = 'l'; int_u8 iu8 = v; Val viu8 = iu8;
      Str s = viu8;
      arg1 = string(s.c_str());
    }
    break;
  case 'f' : // real_4
  case 'd' : // real_8
    {
      choose = 'd'; real_8 d8 = v; Val vd8 = d8;
      Str s = vd8;
      arg1 = string(s.c_str());
      break;
    }
  case 'F': // complex_8
  case 'D': // complex_16
    {
      choose = 'D'; complex_16 c16 = v;
      Val vr =c16.re; Val vi = c16.im;
      Str sr = vr;    Str si = vi;
      arg1 = string(sr.c_str()); arg2 = string(si.c_str());
      break;
    }
  case 'q': // int_n
  case 'Q': // int_un
    {
      choose = 'q'; int_n in = v;
      arg1 = Stringize(in);
      break;
    }
  default: 
    string s; s=v.tag;
    MakeException("ChooseNumber:don't know how to serialize numeric type:"+s);
    break;
  }
  return choose;
}

string BuildVector (const Val& v, const string& typecode)
{  
  // Lookup the val typecode from the python typecode
  char python_typecode[]   = "dlFD";
  char* where = strchr(python_typecode, typecode[0]);
  if (!where) {
    MakeException("Unknown depickling typecode"+typecode);
  }
  char val_typecode[] = { 'd', 'l', 'F', 'D' };
  char val_type = val_typecode[where-python_typecode];

  // Build the Vector of the type that maps to some Python entity
  int elements=0;
  int byte_len = 0; 
  char* vbuff = 0;
  bool is_cx = false;
  if (val_type=='d') {
    Array<real_8>& a = v;
    elements = a.length();
    byte_len = sizeof(real_8);
    vbuff = (char*)a.data();
  } else if (val_type=='l') {
    Array<int_4>& a = v;
    elements = a.length();
    byte_len = sizeof(int_4);
    vbuff = (char*)a.data();
  } else if (val_type=='D') {
    is_cx = true;
    Array<complex_16>& a = v;
    elements = a.length();
    byte_len = sizeof(complex_16);
    vbuff = (char*)a.data();
  } else if (val_type=='F') {
    is_cx = true;
    Array<complex_8>& a = v;
    elements = a.length();
    byte_len = sizeof(complex_8);
    vbuff = (char*)a.data();
  } else {
    string arr_type; arr_type = v.subtype;
    MakeException("Don't support Arrays of type:"+arr_type);
  }
  
  if (!IsLittleEndian()) { // machine is big endian, convert!
    InPlaceReEndianize(vbuff, elements, byte_len, is_cx);
  }
  // Why *2?  Because about 3/5 take exactly 1 character, about half
  // *take 2/5 take 4 characters
  Array<char> a(elements*2); 
  PrintBufferToString(vbuff, elements*byte_len, a);
  return string(a.data());
}

void UnknownType (PythonPicklerA<Val>& pickler, const Val& v)
{
  MakeWarning("Unknown type found during picking, turning into a string");
  Str s = v;
  pickler.dumpString(string(s.c_str(), s.length()));
}

void PickleIntoBuffer (const Val& v, Array<char>& array_to_dump_to, 
		       bool uses_numeric)
{
  {
    PythonBufferPickler<Val> pp(array_to_dump_to, uses_numeric);
    pp.dump(v);
  }
}

Val DepickleFromBuffer (int buff_len, char* buff, bool uses_numeric)
{ 
  Val retval;
  {
    PythonBufferDepickler<Val> pbd(buff_len, buff, uses_numeric);
    retval = pbd.load();
  }
  return retval;
}


PTOOLS_END_NAMESPACE
