#
#
#

import ThreadContext
import CModule
import CSConfigParser
import xmlrpclib
import logging
import sys
from PMC import PMC,TopicMap
from time import clock


def diagnosis():
    #print "Classifier count: " + str(CModule.getClassifierCount())
    #print "ClassifierResults count: " + str(CModule.getClassifierResultsCount())
    #print "TrainingSetTopicsData count: " + str(CModule.getTrainingSetTopicsDataCount())
    #print "ParsedContent count: " + str(CModule.getParsedContentCount())
    #print "Measure count: " + str(CModule.getMeasureCount())
    #print "Parser count: " + str(CModule.getParserCount())
    #print "PyLogger count: " + str(CModule.getPyLoggerCount())
    pass


class UrlLinkPMC(PMC):
    """ This PMC type supports only basic functions.
    It parses the documents, performs no dictionary manipulation,
    trains the classifier, then uses the classifier to predict.
    It does not process links in any way.
    """
    #def requestDocument(self,basket,docNum):
        #s = xmlrpclib.Server(self.dmmUri)
        #try:
            #logging.debug('Attempting getting document.')
            #return s.Kernel.getLink(int(basket),int(docNum))
        #except Exception, e:
            #print e
            #raise e
    def saveLinks(self,classifyResults):
        s = xmlrpclib.Server(self.dmmUri)
        print "YYEEAAHH"
        try:
            logging.debug('Attempting saving document.')
            keys = classifyResults.keys()
            for key in classifyResults:
                if (key == None) or (classifyResults[key] == None):
                    print "Error"
                else: 
                    i = s.Kernel.saveLink(key,classifyResults[key])
        except Exception, e:
            print e
            raise e
    def train(self,parserParams,measureParams,classifierParams,tsParams,context):
        self.__context = context
        self.__encoding = "UTF-8"
	basket = tsParams['basket']
        
        # for each document
        results = CModule.vectorVectorId()
        topics = CModule.TopicsData()
        topicsMap = TopicMap()
        logging.debug('Starting document cycle')
        
	logging.debug('Creating parser')
	parser = CModule.createTrainPackageParser(parserParams,measureParams,self.__encoding)	

        for i in range(0,tsParams['numberOfDocuments']):
            logging.info('Requesting document ' + str(i))
            doc = self.requestDocument(basket,i)
            #self.requestDocument(i)
            # store results
            parser.addChunk(doc['document']['URL'])
            
            topics.addDocument(len(doc['topics']))
            
            for k in range(0,len(doc['topics'])):
                topics.setDocumentTopic(i,k,topicsMap.virtual(doc['topics'][k]))
            del doc
            
	logging.debug('Parsing all links together')
	parser.parse()
	logging.debug('Getting results')
	for i in range (0,tsParams['numberOfDocuments']):
		results.append(parser.getResult(i).getIds())

        diagnosis()
        # create measure
        measure = CModule.createMeasure(parserParams,measureParams)
        # use the results
        measure.preload(results)
        # save measure
        measure.save(context.str('measure.bin'))
        
        # the None parameter is here in case, we decide to parametrize classifier
        classifier = CModule.createClassifier(classifierParams)
        # train
        classifier.train(measure,topics)
        logging.debug('UrlLinkPMC classifying completed.')
        # save parameters
        tsParams['numberOfTopics'] = topicsMap.getNumberOfTopics()
        topicsMap.saveToFile(context.str('topics.cfg'))
        self.saveParams(parserParams,measureParams,classifierParams,tsParams)
        
        del measure
        del classifier
        del results
        del topics
        del topicsMap
        
        pass
    
    def classifyLinks(self,linksToClassify,docUrl,context):
        self.__context = context
	self.__encoding = 'UTF-8'
        if (len(linksToClassify) == 0):
            pass
        #logging.info('Classifying document: ' + str(docUrl))
        logging.debug('Loading parameters')
        # load parameters
        parserParams, measureParams, classifierParams, tsParams = self.restoreParams(self.__context)
        
        logging.debug('Creating parser')
        # create objects
        parser = CModule.createPredictPackageParser(parserParams,measureParams,self.__encoding)
        
        # print "anykey"
        # sys.stdin.read(1)
        logging.debug('Creating measure')
        measure = CModule.createMeasure(parserParams,measureParams)
        logging.debug('Loading measure')
        measure.load(context.str('measure.bin'))

        # print "anykey"
        # sys.stdin.read(1)
        logging.debug('Creating classifier')
        classifier = CModule.createClassifier(classifierParams)
        # print "anykey"
        # sys.stdin.read(1)
        
        
        topicsMap = TopicMap()
        topicsMap.loadFromFile(self.__context.str('topics.cfg'))
        resultingStruct = {}
	a_time = clock()
        #cycle for every url in linksToClassify
        for i in range(0,len(linksToClassify)):
            # add chunk to parser
            parser.addChunk(linksToClassify[i])
            
        # parsing all links together
	parser.parse()
	# getting results
	for i in range(0,len(linksToClassify)):
		logging.debug('Running classifier')
		result = classifier.predict(parser.getResult(i),measure,int(tsParams['numberOfTopics']))
       		resultingStruct[str(linksToClassify[i])] = self.buildResultingStructure(result, topicsMap)
	       	del result
	b_time = clock()
	print "Cycle time: " + str(b_time-a_time)
        logging.debug('Constructing result')
        logging.info(str(resultingStruct))
        
        #save topics of classified links into KB
        #self.saveLinks(resultingStruct)
        
        del parser
        del measure
        del classifier
        del topicsMap
        del linksToClassify

        return resultingStruct
    def saveParams(self,pp,mp,cp,tp):
        logging.debug('Saving parameters')
        parser = CSConfigParser.CSConfigParser()
        # this is read from the predict call
        parser.add_section('PMC')
        parser.set('PMC', 'type', 'LINK')
        # the rest is read from the restoreParams call
        
        logging.debug('Saving parser parameters')
        parser.add_section('parser')
        for key, val in pp.items():
            parser.set('parser', key, str(val))
            
        logging.debug('Saving measure parameters')
        parser.add_section('measure')
        for key, val in mp.items():
            parser.set('measure', key, str(val))

        logging.debug('Saving measure parameters')
        parser.add_section('classifier')
        for key, val in cp.items():
            parser.set('classifier', key, str(val))            

        logging.debug('Saving training set parameters')
        parser.add_section('trainingSet')
        for key, val in tp.items():
            parser.set('trainingSet', key, str(val))
            
        # misc parameters are not needed in this PMC
        f = open(self.__context.str('pmc.cfg'),'w+')
        parser.write(f)
        f.close()
        
        logging.debug('Parameters saved')
        pass
    def restoreParams(self,context):
        parser = CSConfigParser.CSConfigParser()
        parser.read(context.str('pmc.cfg'))
        pp = {}
        mp = {}
        cp = {}
        tp = {}
        if parser.has_section('parser'):
            for opt, val in parser.items('parser'):
                pp[opt] = val
        if parser.has_section('measure'):
            for opt, val in parser.items('measure'):
                mp[opt] = val
        if parser.has_section('classifier'):
            for opt, val in parser.items('classifier'):
                cp[opt] = val
        if parser.has_section('trainingSet'):
            for opt, val in parser.items('trainingSet'):
                tp[opt] = val
        def convertToInt(d,param):
            if param in d:
                d[param] = int(d[param])
            pass
        
        convertToInt(pp,'grammNumber')
        convertToInt(mp,'kernelNumber')
        convertToInt(tp,'numberOfDocuments')
        convertToInt(tp,'numberOfTopics')
        
        return (pp,mp,cp,tp)
    def buildResultingStructure(self,result,topicsMap):
        res = []
        for i in range(0,len(result.resultingTopics)):
            res.append(topicsMap.base(result.resultingTopics[i]))
        return res
