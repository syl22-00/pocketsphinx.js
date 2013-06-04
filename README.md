PocketSphinx.js
---------------
### Speech Recognition in JavaScript

PocketSphinx.js is an attempt to perform speech recognition entirely in the web browser. The idea is to:

* convert a speech recognizer written in C ([PocketSphinx](http://cmusphinx.sourceforge.net/)) into JavaScript using [Emscripten](https://github.com/kripken/emscripten),
* record audio using the Web audio API and send it to the recognizer.


### 1. Compiling and executing the test application

For now, we have a small test that initializes a PocketSphinx decoder. The build is generated with CMake and makes use of LLVM. It should work, for now, on Linux and Mac OSX:

    $ cd .../pocketsphinx.js # This folder
    $ mkdir build
    $ cd build
    $ cmake -DEMSCRIPTEN=1 -DCMAKE_TOOLCHAIN_FILE=path_to_emscripten/cmake/Platform/Emscripten_unix.cmake ..
    $ make

This should generate `test.html` that you can open in your web browser:

    $ firefox test.html &

You should see a simple HTML page which loads the generated JavaScript. After a few seconds, you should see the log output by PocketSphinx, and "Done" in the end which means that the decoder was initialized successfully.

On Google Chrome, launch it with `--disable-web-security`.

### 2. Test library

A test library is also generated, `test_lib.html`. Open it in your browser, start a JavaScript console and interact with it with:

    $ z=Module.cwrap('psGetState') 
    $ z()           # Should be 0
    $ x=Module.cwrap('psInitialize')
    $ y=x()         # Should be 0 if successful
    $ z()           # Should be 2

### 3. Acoustic model

The `am` folder contains an acoustic model trained with [SphinxTrain](http://cmusphinx.sourceforge.net/wiki/tutorialam). It is built using the [RM1](http://www.speech.cs.cmu.edu/databases/rm1/index.html) corpus, semi-continuous, with 200 senones.

### 4. PocketSphinx

pocketsphinx.js ships with PocketSphinx and Sphinxbase version 0.8 without modification except that the `model` folder of PocketSphinx (which contains large acoustic and language models) was not included.

### 5. License

PocketSphinx licensing terms are included in the `pocketsphinx` and `sphinxbase` folders. The remaining of this software is licensed under the MIT license:

Copyright © 2013 Sylvain Chevalier

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
