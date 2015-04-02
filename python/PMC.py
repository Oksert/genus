#
#
#

import ThreadContext
import CModule
import xmlrpclib
import logging
import sys
import pickle
import PropertiesConfiguration


class PMC:
	def __init__(self):
		self.dmmUri = PropertiesConfiguration.getKernelUrl()
	def train(self,parserParams,measureParams,tsParams,misc,context):
		pass
	def predict(self,doc,context):
		pass
	def requestDocument(self,basket,docNum):
		s = xmlrpclib.Server(self.dmmUri)
		try:
			logging.debug('Attempting getting document.')
			return s.Kernel.getDocument(int(basket),int(docNum))
		except Exception, e:
			print e
			raise e	


class TopicMap:
	def __init__(self):
		self.counter = 0
		self.baseToVirtual = {}
		self.virtualToBase = {}
	def virtual(self,base):
		if base not in self.baseToVirtual:
			self.addTopic(base)
		return self.baseToVirtual[base]
	def base(self,virtual):
		return self.virtualToBase[virtual]
	def addTopic(self,baseTopic):
		self.baseToVirtual[baseTopic] = self.counter
		self.virtualToBase[self.counter] = baseTopic
		self.counter += 1
	def getNumberOfTopics(self):
		return self.counter
	def saveToFile(self,filename):
		f = open(filename,"w+")
		for (key,val) in self.baseToVirtual.items():
			f.write(str(key) + " " + str(val) + "\n")
		f.close()
	def loadFromFile(self,filename):
		f = open(filename,"r")
		for line in f:
			pair = line.split(" ")
			baseTopic = int(pair[0])
			virtualTopic = int(pair[1])
			self.counter = max(virtualTopic + 1,self.counter)
			self.baseToVirtual[baseTopic] = virtualTopic
			self.virtualToBase[virtualTopic] = baseTopic
			pass
		f.close()
	def deleteTopic(self,topic):
		virtualTopic = self.virtual(topic)
		del self.virtualToBase[virtualTopic]
		for k in range(virtualTopic + 1, self.counter):
			self.virtualToBase[k - 1] = self.virtualToBase[k]
		if virtualTopic < self.counter - 1:
			del self.virtualToBase[self.counter-1]
		self.counter -= 1
		self.baseToVirtual = {}
		for (k,v) in self.virtualToBase.items():
			self.baseToVirtual[v] = k
		pass
	pass

