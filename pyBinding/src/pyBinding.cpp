
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>


#include <vector>
#include <iostream>
#include <ml_classification.h>
#include <exception>
#include "ParsedContent.h"
#include "Parser.h"
#include "Measure.h"
#include "RetrainMeasure.h"
#include "Classifier.h"
//#include <boost/python.hpp>
//#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

//using namespace boost::python;

#include <ParserLib.h>
#include "ParsedContentImpl.h"
#include "ParserImpl.h"
#include "PackageParser.h"
#include <LibFrequencyMeasure.h>
#include <SimilarityMeasure.h>
#include <BlockAllocator.h>
#include <FrequencyMeasure.h>
#include <RetrainFrequencyMeasure.h>
#include <ValueListAllocator.h>
#include <KSpectrum.h>
#include "MeasureImpl.h"
#include "RetrainMeasureImpl.h"
//#include <AprMeasure.h>
#include <FPTreeMeasureImpl.h>
#include "Context.h"

using namespace boost::python;

#include "Factory.h"
#include "ContextHelper.h"
#include "LoggerHelper.h"

#include "TestRunner.h"

using namespace CModule;


boost::python::list handleLinks(object obj) { 
	std::vector<std::string> vect = extract<std::vector<std::string> > (obj); 
	boost::python::list l;
	std::vector<std::string>::iterator it;
	for (it = vect.begin(); it != vect.end(); it++) {
		boost::python::str elem = boost::python::str(*it);
    	l.append(elem);
	}

	return l;
}

std::vector<std::string> listToSTLVector(object obj, object len) { 
	boost::python::list ids = extract<boost::python::list> (obj); 
	int length = extract<int> (len);
	std::vector<std::string> vect;
	for (int i=0;i<length;i++) {
		std::string elem = extract<std::string> (ids.pop(0));
		vect.push_back(elem);
	}
	
	return vect;
}

BOOST_PYTHON_MODULE(CModule)
{
	def("handleLinks",handleLinks);	
	def("listToSTLVector",listToSTLVector);
	class_<std::vector<id> >("vectorId")
		.def(vector_indexing_suite<std::vector<id> >())
		;
	class_<std::vector<int> >("vectorInt")
		.def(vector_indexing_suite<std::vector<int> >())
		;
	class_<std::vector<float> >("vectorFloat")
		.def(vector_indexing_suite<std::vector<float> >())
		;
	class_<std::vector<std::string> >("vectorString")
		.def(vector_indexing_suite<std::vector<std::string> >())
		;
	class_<std::vector<std::vector<id> > >("vectorVectorId")
		.def(vector_indexing_suite<std::vector<std::vector<id> > >())
		.def("append",&std::vector<std::vector<id> >::push_back)
		;
	class_<TrainingSetTopicsData> ("TopicsData")
		.def("addDocument",&TrainingSetTopicsData::addDocument)
		.def("setDocumentTopic",&TrainingSetTopicsData::setDocumentTopic)
		.def("getTopicsNumber",&TrainingSetTopicsData::getTopicsNumber)
		.def("getNumberOfDocuments",&TrainingSetTopicsData::getNumberOfDocuments)
		.def("loadFromFile",&TrainingSetTopicsData::loadFromFile)
		.def("saveToFile",&TrainingSetTopicsData::saveToFile)
		.def("deleteTopic",&TrainingSetTopicsData::deleteTopic)
		;
	class_<ClassifierResults> ("ClassifierResults")
		.def_readonly("weights",&ClassifierResults::weights)
		.def_readonly("ranks",&ClassifierResults::ranks)
		.def_readonly("resultingTopics",&ClassifierResults::resultingTopics)
		;
	class_<Measure> ("Measure",no_init)
		.def("getPreloadedSetSize",&Measure::getPreloadedSetSize)
		.def("preload",&Measure::preload)
		.def("save",&Measure::save)
		.def("load",&Measure::load)
		;
	class_<RetrainMeasure,bases<Measure> > ("RetrainMeasure",no_init)
		.def("retrain",&RetrainMeasure::retrain)
		;
	class_<Parser> ("Parser",no_init)
		.def("parse",&Parser::parse,return_value_policy<manage_new_object>())
		.def("getId",&Parser::getId)
		.def("getTopicId",&Parser::getTopicId)
		.def("addTopic",&Parser::addTopic)
		;
	
	class_<PackageParser> ("PackageParser",no_init)
		.def("getResult",&PackageParser::getResult,return_value_policy<manage_new_object>())
		.def("parse",&PackageParser::parse)
		.def("addChunk",&PackageParser::addChunk)
		;
	
	class_<Classifier> ("Classifier")
		.def("train",&Classifier::train)
		.def("predict",&Classifier::predict,return_value_policy<manage_new_object>())
		.def("incrementive_learning",&Classifier::incrementive_learning)
		;
	class_<ParsedContent>("ParsedContent",no_init)
		.def("getLinks",&ParsedContent::getLinks)
		.def("replaceLink",&ParsedContent::replaceLink)
		.def("getIds",&ParsedContent::getIds)
		.def("getWordsIds",&ParsedContent::getWordsIds)
		.def("getBase",&ParsedContent::getBaseUrl)
		.def("writeIds",&ParsedContent::writeIds)
		;
	// misc
	def("setContextManager",&StaticObjectHolder::setContextManager);
	def("setLoggingFunctions",&LoggerHelper::setLoggers);
	// parser
	def("createTrainParser",&createTrainParser,return_value_policy<manage_new_object>());
	def("createPredictParser",&createPredictParser,return_value_policy<manage_new_object>());
	// package parser
	def("createTrainPackageParser",&createTrainPackageParser,return_value_policy<manage_new_object>());
	def("createPredictPackageParser",&createPredictPackageParser,return_value_policy<manage_new_object>());
	// measure
	def("createMeasure",&createMeasure,return_value_policy<manage_new_object>());
	def("createRetrainMeasure",&createRetrainMeasure,return_value_policy<manage_new_object>());
	// classifier
	def("createClassifier",&createClassifier,return_value_policy<manage_new_object>());
	// tests
	def("runTests",&runTests,return_value_policy<return_by_value>());
}
