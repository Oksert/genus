
import SimpleXMLRPCServer
import Server
import ThreadContext
import SocketServer

import logging
import logging.handlers
import CModule
import PropertiesConfiguration

f = file('/tmp/classifier.log','w')
del f

logger = logging.getLogger("")
logger.setLevel(logging.DEBUG)
handler = logging.handlers.RotatingFileHandler('/tmp/classifier.log','a',1000000,10)
formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s')
handler.setFormatter(formatter)
logger.addHandler(handler)

class PathFactory(ThreadContext.ContextPathFactory):
	def getContextPath(self,contextId):
		return "context" + str(int(contextId))
	def getDefaultContextPath(self):
		return "default"

def initializeCModule():
	cm = ThreadContext.ContextManager(PathFactory())
	CModule.setContextManager(cm)
	CModule.setLoggingFunctions(logging.getLogger)
	pass


#class XMLRPCServer(SocketServer.ThreadingMixIn,SimpleXMLRPCServer.SimpleXMLRPCServer): pass

try:
	initializeCModule()
	cm = ThreadContext.ContextManager.get()
	host = PropertiesConfiguration.getServerHost()
	port = PropertiesConfiguration.getServerPort()
	s = Server.XMLRPCServer((host,port))
	#s.prc_paths = (PropertiesConfiguration.getServerXmlrpc(),)
	#s.register_function(Server.startTrain,'Classifier.startTrain')
	#s.register_function(Server.retrain,'Classifier.retrain')
	#s.register_function(Server.deleteTopic,'Classifier.deleteTopic')
	#s.register_function(Server.predict,'Classifier.predict')
	#s.register_function(Server.delete,'Classifier.deleteContext')
	#s.register_function(Server.getContexts,'Classifier.getContexts')
	#s.register_function(Server.getContext,'Classifier.getContext')
	#s.register_function(Server.getCurrentContext,'Classifier.getCurrentContext')
	#s.register_function(Server.setCurrentContext,'Classifier.setCurrentContext')
	#s.register_introspection_functions()
	logging.debug('Starting server...')
	s.serve_forever()
except Exception:
	ThreadContext.ContextManager.get().saveContextData()
	raise
