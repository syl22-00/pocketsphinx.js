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
