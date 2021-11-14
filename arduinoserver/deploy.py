
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