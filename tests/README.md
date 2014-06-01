Test suite for Pocketsphinx.js
==============================

The tests are built on [QUnit](http://qunitjs.com). To run the suite, just open `test_suite.html` in a web browser. There is also `test_suite_kws.html` for the keyword spotting functionality. `qunit` is included, there is no need to install it. Meanwhile, it assumes `pocketsphinx.js` is in a `../build` folder. It only tests `pocketsphinx.js` for now.

You can for instance start a small web server with `python -m SimpleHTTPServer` in the base directory and open `http://localhost:8000/tests/test_suite.html` in your browser.

In addition, there is a test suite for the `pocketsphinx_zh.js` file which is built with a Chinese acoustic model. Open `http://localhost:8000/tests/test_suite_zh.html` in your browser.
