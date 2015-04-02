#
#
#

import ThreadContext
import PMC
import xmlrpclib
import Server
import SimpleXMLRPCServer
import SocketServer
import logging
import CModule
import pickle

import RequestPMC
import ClassifyPMC

def diagnosis():
	#print "Classifier count: " + str(CModule.getClassifierCount())
	#print "ClassifierResults count: " + str(CModule.getClassifierResultsCount())
	#print "TrainingSetTopicsData count: " + str(CModule.getTrainingSetTopicsDataCount())
	#print "ParsedContent count: " + str(CModule.getParsedContentCount())
	#print "Measure count: " + str(CModule.getMeasureCount())
	#print "Parser count: " + str(CModule.getParserCount())
	#print "PyLogger count: " + str(CModule.getPyLoggerCount())
	pass

class PathFactory(ThreadContext.ContextPathFactory):
	def getContextPath(self,contextId):
		return "context" + str(int(contextId))
	def getDefaultContextPath(self):
		return "default"

class XMLRPCServer(SocketServer.ThreadingMixIn,SimpleXMLRPCServer.SimpleXMLRPCServer): pass

processingTypes = ['GRAMM','STEMMING','NONE']
measureTypes = ['K_SPECTRUM','FREQUENCY','EPISODES']
pmcDefault = 'DEFAULT'
decompositionTypes = ['oneVsOne','oneVsAll']
classificationTypes = ['SVM','KP']
pmcTypes = {'request' : 'REQUEST', 'classify' : 'CLASSIFY','classifyUrl' : 'CLASSIFY_URL'}


def getCurrentContext():
	try:
		cm = ThreadContext.ContextManager.get()
		defContext = cm.getDefaultContext()
		inp = file(defContext.str("currentContext.cfg"))
		curContextData = pickle.load(inp)
		return curContextData
	except Exception, e:
		logging.exception(e)
		raise Exception("No current context is set. Use setCurrentException method.")
	pass

def setCurrentContext(contextNumber):
	cm = ThreadContext.ContextManager.get()
	defContext = cm.getDefaultContext()
	out = file(defContext.str("currentContext.cfg"),"w")
	curContextData = contextNumber
	pickle.dump(curContextData,out)
	logging.debug("Current context set to " + str(contextNumber))
	# Returns 0 because xmlrpc doesn't support void.
	return 0

def validateStartTrainParams(ParserParams,MeasureParams,ClassifierParams,TSParams):
	if not isinstance(ParserParams,dict):
		raise Exception('Parser parameters is not a Structure')
	if not isinstance(MeasureParams,dict):
		raise Exception('Measure parameters is not a Structure')
	if not isinstance(TSParams,dict):
		raise Exception('Training set parameters is not a Structure')
	if not isinstance(ClassifierParams,dict):
		raise Exception('Classifier parameters is not a Structure')
	
	# parsing ParserParams
	processing = ParserParams['processing']
	if not processing in Server.processingTypes:
		raise Exception('Wrong value: processing = ' + str(processing))
	if processing == 'GRAMM':
		gramm_number = ParserParams['grammNumber']
		if not isinstance(gramm_number,int) or (gramm_number <= 0):
			raise Exception('Wrong value: gramm_number = ' + str(gramm_number))
	
	# parsing MeasureParams
	measure = MeasureParams['measure']
	if not measure in Server.measureTypes:
		raise Exception('Wrong value: measure = ' + str(measure))
	if measure == 'K_SPECTRUM':
		kernel_number = MeasureParams['kernelNumber']
		if not isinstance(kernel_number,int) or (kernel_number<=0):
			raise Exception('Wrong value: kernel_number = ' + str(kernel_number))
	
	# parsing TSParams
	numOfDocs = TSParams['numberOfDocuments']
	#numBasket = TSParams['basket']
	if not isinstance(numOfDocs,int) or (numOfDocs <= 0):
		raise Exception('Wrong value: numberOfDocuments = ' + str(numOfDocs))
	#sg if not isinstance(numBasket,int) or (numBasket < 0):
	#sg	raise Exception('Wrong value: basket = ' + str(numBasket))
	return True
	
	# parsing ClassifierParams
	decompType=ClassifierParams['decompType']
	if not decompType in Server.decompositionTypes:
		raise Exception('Wrong value: decompType = ' + str(decompType))
	classType=ClassifierParams['classType']
	if not classType in Server.classificationTypes:
		raise Exception('Wrong value: classType = ' + str(classType))
		
	
	# !!!!!TODO: Validate PMCPARAMS!!!!!!

def validatePredictParams(ContextName,Doc):
	if not isinstance(ContextName,int):
		raise Exception('Wrong value: ContextName = ' + str(ContextName))
		
	# parsing Doc
	Content = Doc['content']
	URL = Doc['URL']
	encoding = Doc['encoding']
	if not isinstance(Content, xmlrpclib.Binary):
		raise Exception('Wrong value: Content is not a binary string')
	if not isinstance(URL,str):
		raise Exception('Wrong value: URL = ' + str(URL))
	if not isinstance(encoding,str):
		raise Exception('Wrong value: encoding = ' + str(encoding))
	return True

