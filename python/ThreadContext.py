"""

The Context module manages different contexts.

Contexts are maintained within a thread.

"""

import thread
import exceptions
import os
import os.path
import ThreadContext
import CSConfigParser

singleton = None

class ContextPathFactory:
	def getContextPath(self,contextId):
		raise exceptions.NotImplementedError
	def getDefaultContextPath(self):
		return ""

class Context:
	path = ""
	ident = 0
	def __init__(self,pth,id):
		self.path = pth
		self.ident = id
	def __str__(self):
		return self.path
	def __int__(self):
		return self.ident
	def str(self,str2):
		"""str(str) -> str
		"""
		return self.__str__() + "/" + str2

class ContextError(Exception): pass

class ContextManager:
	__contextPathFactory = None
	__defaultContext = None
	__threadToContextDict = {}
	__idToContextDict = {}
	__idToLockDict = {}
	__contextNumber = 1000
	@staticmethod
	def get():
		if ThreadContext.singleton != None:
			return ThreadContext.singleton
		else:
			raise Exception
	def __init__(self,pathFactory,contNum = 1000):
		if ThreadContext.singleton != None:
			raise Exception
		ThreadContext.singleton = self
		self.__contextNumber = contNum
		self.__contextPathFactory = pathFactory
		self.__defaultContext = Context(pathFactory.getDefaultContextPath(), 0)
		try:
			self.loadContextData()
			self.__defaultContext = self.__idToContextDict[0]
		except Exception: 
			# creating default context
			self.__defaultContext = Context(self.__contextPathFactory.getDefaultContextPath(), 0)
			# adding it to the file system
			self.addContext(self.__defaultContext)
			self.saveContextData()
	def getDefaultContext(self):
		"""getDefaultContext() -> Context
		"""
		return self.__defaultContext
	def getContext(self,id):
		return self.__idToContextDict[int(id)]
	def createNewContext(self):
		"""createNewContext() -> Context
		"""
		lock = self.__idToLockDict[self.__defaultContext.ident]
		try:
			lock.acquire()
			for i in range(0,self.__contextNumber):
				if not i in self.__idToContextDict:
					path = self.__contextPathFactory.getContextPath(i)
					newContext = Context(path, i)
					self.addContext(newContext)
					return newContext
		finally:
			lock.release()
	def setThreadContext(self,context,create = False):
		"""setThreadContext(Context,bool) -> None
		setThreadContext(int,bool) -> None
		"""
		# only context id is required
		id = int(context)
		thr_id = thread.get_ident()
		
		path = self.__contextPathFactory.getContextPath(id)
		
		# creating new Context
		newContext = Context(path, id)
		# binding the thread to the context
		self.__threadToContextDict[thr_id] = newContext
		
		if create:
			# if it IS a new context, add it to the file system
			if not id in self.__idToContextDict:
				addContext(newContext)
		else:
			if not id in self.__idToContextDict:
				raise ContextError
	def getThreadContext(self):
		"""getThreadContext() -> Context
		"""
		thr_id = thread.get_ident()
		try:
			return self.__threadToContextDict[thr_id]
		except KeyError:
			return self.__defaultContext
	def addContext(self,context):
		"""addContext(Context) -> None
		"""
		self.__idToContextDict[context.ident] = context
		self.__idToLockDict[context.ident] = thread.allocate_lock()
		try:
			print "making directory " + str(context)
			os.makedirs(str(context))
		except OSError, e:
			print str(e)
			pass
		pass
	def deleteContext(self,context):
		"""deleteContext(Context) -> None
		deleteContext(int) -> None
		"""
		lock = self.__idToLockDict[int(context)]
		try:
			lock.acquire()
			id = int(context)
			cont = self.__idToContextDict[id]
			del self.__idToContextDict[id]
			del self.__idToLockDict[id]
			self.cleanDir(str(cont))
		finally:
			lock.release()
	def cleanDir(self,dir):
		try:
			for file in os.listdir(dir):
				if os.path.isfile(dir + "/" + file):
					os.remove(dir + "/" + file)
				elif os.path.isdir(dir + "/" + file):
					self.cleanDir(dir + file)
			os.rmdir(dir)
		except Exception:
			pass
	def saveContextData(self):
		lock = self.__idToLockDict[self.__defaultContext.ident]
		try:
			lock.acquire()
			cfg = open(self.__defaultContext.str("contextData.cfg"),'w+')
			for k, v in self.__idToContextDict.iteritems():
				cfg.write(str(k) + " " + str(v) + "\n")
			cfg.close()
		finally:
			lock.release()
	def loadContextData(self):
		for s in open(self.__defaultContext.str("contextData.cfg"),'r'):
			id,path = s.split(" ")
			path = path.strip()
			id = int(id)
			self.__idToContextDict[id] = Context(path, id)
			self.__idToLockDict[id] = thread.allocate_lock()
	def getContextLock(self,context):
		return self.__idToLockDict[int(context)]
		
	def getContextNumbers(self):
		return self.__idToContextDict.keys()