class DefaultPMC(PMC):
	""" This PMC type supports only basic functions.
	It parses the documents, performs no dictionary manipulation,
	trains the classifier, then uses the classifier to predict.
	It does not process links in any way.
	"""
	def train(self,parserParams,measureParams,classifierParams,tsParams,pmcType,pmcParams,threadContext):
		if pmcType == None:
			pmcType = "DEFAULT"
		
		if pmcParams == None:
			pmcParams = {}
	
		self.__context = threadContext
		basket = tsParams['basket']

		results = CModule.vectorVectorId()
		topics = CModule.TopicsData()
		topicsMap = TopicMap()
		
		logging.debug('Starting document cycle')
		
		for i in range(0,tsParams['numberOfDocuments']):
			print "i=", i
			logging.debug('Creating parser')
			parser = CModule.createTrainParser(parserParams,measureParams)
			
			logging.info('Requesting document ' + str(i))
			#sg: doc = self.requestDocument(basket,i)
			
			doc = basket[i]
			
			logging.debug('Parsing document')
			#sg: parseResults = parser.parse(doc['document']['content'].data,doc['document']['encoding'])
			parseResults = parser.parse(doc['document']['content'],doc['document']['encoding'])

			results.append(parseResults.getIds())
			logging.debug('Parse succeeded')
			topics.addDocument(len(doc['topics']))
			for k in range(0,len(doc['topics'])):
				topics.setDocumentTopic(i,k,topicsMap.virtual(doc['topics'][k]))
			
			del parser
			del doc
			del parseResults
		
		measure = CModule.createMeasure(parserParams,measureParams)
		measure.preload(results)
		measure.save(threadContext.str('measure.bin'))

		classifier = CModule.createClassifier(classifierParams)
		classifier.train(measure,topics)
		
		
		tsParams['numberOfTopics'] = topicsMap.getNumberOfTopics()
		topicsMap.saveToFile(threadContext.str('topicsMap.cfg'))
		topics.saveToFile(threadContext.str('topics.cfg'))
		
		contextData = {}
		contextData['numContext'] = threadContext.ident
		contextData['topics'] = topicsMap.baseToVirtual.keys()
		contextData['parserParams'] = parserParams
		contextData['measureParams'] = measureParams
		contextData['classifierParams'] = classifierParams
		contextData['tsParams'] = tsParams
		contextData['pmcType'] = pmcType
		contextData['pmcParams'] = pmcParams
		self.saveContextData(contextData)
		
		print "before del"
		del measure
		del classifier
		del results
		del topics
		del topicsMap
		print "adter del"
		pass
	
	#def predict(self,docContent,docUrl,predictParams,context):
	def predict(self,doc,context):
		docContent = doc['content']
		docURL = doc['URL']
		encoding = doc['encoding']
		
		self.__context = context
		logging.debug('Loading parameters')
		contextData = self.loadContextData()
		
		logging.debug('Creating parser')
		parser = CModule.createPredictParser(contextData['parserParams'],contextData['measureParams'])
		
		logging.debug('Creating measure')
		measure = CModule.createMeasure(contextData['parserParams'],contextData['measureParams'])
		
		logging.debug('Creating classifier')
		classifier = CModule.createClassifier(contextData['classifierParams'])
		
		logging.debug('Parsing content')
		parsingResult = parser.parse(docContent,encoding)
		logging.debug('Loading measure')
		measure.load(context.str('measure.bin'))
		
		logging.debug('Running classifier')
		result = classifier.predict(parsingResult,measure,int(contextData['tsParams']['numberOfTopics']))
		
		logging.debug('Constructing result')
		topicsMap = TopicMap()
		topicsMap.loadFromFile(self.__context.str('topicsMap.cfg'))
		resultingStruct = self.buildResultingStructure(result,topicsMap)
		logging.info(str(resultingStruct))
		
		del parser
		del measure
		del classifier
		del parsingResult
		del result
		del topicsMap
		
		return resultingStruct
	def retrain(self,tsParamsRetrain,context):
		self.__context = context
		basket = tsParamsRetrain['basket']
		
		contextData = self.loadContextData()
		
		parserParams = contextData['parserParams']
		measureParams = contextData['measureParams']
		classifierParams = contextData['classifierParams']
		pmcParams = contextData['pmcParams']
		
		# for each document
		results = CModule.vectorVectorId()
		topics = CModule.TopicsData()
		topics.loadFromFile(self.__context.str('topics.cfg'))
		topicsMap = TopicMap()
		topicsMap.loadFromFile(self.__context.str('topicsMap.cfg'))
		logging.debug('Starting document cycle')
		
		for i in range(0,tsParamsRetrain['numberOfDocuments']):
			# create parser
			logging.debug('Creating parser')
			parser = CModule.createTrainParser(parserParams,measureParams)
			logging.info('Requesting document ' + str(i))
			doc = self.requestDocument(basket,i)
			# store results
			logging.debug('Parsing document')
			#logging.debug('Encoding: ' + str(doc['encoding']))
			parseResults = parser.parse(doc['document']['content'].data,doc['document']['encoding'])
			results.append(parseResults.getIds())
			
			logging.debug('Parse succeeded')
			
			topics.addDocument(len(doc['topics']))
			logging.debug('Added a document to the topics struct.')
			for k in range(0,len(doc['topics'])):
				topics.setDocumentTopic(topics.getNumberOfDocuments()-1 ,k,topicsMap.virtual(doc['topics'][k]))
			
			logging.debug('Document topics saved')
			del parser
			del doc
			del parseResults
		
		measure = CModule.createRetrainMeasure(parserParams,measureParams)
		measure.load(context.str('measure.bin'))
		measure.retrain(results)
		
		classifier = CModule.createClassifier(classifierParams)
		classifier.incrementive_learning(measure,topics,tsParamsRetrain['numberOfDocuments'])
		logging.debug('Incrementive learning succeeded.')
		
		measure.save(context.str('measure.bin'))

		topicsMap.saveToFile(context.str('topicsMap.cfg'))
		topics.saveToFile(context.str('topics.cfg'))
		
		tsParams['numberOfDocuments'] = tsParams['numberOfDocuments'] + tsParamsRetrain['numberOfDocuments']
		tsParams['numberOfTopics'] = topicsMap.getNumberOfTopics()
		contextData['tsParams'] = tsParams
		saveContextData(contextData)
		
		logging.debug('Parameters saved.')
		
		del measure
		del classifier
		del results
		del topics
		del topicsMap
		
		return 0
	def saveContextData(self,contextData):
		path = self.__context.str('context.cfg')
		print path
		output = file(path,"w")
		pickle.dump(contextData,output)
		pass
	def loadContextData(self):
		path = self.__context.str('context.cfg')
		output = file(path)
		context = pickle.load(output)
		return context
	def saveParams(self,numContext,topics,pp,mp,cp,tp,pmcType,pmcParams):
		logging.debug('Saving parameters')
		
		path = self.__context.str('context.cfg')
		
		output = file(path)
		
		context = {}
		context['numContext'] = numContext
		context['topics'] = topics
		context['parserParams'] = pp
		context['measureParams'] = mp
		context['classifierParams'] = cp
		context['tsParams'] = tp
		context['pmcType'] = pmcType
		context['pmcParams'] = pmcParams
		
		pickle.dump(context,output)
		
		logging.debug('Parameters saved')
		pass
	def buildResultingStructure(self,result,topicsMap):
		res = {}
		res['topics'] = {}
		res['weights'] = {}
		res['numberRelevant'] = len(result.resultingTopics)

		for i in range(0,len(result.ranks)):
			res['topics'][str(i)] = topicsMap.base(result.ranks[i])
			res['weights'][str(i)] = result.weights[i]
		return res


