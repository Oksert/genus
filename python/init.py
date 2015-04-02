
import ThreadContext
import logging
import logging.handlers
import CModule


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

initializeCModule()
