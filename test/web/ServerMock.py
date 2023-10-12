from http.server import HTTPServer, BaseHTTPRequestHandler
from io import BytesIO
import re
import os
import json

import utils.ServerUtils as ServerUtils

scriptLocation = __file__
reader = ServerUtils.MockDataReader(scriptLocation, '../../data', 'Responses.json')

# configDir = os.path.join(rootDir, 'include')
print("Initial Notes for Test Routine")
print("  Place test-data in: " + reader.serverDir)
print("  Pages are placed in: " + reader.webSiteDir)

os.chdir(reader.webSiteDir)
nextApiResponse = '{"Test": "Response"}'


class MockServer(BaseHTTPRequestHandler):

    def _set_response(self):
        self.send_response(200)
        self.send_header('Content-type', 'text/html')
        self.end_headers()
        
    def _html(self, message, details = ""):
        content = f"<html><body><h1>{message}</h1><p>{details}</p></body></html>"
        return content.encode("utf8")  # NOTE: must return a bytes object!

    def do_GET(self):
        print(f"        Evaluation request path '{self.path}'")
        found = reader.ScanResponseDefinitionsForRouteTakeFirst(self.path)
        if (found != None):
            self.prepareJsonResult(found)
            return

        if self.path == '/':
            self.path = '/mainPage.html' # redirect to Hompage
        if self.path == '/favicon.ico':
            self.path = '/favicon.png' # redirect favicon
            with open("favicon.png", 'rb') as iconFile:
                self.send_header('Content-type', 'image/png')
                self.end_headers()
                self.wfile.write(iconFile.read())
                return

        responseType = 'text/html'
        ending = ""

        if match := re.search('\\w+[.](\\w+)', self.path):
            ending = match.group(1)
        
        print(f"        Ending recognized '{ending}'")
        if ending == "":
            responseType = 'text/html'
            self.path = self.path + ".html"
        if ending == 'txt':
            responseType = 'text/txt'
        elif ending == 'js':
            responseType = 'application/javascript'
        elif ending == 'css':
            responseType = 'text/css'
        elif ending == 'png':
            responseType = 'image/png'
        elif ending == 'ico':
            responseType = 'image/*'
            self.send_response(200)
            self.send_header('Content-type', responseType)
            self.end_headers()
            self.wfile.write(load_binary(self.path[1:]))
            return

        try:
            file_to_open = open(self.path[1:]).read()
            self.send_response(200)
        except:
            file_to_open = "File not found"
            self.send_response(404)
            return
        
        self.send_header('Content-type', responseType)
        self.end_headers()
        self.wfile.write(bytes(file_to_open, 'utf-8'))


    def do_POST(self):
        content_length = int(self.headers['Content-Length'])
        post_data = self.rfile.read(content_length)
        data = post_data.decode("utf-8")

        if self.path == '/Debug/SetNextJsonResponse':
            nextApiResponse = data
            print(f"       => Next Payload on API path '{nextApiResponse}'")
            self._set_response()
            self.send_response(200)
            return
        
        self._set_response()
        # self.wfile.write(self._html("POST "+self.path, data.encode('utf-8')))
        self.wfile.write("{}".format(data).encode('utf-8'))
        print(f"        Payload {data}")

    def prepareJsonResult(self, object):
            print(f"        => Responding: '{json.dumps(object)}'")
            self.send_response(200)
            wStat = json.dumps(object)
            self.send_header('Content-type', 'text/json')
            self.end_headers()
            self.wfile.write("{}".format(wStat).encode('utf-8'))

        
port = 80
httpd = HTTPServer(('localhost', port), MockServer)
print("serving at port", port)
try:
    httpd.serve_forever()
except KeyboardInterrupt:
    pass

httpd.server_close()
