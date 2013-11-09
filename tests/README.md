Test suite for Pocketsphinx.js
==============================

We are adding tests for the different components. The tests are built on [QUnit](http://qunitjs.com). To run the suite, just open `test_suite.html` in a web browser. `qunit` is included, there is no need to install it. Meanwhile, it assumes `pocketsphinx.js` is in a `../build` folder. It only tests `pocketsphinx.js` for now.

You can for instance start a small web server with `python -m SimpleHTTPServer` in the base directory and open `http://localhost:8000/tests/test_suite.html` in your browser.