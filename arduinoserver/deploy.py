class Deployment():

    def __init__(self, manager, arguments: str):
        pass

    def deploy(self) -> None:
        """Blocking call that deploys the server. Should return after shutdown is called. Must be blocking"""
        self.run(self.token)

    def reload(self, module: str, raw: bool=False) -> bool:
        """Reloads and updates a module of the server"""
        pass

    async def shutdown(self) -> None:
        """Called when the server has been requested to be shut down"""
        pass


from http.server import BaseHTTPRequestHandler, HTTPServer

hostName = "localhost"#
serverPort = 8080

class MyServer(BaseHTTPRequestHandler):
    def do_GET(self):
        self.send_response(200)
        self.send_header("Content-type", "text/html")
        self.end_headers()
        self.wfile.write(bytes("<html><head><title>https://pythonbasics.org</title></head>", "utf-8"))
        #self.wfile.write(bytes("<p>Request: %s</p >" % self.path, "utf-8"))
        self.wfile.write(bytes("<body>", "utf-8"))
        self.wfile.write(bytes("<p>Hi</p >", "utf-8"))
        self.wfile.write(bytes("</body></html>", "utf-8"))

if __name__ == "__main__":
    webServer = HTTPServer((hostName, serverPort), MyServer)
    print("Server started http://%s:%s" % (hostName, serverPort))

    try:
        webServer.serve_forever()
    except KeyboardInterrupt:
        pass

    webServer.server_close()
    print("Server stopped.")

