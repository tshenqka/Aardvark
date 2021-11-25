import multiprocessing
from flask import Flask
import gunicorn.app.base

app = Flask(__name__)

@app.route("/",methods=["GET","POST"])
def main():
    return "Welcome Aardvark!"

class FlaskApplication(gunicorn.app.base.BaseApplication):
    def __init__(self, app, options=None):
        self.options = options or {}
        self.application = app
        super().__init__()

    def load_config(self):
        config = {key: value for key, value in self.options.items() if key in self.cfg.settings and value is not None}
        for key, value in config.items():
            self.cfg.set(key.lower(), value)

    def load(self):
        return self.application

class Deployment():
    def __init__(self, manager, arguments: str):
        self.manager = manager
        self.arguments = arguments

    def deploy(self) -> None:
        """Blocking call that deploys the server. Should return after shutdown is called. Must be blocking"""
        self.manager.signalRunning()
        options = {
            'bind': '%s:%s' % ('0.0.0.0', self.arguments if self.arguments else "8080"),
            'workers': (multiprocessing.cpu_count() * 2) + 1,
        }
        FlaskApplication(app, options).run()
    
    def onDeploymentCommand(self, command):
        """Called when a command is recieved"""
        if command.command == "stop": 
            pass # Not implemented
