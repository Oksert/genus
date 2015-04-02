from Server import *
from ThreadContext import *
from PMC import *
import logging
import logging.handlers
import glob
from dbHandler import *

def main():
	parserParams=dict([('processing', 'STEMMING')])
	measureParams=dict([('measure','FREQUENCY')])
	trainSet = "/home/kostya/Downloads/Dataset/*.txt"
	classifierParams=dict([('decompType', 'oneVsOne'), ('classType','SVM')])
	bask = []
	allFiles = glob.glob(trainSet)
	print len(allFiles)
	allTopics = getAllTopics(trainSet)
	for f in allFiles:
		with open (f, "r") as myfile:
    			data=myfile.read().replace('\n', '')

		doc = {'encoding': "UTF-8", 'content': data, 'URL': ''}
		docTopic = getTopicName(f)
		docTopicId = []
		print allTopics[docTopic]
		docTopicId.append(allTopics[docTopic])
		document = {'document':doc, 'topics':docTopicId}
		bask.append(document)
	tsParams=dict([('numberOfDocuments', len(allFiles)), ('basket',bask)])
	cm = ThreadContext.ContextManager(PathFactory())
	CModule.setContextManager(cm)
	CModule.setLoggingFunctions(logging.getLogger)
	f = file('tmp.log','w')
	del f
	logger=logging.getLogger("")
	logger.setLevel(logging.DEBUG)
	handler = logging.handlers.RotatingFileHandler('tmp.log','a',1000000,10)
	formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s')
	handler.setFormatter(formatter)
	logger.addHandler(handler)
	logging.debug('Test run started...')
	logging.debug('Test run finished...')
	pmc = PMC()
	pmc = DefaultPMC()
	#misc = {'parserParams':parserParams,'measureParams':measureParams,'classifierParams':classifierParams,'tsParams':tsParams}
	ctx = cm.getDefaultContext()
	pmc.train(parserParams,measureParams,classifierParams,tsParams,None,None,ctx)
	predictSet = "/home/kostya/Downloads/predict/*.txt"
	allFiles = glob.glob(predictSet)
	ff = file ('resultingStruct.txt', "w+")
	for f in allFiles:
	     	with open (f, "r") as myfile:
    			data=myfile.read().replace('\n', '')
		doc = {'content': data, 'URL': "", "encoding": 'UTF-8',}
		ff.write(f)
		pmc.predict(doc,ctx)
	ff.write("Topics:\n")
	for item in allTopics:
		ff.write(item + ":" + str(allTopics[item]) + "\n")
	