/*****************************************
 *
 * This is the publci interface of PocketSphinx.js
 * The following functions can be accessed from 
 * JavaScript.
 *
 * It is a C wrapper to avoid name-mangling
 *
 *****************************************/

#include "pocketsphinxjsImpl.h"

extern "C" {
  int initialize() {
    return ps_initialize();
  }
}
