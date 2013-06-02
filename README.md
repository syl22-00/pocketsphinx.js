PocketSphinx.js - Speech recognition in JavaScript
==================================================

PocketSphinx.js is an attempt to perform speech recognition entirely in the web browser. The idea is to:

* convert a speech recognizer written in C ([PocketSphinx](http://cmusphinx.sourceforge.net/)) into JavaScript using [Emscripten](https://github.com/kripken/emscripten),
* record audio using the Web audio API and send it to the recognizer.


### 1. Compiling and executing the test application

For now, we have a small test that initializes a PocketSphinx decoder. The build is generated with CMake and makes use of LLVM. It should work, for now, on Linux and Mac OSX:

    $ cd .../pocketsphinx.js # This folder
    $ mkdir build
    $ cd build
    $ cmake -DEMSCRIPTEN=1 -DCMAKE_TOOLCHAIN_FILE=path_to_emscripten/cmake/Platform/Emscripten.cmake -DCMAKE_MODULE_PATH=path_to_emscripten/cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=path_to_emscripten/emcc -DCMAKE_CXX_COMPILER=path_to_emscripten/em++  -DCMAKE_AR=path_to_emscripten/emar -DCMAKE_RANLIB=path_to_emscripten/emranlib ..
    $ make

This should generate `test.html` that you can open in your web browser:

    $ firefox test.html &

You should see a simple HTML page which loads the generated JavaScript. After a few seconds, you should see the log output by PocketSphinx, and "Done" in the end which means that the decoder was initialized successfully.

On Google Chrome, launch it with `--disable-web-security`.

### 3. Test library

A test library is also generated, `test_lib.html`. Open it in your browser, start a JavaScript console and interact with it with:

    $ x=Module.cwrap('initialize')
    $ y=x()

It should return 0 if successful.

### 2. Acoustic model

The `am` folder contains an acoustic model trained with [SphinxTrain](http://cmusphinx.sourceforge.net/wiki/tutorialam). It is built using the [RM1](http://www.speech.cs.cmu.edu/databases/rm1/index.html) corpus, semi-continuous, with 200 senones.

### 3. PocketSphinx

pocketsphinx.js ships with PocketSphinx and Sphinxbase version 0.8 without modification except that the `model` folder of PocketSphinx (which contains large acoustic and language models) was not included.