%include <exception.i>

%apply int {int32};

#if SWIGPYTHON

%include <file.i>

// Special typemap for arrays of audio.
%typemap(in) \ 
  (const void *SDATA, size_t NSAMP) = (const char *STRING, size_t LENGTH);

%typemap(check) size_t NSAMP {
  char buf[64];
  if ($1 % sizeof(int16)) {
    sprintf(buf, "block size must be a multiple of %zd", sizeof(int16));
    SWIG_exception(SWIG_ValueError, buf);
  }
}

// Special typemap for string array
%typemap(in) (size_t n, char **ptr) {
  /* Check if is a list */
  if (PyList_Check($input)) {
    int i;
    $1 = PyList_Size($input);
    $2 = (char **) malloc(($1)*sizeof(char *));
    for (i = 0; i < $1; i++) {
      PyObject *o = PyList_GetItem($input,i);
      if (PyString_Check(o))
        $2[i] = PyString_AsString(PyList_GetItem($input,i));
      else {
        PyErr_SetString(PyExc_TypeError,"must be a list of strings");
        free($2);
        return NULL;
      }
    }
  } else {
    PyErr_SetString(PyExc_TypeError,"list type expected");
    return NULL;
  }
}

%typemap(freearg) (size_t n, char **ptr) {
  free($2);
}
#elif SWIGJAVA

%include <arrays_java.i>

// Special typemap for arrays of audio.
%apply short[] {const int16 *SDATA};

// Typemap for arrays of strings used in ngram for example
%typemap(in) (size_t n, char **ptr) {
  int i = 0;
  $1 = (*jenv)->GetArrayLength(jenv, $input);
  $2 = (char **) malloc(($1)*sizeof(char *));
  /* make a copy of each string */
  for (i = 0; i<$1; i++) {
    jstring j_string = (jstring)(*jenv)->GetObjectArrayElement(jenv, $input, i);
    const char * c_string = (*jenv)->GetStringUTFChars(jenv, j_string, 0);
    $2[i] = malloc((strlen(c_string)+1)*sizeof(char));
    strcpy($2[i], c_string);
    (*jenv)->ReleaseStringUTFChars(jenv, j_string, c_string);
    (*jenv)->DeleteLocalRef(jenv, j_string);
  }
}

%typemap(freearg) (size_t n, char **ptr) {
  int i;
  for (i=0; i<$1; i++)
    free($2[i]);
  free($2);
}

%typemap(jni) (size_t n, char **ptr) "jobjectArray"
%typemap(jtype) (size_t n, char **ptr) "String[]"
%typemap(jstype) (size_t n, char **ptr) "String[]"
%typemap(javain) (size_t n, char **ptr) "$javainput"

#endif

// Define typemaps to wrap error codes returned by some functions,
// into runtime exceptions.
%typemap(in, numinputs=0, noblock=1) int *errcode {
  int errcode;
  $1 = &errcode;
}

%typemap(argout) int *errcode {
  if (*$1 < 0) {
    char buf[64];
    sprintf(buf, "$symname returned %d", *$1);
    SWIG_exception(SWIG_RuntimeError, buf);
  }
}

