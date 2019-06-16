#!/usr/bin/env python
#
# Small web server that serves
# pocketsphinx.wasm with the
# correct MIME type
#

import SimpleHTTPServer
import SocketServer

PORT = 8000

Handler = SimpleHTTPServer.SimpleHTTPRequestHandler

Handler.extensions_map['.wasm'] = 'application/wasm'
httpd = SocketServer.TCPServer(("", PORT), Handler)

print("serving at port {}".format(PORT))

httpd.serve_forever()
