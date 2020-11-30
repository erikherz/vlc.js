#!/usr/bin/env python3

import sys
import socketserver
import ssl
import argparse
import os
import posixpath
import urllib
from http.server import SimpleHTTPRequestHandler, HTTPServer

class Server(HTTPServer):
    def __init__(self, base_path, *args, **kwargs):
        HTTPServer.__init__(self, *args, **kwargs)
        self.RequestHandlerClass.base_path = base_path

class Handler(SimpleHTTPRequestHandler):
    def translate_path(self, path):
        path = posixpath.normpath(urllib.parse.unquote(path))
        words = path.split('/')
        words = filter(None, words)
        path = self.base_path
        for word in words:
            drive, word = os.path.splitdrive(word)
            head, word = os.path.split(word)
            if word in (os.curdir, os.pardir):
                continue
            path = os.path.join(path, word)
        return path

    def end_headers(self):
        #self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Cross-Origin-Opener-Policy', 'same-origin')
        self.send_header('Cross-Origin-Embedder-Policy', 'require-corp')
        self.send_header('Cross-Origin-Resource-Policy', 'cross-origin')
        SimpleHTTPRequestHandler.end_headers(self)

if __name__ == '__main__':
    if sys.version_info < (3, 7, 5):
        Handler.extensions_map['.wasm'] = 'application/wasm'

    parser = argparse.ArgumentParser()
    parser.add_argument('--port', '-p', default=8000, type=int)
    parser.add_argument('--dir', '-d', default=os.getcwd(), type=str)
    parser.add_argument('--certfile', '-c', default="/home/"+ os.getenv("USER") + "/server.pem", type=str)
    
    args = parser.parse_args()
    server_address = ('', args.port)
    certpath = args.certfile
    
    # with socketserver.TCPServer(("", PORT), Handler) as httpd:
    httpd = Server(args.dir, server_address, Handler)
    httpd.socket = ssl.wrap_socket(httpd.socket,
                                   server_side=True,
                                       certfile= certpath)
    print("Serving directory {} port {}.".format(args.dir, args.port))
    httpd.serve_forever()
