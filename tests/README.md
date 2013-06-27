Test suite for Pocketsphinx.js
==============================

We are adding tests for the different components. The tests are built on [QUnit](http://qunitjs.com) and [Karma](http://karma-runner.github.io/0.8/index.html). You can, for instance launch all tests with:

    $ for i in karma_pocketsphinx.js_*; do karma start $i --single-run --browsers Chrome --reporters progress,junit; done

# 1. Design of test suite

Since `pocketsphinx.js` makes use of global variables and keeps track of a state, we can't implement a standard test suite which loads the JavaScript file to be tested once and runs different tests. Ultimately, we might want to change the internal structure of `pocketsphinx.js` and allow multiple instances of the recognizer, but for now we get around the issue by making different test suites running in different browser instances, using multiple Karma config files.

# 2. Files

The test suite currently includes

    tests/README.md # this file
    tests/pocketsphinx.js_test_*.js # Karma config file, needed to be run independently
    tests/js/pocketsphinx.js_test_*.js # QUnit tests
    tests/js/utils/wrapModule.js # Utility to access pocketsphin.js functions

Tests must be run with each Karma config file independently, for instance:

    $ cd tests
    $ karma start karma_pocketsphinx.js_initialized_recognizer.conf.js --single-run

The test suite tests the provided prebuilt version of `pocketsphinx.js`, in `webapp/js`. Edit the Karma config files if you wish to change that.