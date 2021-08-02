// function definition for args2str

int args2str(int argc, char* argv[], int opsize, char* op) {
  int written = 0;
  for (int i=0; i<argc && written < opsize; i++) {
    if (i>0) {
      for (int j=0; argv[i][j] != '\0'; j++)
      if (written<opsize-1) {
        op[written] = argv[i][j];
        written++;
      }
      op[written] = ' ';
      written++;
    }
  }
  if (written<opsize) op[written] = '\0';
  return written;
}

int cl_valid_char(char** input_string_ptr, char* output_string) {
  // <valid_char> ::= {'#'..'z'} - {'<','>',':','"','\','|','?','*','.','-'} 
  // if valid char, write it out to output_string
  char c = **input_string_ptr; 
  if ((c>='#')&&(c<='z')&&(c!='<')&&(c!='>')&&(c!=':')&&(c!='"')
    &&(c!='\\')&&(c!='|')&&(c!='?')&&(c!='*')
    &&(c!='.')&&(c!='-')) {
    *output_string = c;
    (*input_string_ptr)++;
    return !(0);
   } else return 0;
}

int cl_string(char** input_string_ptr, char* output_string) {
  // <string> ::= <valid_char>|<valid_char><string>
  // if valid string, write it out to output_string
  if (cl_valid_char(input_string_ptr, output_string)) {
    if (!cl_string(input_string_ptr, output_string+1)) {
      //end of string, add terminator to output_string
      output_string[1] = '\0';
      return !(0);
    } else return !(0); 
  } else return 0;
}

int cl_switch(char** input_string_ptr, char* output_string) {
  // <switch> ::= '-'<string>
  // test **input_string_ptr. If it is not '-', then return 0
  if (**input_string_ptr != '-') return 0;
  // otherwise increment (*input_string_ptr)
  else {
    (*input_string_ptr)++;
    // call cl_string(input_string_ptr, output_string)
    // if it returns non-zero, return non-zero
    if (cl_string(input_string_ptr, output_string)) return !(0);
    // otherwise decrement (*input_string_ptr) and return zero.
    else {
      (*input_string_ptr)++;
      return 0;
    }
  }
}

int cl_switches(char** input_string_ptr, char* output_string) {
  // <switches> ::= <switch>|<switch><switches>
  // call cl_switch(input_string_ptr, output_string)
  // if it returns with a value of zero, return 0
  if (!cl_switch(input_string_ptr, output_string)) {
    return 0;
    // terminate the output string by assigning '\0' to *(output_string+1)
    *output_string = '\0';
  } else {
    // else call switches(input_string_ptr, output_string+1)
    // (only the first character of each string is kept)
    int ignore = cl_switches(input_string_ptr, output_string+1);
    // return !(0)
    return !(0);
  }
}

int cl_filename(char** input_string_ptr, char* file_string, char* ext_string) {
  //<filename> ::=<string>|<string>'.'<string>
  // takes two output strings, one for the filename, one for the extension (if any)
  // if cl_string(input_string_ptr,file_string) returns zero then return zero
  if (!cl_string(input_string_ptr,file_string)) return 0;
  // else if **input_string_ptr != '.' assign '\0' to ext_string[0] andreturn non-zero
  else if (**input_string_ptr != '.') {
    ext_string[0]='\0';
    return !(0);
  }
  // else increment (*input_string_ptr)
  else {
    (*input_string_ptr)++;
    // if cl_string(input_string_ptr,ext_string) returns zero then
    if (!cl_string(input_string_ptr,ext_string)) {
    // assign '\0' to ext_string[0] andreturn non-zero
      ext_string[0]='\0';
      return !(0);
    }
    // else return non-zero;
    else return !(0);
  }
}

int cl_parameters(char** input_string_ptr, char* file_string, char* ext_string, char* switch_string){
  // <parameters> ::= <filename>|<filename>' '<switch>|<switch>' '<filename>.
  // if cl_filename(input_string_ptr,file_string, ext_string) then
  if (cl_filename(input_string_ptr,file_string, ext_string)) {
  // if **input_string_ptr == ' ', increment (*input_string_ptr) then
    if (**input_string_ptr = ' ') (*input_string_ptr)++;
  // call cl_switches(input_string_ptr, switch_string) and return non-zero
    int ignore = cl_switches(input_string_ptr,switch_string);
    return !(0);
  } 
    // else, in the case that the original call of cl_filename() returned zero
    // if cl_switches(input_string_ptr, switch_string) then
  else if (cl_switches(input_string_ptr, switch_string)) {
    // if **input_string_ptr == ' ', increment (*input_string_ptr) 
    if (**input_string_ptr == ' ') (*input_string_ptr)++;
    // and return cl_filename(input_string_ptr,file_string, ext_string)
    return cl_filename(input_string_ptr,file_string, ext_string);
    // otherwise return 0
  } else return 0;
}




