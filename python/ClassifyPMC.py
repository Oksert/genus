#
#
#

import ThreadContext
import CModule
import xmlrpclib
import logging
import sys
from PMC import PMC,TopicMap
from LinkPMC import LinkPMC
from UrlLinkPMC import UrlLinkPMC
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

def intToStr(arr):
	for i in range(0,len(arr)):
		arr[i]=str(arr[i])
	return arr
class ClassifyPMC(PMC):
    def __init__(self,linkPMCType):
	PMC.__init__(self)
	self.__linkPMCType = linkPMCType
	self.__types = {'content' : 'CONTENT','url' : 'URL'}
		
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
        
        # create new context for LinkPMC
        cm = ThreadContext.ContextManager.get()
        self.__linkContext = cm.createNewContext()
        # change active context
        logging.debug('Creating LinkPMC')
        cm.setThreadContext(self.__linkContext,False)
        # create LinkPMC
        if (self.__linkPMCType==self.__types['content']):
		linkPMC = LinkPMC()
	else:
		linkPMC = UrlLinkPMC()
        # train LinkPMC
        logging.debug('Training LinkPMC')
        linkPMC.train(misc['parserParams'],misc['measureParams'],
                misc['classifierParams'],misc['tsParams'],self.__linkContext)
        # change active context backwards
        cm.setThreadContext(self.__context,False)
	f = open('links.txt','w')
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
            linksToClassify = []
            images = [] # yet we don't deal with images
            # request topics for links
            if len(handledLinks) != 0:
		## need to write into db all topics, so i can later replace link topics with its db ids
                uri = doc['document']['URL']
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
                    else:
                        # add link that needs to be classified
                        linksToClassify.append(link)
            # request linkPMC for classifiyng links
            classifyResult = {}
	    parseResults.writeIds()
            if (len(linksToClassify) != 0):
                logging.debug('Classifying links with LinkPMC')
                cm.setThreadContext(self.__linkContext,False)
                logging.debug('Link classification started')
                classifyResult = linkPMC.classifyLinks(linksToClassify,doc['document']['URL'],self.__linkContext)
                logging.debug('Link classification completed')
                cm.setThreadContext(self.__context,False)
                # replace newly classified links with topics
                for link in classifyResult:
                    f.write(link+": " + str(classifyResult[link])+"\n")
		    classifyResult[link]=intToStr(classifyResult[link])
                    idVect = CModule.listToSTLVector(classifyResult[link],len(classifyResult[link]))
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
            del linksToClassify
            del classifyResult
            
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
        del linkPMC
        
        pass
    
    def predict(self,doc,context):
	
	docContent = doc['content']
	docUrl = doc['URL']
	encoding = doc['encoding']
		
	all_time1 = clock()
        self.__context = context
        #logging.info('Classifying document: ' + str(docUrl))
        logging.debug('Loading parameters')
        # load parameters
        parserParams, measureParams, classifierParams, tsParams, misc = self.restoreParams()
        self.__linkContext = misc['linkPMCcontext']
        
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
	topicsMap = TopicMap()
        topicsMap.loadFromFile(self.__context.str('topics.cfg'))
        # launching LinkPMC
        cm = ThreadContext.ContextManager.get()
        self.__linkContext = cm.getContext(self.__linkContext)
        #restore LinkPMC parameters
        cm.setThreadContext(self.__linkContext,False)
        if (self.__linkPMCType==self.__types['content']):
		linkPMC = LinkPMC()
	else:
		linkPMC = UrlLinkPMC()
        cm.setThreadContext(self.__context,False)
        #get links from document
        links = parsingResult.getLinks()
        handledLinks = CModule.handleLinks(links) # create python list from class VectorString
        linksResult = {}
        linksToClassify = []
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
                else:
                    # add link that needs to be classified
                    linksToClassify.append(link)
        
        classifyResult = {}
	#print str(len(linksToClassify))
        if (len(linksToClassify) != 0):
        #if False:
            # request linkPMC for classifiyng links
            cm.setThreadContext(self.__linkContext,False)
            logging.debug("Classifying links with LinkPMC")
	    parsingResult.writeIds()
	    link_time1 = clock()
            classifyResult = linkPMC.classifyLinks(linksToClassify,docUrl,self.__linkContext)
	    link_time2 = clock()
	    print "Link time: " + str(link_time2-link_time1)
            cm.setThreadContext(self.__context,False)
            # replace newly classified links with topics
            for link in classifyResult:
		classifyResult[link]=intToStr(classifyResult[link])
                idVect = CModule.listToSTLVector(classifyResult[link],len(classifyResult[link]))
                parsingResult.replaceLink(link,idVect)
                del idVect
            logging.debug('Link classification completed')
    
        logging.debug('Loading measure')
	#print str(context.str('measure.bin'))
        measure.load(context.str('measure.bin'))
        
        # use it all to predict
        logging.debug('Running classifier')
        result = classifier.predict(parsingResult,measure,int(tsParams['numberOfTopics']))
        # construct resulting structure
        logging.debug('Constructing result')
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
        del linksToClassify
        del classifyResult
        del linkPMC
	
	all_time2 = clock()
	print "Overall predict time: " + str(all_time2-all_time1)
        
        return resultingStruct
    def saveParams(self,pp,mp,cp,tp,misc):
        logging.debug('Saving parameters')
        parser = CSConfigParser.CSConfigParser()
        # this is read from the predict call
        parser.add_section('PMC')
        if (self.__linkPMCType==self.__types['content']):
		parser.set('PMC', 'type', 'CLASSIFY')
	else: 
		parser.set('PMC','type','CLASSIFY_URL')	
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
            
        logging.debug('Saving misc parameters')
        parser.add_section('misc')
        for key, val in tp.items():
            parser.set('misc', 'linkPMCcontext', str(int(self.__linkContext)))
            
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
        misc = {}
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
        if parser.has_section('misc'):
            for opt, val in parser.items('misc'):
                misc[opt] = val
        def convertToInt(d,param):
            if param in d:
                d[param] = int(d[param])
            pass
        
        convertToInt(pp,'grammNumber')
        convertToInt(mp,'kernelNumber')
        convertToInt(tp,'numberOfDocuments')
        convertToInt(tp,'numberOfTopics')
        convertToInt(misc,'linkPMCcontext')
        
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


