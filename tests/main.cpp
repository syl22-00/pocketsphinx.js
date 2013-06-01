/************************************************************
 *
 * Simple command line program which initializes a PocketSphinx
 * decoder.
 *
 ***********************************************************/

#include <iostream>
#include "pocketsphinx.h"

int main() {
  std::cout << "Starting...\n";
  const arg_t cont_args_def[] = {
    POCKETSPHINX_OPTIONS,
    { "-argfile",
      ARG_STRING,
      NULL,
      "Argument file giving extra arguments." },
    { "-adcdev",
      ARG_STRING,
      NULL,
      "Name of audio device to use for input." },
    { "-infile",
      ARG_STRING,
      NULL,
      "Audio file to transcribe." },
    { "-time",
      ARG_BOOLEAN,
      "no",
      "Print word times in file transcription." },
    CMDLN_EMPTY_OPTION
  };
  // We will initialize the decoder with the provided acoustic model
  char * argv[] = {(char *)"-hmm", (char *)"am/rm1_200", (char *)"-bestpath", (char *)"no"};
  int argc = 4;
  // First, create a config with the provided arguments
  cmd_ln_t * config = cmd_ln_parse_r(NULL, cont_args_def, argc, argv, FALSE);
  if (config == NULL) {
    std::cout << "Config is NULL\n";
    return 1;
  }
  // Now it is time to initialize the decoder
  ps_decoder_t * recognizer = ps_init(config);
  if (recognizer == NULL) {
    std::cout << "recognizer is NULL\n";
    return 1;
  }
  // If we're there, it means we're successful
  std::cout << "Done\n";
  return 0;
}
