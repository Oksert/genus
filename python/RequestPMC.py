#
#
#

import ThreadContext
import CModule
import xmlrpclib
import logging
import sys
from PMC import PMC,TopicMap

def diagnosis():
    #print "Classifier count: " + str(CModule.getClassifierCount())
    #print "ClassifierResults count: " + str(CModule.getClassifierResultsCount())
    #print "TrainingSetTopicsData count: " + str(CModule.getTrainingSetTopicsDataCount())
    #print "ParsedContent count: " + str(CModule.getParsedContentCount())
    #print "Measure count: " + str(CModule.getMeasureCount())
    #print "Parser count: " + str(CModule.getParserCount())
    #print "PyLogger count: " + str(CModule.getPyLoggerCount())
    pass

class RequestPMC(PMC):
    def setLinks(self,uri,base,links,images=None):
        s = xmlrpclib.Server(self.dmmUri)
        try:
            logging.debug('Attempting getting document.')
            return s.Kernel.setLinks(uri,base,links,images)
        except Exception, e:
            print e
            raise e
    def setTrustedLinks(self,uri,base,links,images=None):
        s = xmlrpclib.Server(self.dmmUri)
        try:
            logging.debug('Attempting getting document.')
            return s.Kernel.setTrustedLinks(uri,base,links,images)
        except Exception, e:
            print e
            raise e  
    """ This PMC type parses document as DefaultPMC,
	requests KB for topics set for every link in this document,
	then replaces links in parsedContent with its topics sets
    """
    def train(self,parserParams,measureParams,classifierParams,tsParams,misc,context):
        self.__context = context
	basket = tsParams['basket']

        # for each document
        results = CModule.vectorVectorId()
        topics = CModule.TopicsData()
        topicsMap = TopicMap()
        logging.debug('Starting document cycle')
        
        for i in range(0,tsParams['numberOfDocuments']):
            # create parser
            logging.debug('Creating parser')
            parser = CModule.createTrainParser(parserParams,measureParams)
            logging.info('Requesting document ' + str(i))
            doc = self.requestDocument(basket,i)
            # store results
            logging.debug('Parsing document')
            logging.debug('Encoding: ' + str(doc['document']['encoding']))
            parseResults = parser.parse(doc['document']['content'].data,doc['document']['encoding'])
           
            #get links from document
            links = parseResults.getLinks()
            handledLinks = CModule.handleLinks(links) # create python list from class VectorString
            linksResult = {}
            images = [] # yet we don't deal with images
            # request topics for links
            if len(handledLinks) != 0:
		# need to write into db all topics, so i can later replace link topics with its db ids
                uri = doc['document']['url']
                base = parseResults.getBase()
                logging.info('Requesting trusted links topics from KB')
                linksResult = self.setTrustedLinks(uri,base,handledLinks,images)
                #replace link with ids
                logging.debug('Replacing links with its topic sets')
                for link in handledLinks:
                    if link in linksResult:
			linksResult[link]=intToStr(linksResult[link])
                        idVect = CModule.listToSTLVector(linksResult[link],len(linksResult[link]))
                        parseResults.replaceLink(link,idVect)
                        del idVect

            results.append(parseResults.getIds())
        
            
            logging.debug('Parse succeeded')
            
            topics.addDocument(len(doc['topics']))
            
            for k in range(0,len(doc['topics'])):
                topics.setDocumentTopic(i,k,topicsMap.virtual(doc['topics'][k]))
            del parser
            del doc
            del parseResults
            del links
            del handledLinks
	del images
	del linksResult

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
        
        # save parameters
        tsParams['numberOfTopics'] = topicsMap.getNumberOfTopics()
        topicsMap.saveToFile(context.str('topics.cfg'))
        self.saveParams(parserParams,measureParams,classifierParams,tsParams,misc)
        
        del measure
        del classifier
        del results
        del topics
        del topicsMap
        
        pass
    
    def predict(self,doc,context):
	docContent = doc['content']
	docURL = doc['URL']
	encoding = doc['encoding']
		
        self.__context = context
        #logging.info('Classifying document: ' + str(docUrl))
        logging.debug('Loading parameters')
        # load parameters
        parserParams, measureParams, classifierParams, tsParams, misc = self.restoreParams()
        
        logging.debug('Creating parser')
        # create objects
        parser = CModule.createPredictParser(parserParams,measureParams)
        
        # print "anykey"
        # sys.stdin.read(1)
        logging.debug('Creating measure')
        measure = CModule.createMeasure(parserParams,measureParams)

        # print "anykey"
        # sys.stdin.read(1)
        logging.debug('Creating classifier')
        classifier = CModule.createClassifier(classifierParams)
        # print "anykey"
        # sys.stdin.read(1)
        
        # parse
        logging.debug('Parsing content')
        parsingResult = parser.parse(docContent.data,encoding)
        
	 #get links from document
        links = parsingResult.getLinks()
        handledLinks = CModule.handleLinks(links) # create python list from class VectorString
        linksResult = {}
        images = [] # yet we don't deal with images
        # request topics for links
        if len(handledLinks) != 0:
            uri = docUrl
            base = parsingResult.getBase()
            logging.info('Requesting trusted links topics from KB')
            linksResult = self.setTrustedLinks(uri,base,handledLinks,images)
            #replace link with ids
            logging.debug('Replacing links with its topic sets')
            for link in handledLinks:
                if link in linksResult:
			linksResult[link]=intToStr(linksResult[link])
                	idVect = CModule.listToSTLVector(linksResult[link],len(linksResult[link]))
                        parsingResult.replaceLink(link,idVect)
                        del idVect


        logging.debug('Loading measure')
        measure.load(context.str('measure.bin'))
        
        # use it all to predict
        logging.debug('Running classifier')
        result = classifier.predict(parsingResult,measure,int(tsParams['numberOfTopics']))
        
        # construct resulting structure
        logging.debug('Constructing result')
        topicsMap = TopicMap()
        topicsMap.loadFromFile(self.__context.str('topics.cfg'))
        resultingStruct = self.buildResultingStructure(result,topicsMap)
        logging.info(str( resultingStruct))
        
        del parser
        del measure
        del classifier
        del parsingResult
        del result
        del topicsMap
        del links
	del handledLinks	
	del images
	del linksResult
	return resultingStruct
    def saveParams(self,pp,mp,cp,tp,misc):
        logging.debug('Saving parameters')
        parser = CSConfigParser.CSConfigParser()
        # this is read from the predict call
        parser.add_section('PMC')
        parser.set('PMC', 'type', 'REQUEST')
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
    def restoreParams(self):
        parser = CSConfigParser.CSConfigParser()
        parser.read(self.__context.str('pmc.cfg'))
        pp = {}
        mp = {}
        cp = {}
        tp = {}
        misc = None
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
        
        return (pp,mp,cp,tp,misc)
    def buildResultingStructure(self,result,topicsMap):
        res = {}
        res['topics'] = {}
        res['weights'] = {}
        res['numberRelevant'] = len(result.resultingTopics)

        for i in range(0,len(result.ranks)):
            #logging.debug("i: ")
            #logging.debug(i)
            #logging.debug(result.ranks[i])
            #logging.debug(result.weights[i])
            res['topics'][str(i)] = topicsMap.base(result.ranks[i])
            res['weights'][str(i)] = result.weights[i]
        #logging.debug(topicsMap.virtualToBase)
        #logging.debug(topicsMap.baseToVirtual)
        return res


