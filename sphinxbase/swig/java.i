// camelCase method names
%rename("%(lowercamelcase)s", notregexmatch$name="^[A-Z]") "";
%include <arrays_java.i>

// Special typemap for arrays of audio.
%include "arrays_java.i"
%apply short[] {const int16 *SDATA};