def startTrain(ParserParams,MeasureParams,ClassifierParams,TSParams,PMCType=None,PMCParams=None):
	try:
		try:
			print 'startTrain'
			print str(ParserParams)
			print str(MeasureParams)
			print str(ClassifierParams)
			print str(TSParams)
			print str(PMCType)
			print str(PMCParams)
			
			logging.info('Start train')
			
			# validating parameters
			validateStartTrainParams(ParserParams, MeasureParams, ClassifierParams, TSParams)

			#validateStartTrainParams(PMCParams['parserParams'], PMCParams['measureParams'], PMCParams['classifierParams'], PMCParams['tsParams'])
			
			logging.debug('Parameters validated')
			
			# creating correct PMC
			pmc = None
			if (PMCType == None) or (PMCType == Server.pmcDefault):
				pmc = PMC.DefaultPMC()
				
			if (PMCType == pmcTypes['request']):
				pmc = RequestPMC.RequestPMC()
			
			if (PMCType == pmcTypes['classify']):
				pmc = ClassifyPMC.ClassifyPMC('CONTENT')
			
			if (PMCType==pmcTypes['classifyUrl']):
				pmc = ClassifyPMC.ClassifyPMC('URL')
			
			logging.debug('PMC created')
			
			# managing context
			cm = ThreadContext.ContextManager.get()
			context = cm.createNewContext()
			
			cm.setThreadContext(context,False)
			
			
			try:
				logging.debug('Thread context created')
				print "a"								
				pmc.train(ParserParams, MeasureParams, ClassifierParams, TSParams, PMCType, PMCParams, context)
				print "b"
				logging.info('Train completed')
				
				logging.debug('Updating context data')
				
				cm.saveContextData()
				
				setCurrentContext(int(context)) # makes the last trained context the current context
				
				return int(context)
			except Exception:
				cm.deleteContext(context)
				cm.saveContextData()
				raise
		except Exception, e:
			logging.exception(e)
			raise
	finally:
		diagnosis()

def predict(ContextName,Doc) :
	try:
		try:
			print 'predict'
			
			# validating parameters
			validatePredictParams(ContextName,Doc)
			
			logging.debug('Parameters validated')
			
			# getting context
			cm = ThreadContext.ContextManager.get()
			cm.setThreadContext(ContextName, False)
			context = cm.getThreadContext()
			
			# reading PMC type
			contextData = getContext(int(ContextName))
			PMCType = contextData['pmcType']
			logging.debug('PMCType = ' + str(PMCType))
			
			# creating correct PMC
			pmc = None
			if PMCType == Server.pmcDefault:	
				pmc = PMC.DefaultPMC()
				
			if (PMCType == pmcTypes['request']):
				pmc = RequestPMC.RequestPMC()
			
			if (PMCType == pmcTypes['classify']):
				pmc = ClassifyPMC.ClassifyPMC('CONTENT')

			if (PMCType == pmcTypes['classifyUrl']):
				pmc = ClassifyPMC.ClassifyPMC('URL')
			
			logging.debug('PMC created')
			
			return pmc.predict(Doc, context)
		except Exception, e:
			logging.exception(e)
			raise
	finally:
		diagnosis()

def retrain(ContextName,TSParams) :
	try:
		
		logging.info('Retrain')
		
		
		# getting context
		cm = ThreadContext.ContextManager.get()
		cm.setThreadContext(ContextName, False)
		context = cm.getThreadContext()
		
		# reading PMC type
		contextData = getContext(int(ContextName))
		PMCType = contextData['pmcType']
		logging.debug('PMCType = ' + str(PMCType))
		
		# creating correct PMC
		pmc = None
		if PMCType == Server.pmcDefault:	
			pmc = PMC.DefaultPMC()
			
		if (PMCType == pmcTypes['request']):
			pmc = RequestPMC.RequestPMC()
		
		if (PMCType == pmcTypes['classify']):
			pmc = ClassifyPMC.ClassifyPMC('CONTENT')
		
		if (PMCType == pmcTypes['classifyUrl']):
			pmc = ClassifyPMC.ClassifyPMC('URL')
		
		logging.debug('PMC created')
		
		return pmc.retrain(TSParams, context)
	except Exception, e:
		logging.exception(e)
		raise

def deleteTopic(ContextName,TopicNumber) :
	try:
		
		logging.info('Deleting topic')
		# getting context
		cm = ThreadContext.ContextManager.get()
		cm.setThreadContext(ContextName, False)
		context = cm.getThreadContext()
		
		# reading PMC type
		contextData = getContext(int(ContextName))
		PMCType = contextData['pmcType']
		logging.debug('PMCType = ' + str(PMCType))
		
		logging.debug('PMCType = ' + str(PMCType))
		
		# creating correct PMC
		pmc = None
		if (PMCType==pmcTypes['retrain']):
			pmc = RetrainPMC.RetrainPMC()
		
		logging.debug('PMC created')
		
		pmc.deleteTopic(TopicNumber, context)
	except Exception, e:
		logging.exception(e)
		raise

def delete(ContextName):
	try:
		cm = ThreadContext.ContextManager.get()
		cm.deleteContext(ContextName)
		cm.saveContextData()
		return True
	except Exception, e:
		logging.exception(e)
		raise

def getContexts():
	cm = ThreadContext.ContextManager.get()
	contextsData = {}
	for i in cm.getContextNumbers():
		if i != int(cm.getDefaultContext()):
			contextsData[str(i)] = getContext(i)
	print contextsData
	return contextsData

def getContext(i):
	cm = ThreadContext.ContextManager.get()
	context = cm.getContext(i)
	path = context.str('context.cfg')
	inp = file(path)
	contextData = pickle.load(inp)
	return contextData

