/*******************************************************************************

    Author:      Pavlov Anton
    Created:     2008/02/17

*******************************************************************************/

#include "RetrainFrequencyMeasure.h"
#include "BinarySerializer.h"
#include "BinaryDeserializer.h"

using namespace Similarity;

void RetrainFrequencyMeasure::clean()
{
	std::cout << "RetrainFrequencyMeasure::clean()1" << std::endl;
	if(rawDataLoaded)
	{
		delete nonadjustedDocuments;
		delete frequencies;
	}
	if(adjustedDataLoaded)
	{
		delete adjustedDocuments;
		delete idf;
	}
	nonadjustedDocuments = NULL;
	frequencies = NULL;
	adjustedDocuments = NULL;
	idf = NULL;
	rawDataLoaded = false;
	adjustedDataLoaded = false;
	pathString = "";
	std::cout << "RetrainFrequencyMeasure::clean()2" << std::endl;
};

void RetrainFrequencyMeasure::loadRawData()
{
std::cout << "loadRawData()1" << std::endl;
	if(rawDataLoaded) return;
std::cout << "loadRawData()2" << std::endl;

	std::string fullPath(pathString);
std::cout << "fullPath = " << fullPath << std::endl;
std::cout << "loadRawData()3" << std::endl;
	{
		std::string docsPath = fullPath + ".docs.raw";
		std::cout << "docsPath = " << docsPath << std::endl;
		std::cout << "loadRawData()4" << std::endl;
		BinaryDeserializer deserDocs(docsPath.c_str());
		std::cout << "loadRawData()5" << std::endl;
		nonadjustedDocuments = new SparseDocumentCollection(deserDocs);
		std::cout << "loadRawData()6" << std::endl;
	}
std::cout << "loadRawData()7" << std::endl;
	{
		std::string freqPath = fullPath + ".freq.raw";
		BinaryDeserializer deserFreq(freqPath.c_str());
		frequencies = new DFStatistics(deserFreq);
	}
std::cout << "loadRawData()8" << std::endl;
	rawDataLoaded = true;
}

void RetrainFrequencyMeasure::saveRawData()
{
std::cout << "saveRawData()1" << std::endl;
	std::string fullPath(pathString);

	{
		std::string docsPath = fullPath + ".docs.raw";
		BinarySerializer serDocs(docsPath.c_str());
		nonadjustedDocuments->serialize(serDocs);
	}
std::cout << "saveRawData()2" << std::endl;
	{
		std::string freqPath = fullPath + ".freq.raw";
		BinarySerializer serFreq(freqPath.c_str());
		frequencies->serialize(serFreq);
	}
std::cout << "saveRawData()3" << std::endl;
}

void RetrainFrequencyMeasure::loadAdjustedData()
{
	if(adjustedDataLoaded) return;

	std::string fullPath(pathString);

	{
		std::string docsPath = fullPath + ".docs.adj";
		BinaryDeserializer deserDocs(docsPath.c_str());
		adjustedDocuments = new SparseDocumentCollection(deserDocs);
	}

	{
		std::string freqPath = fullPath + ".freq.adj";
		BinaryDeserializer deserFreq(freqPath.c_str());
		idf = new IDFStatistics(deserFreq);
	}

	adjustedDataLoaded = true;
}

void RetrainFrequencyMeasure::saveAdjustedData()
{
	std::string fullPath(pathString);

	{
		std::string docsPath = fullPath + ".docs.adj";
		BinarySerializer serDocs(docsPath.c_str());
		adjustedDocuments->serialize(serDocs);
	}

	{
		std::string freqPath = fullPath + ".freq.adj";
		BinarySerializer serFreq(freqPath.c_str());
		idf->serialize(serFreq);
	}
}

void RetrainFrequencyMeasure::retrain(Document **docs, size_t number)
{
	std::cout << "Loading all previous data1" << std::endl;
	loadRawData();
	std::cout << "Loading all previous data2" << std::endl;
	loadAdjustedData();
	std::cout << "create a new set of documents." << std::endl;
	SparseDocumentCollection newSet;
	for(size_t i = 0; i < number; i++)
	{
		newSet.addDocument(new SparseDocument(*docs[i]));
	}
	std::cout << "get the new max index." << std::endl;
	size_t newMaxIndex = 
		(newSet.getMaxIndex() > nonadjustedDocuments->getMaxIndex())?
		(newSet.getMaxIndex()):(nonadjustedDocuments->getMaxIndex());
	std::cout << "merge the document sets." << std::endl;
	nonadjustedDocuments->mergeIn(newSet);
	std::cout << "merge the statistics." << std::endl;
	DFStatistics *newFreq = new DFStatistics(*nonadjustedDocuments);
	//newFreq->addStatistics(*frequencies);
	std::cout << "delete old statisti cs." << std::endl;
	delete idf;
	delete frequencies;
	std::cout << "set new statistics." << std::endl;
	frequencies = newFreq;
	idf = new IDFStatistics(*frequencies);
	std::cout << "delete all old adjusted documents" << std::endl;
	delete adjustedDocuments;
	std::cout << "create a new set of adjusted documents." << std::endl;
	adjustedDocuments = new SparseDocumentCollection();
	std::cout << "fill the new adjusted document collection." << std::endl;
	for(size_t i = 0; i < nonadjustedDocuments->size(); i++)
	{
		SparseDocument *document = new SparseDocument((*nonadjustedDocuments)[i]);
		document->adjustBy(*idf);
		document->normalize();
		adjustedDocuments->addDocument(document);
	}
};

real RetrainFrequencyMeasure::similarity(Document &doc1, Document &doc2)
{
	loadAdjustedData();
	SparseDocument sdoc1(doc1);
	SparseDocument sdoc2(doc2);
	sdoc1.adjustBy(*idf);
	sdoc2.adjustBy(*idf);
	sdoc1.normalize();
	sdoc2.normalize();
	return sdoc1.dot(sdoc2);
};
void RetrainFrequencyMeasure::similarity(Document &doc, real *dest)
{
	loadAdjustedData();
	SparseDocument sdoc(doc);
	sdoc.adjustBy(*idf);
	sdoc.normalize();
	for(size_t i = 0; i < adjustedDocuments->size(); i++)
	{
		dest[i] = (*adjustedDocuments)[i].dot(sdoc);
	}
};
real RetrainFrequencyMeasure::similarity(size_t i1, size_t i2)
{
	loadAdjustedData();
	return (*adjustedDocuments)[i1].dot((*adjustedDocuments)[i2]);
};
void RetrainFrequencyMeasure::similarity(size_t i, real *dest)
{
	loadAdjustedData();
	SparseDocument &sdoc = (*adjustedDocuments)[i];
	for(size_t i = 0; i < adjustedDocuments->size(); i++)
	{
		dest[i] = (*adjustedDocuments)[i].dot(sdoc);
	}
};
size_t RetrainFrequencyMeasure::preloadedSetSize()
{
	loadAdjustedData();
	return adjustedDocuments->size();
};
void RetrainFrequencyMeasure::preload(Document **docs, size_t number)
{
	clean();
	nonadjustedDocuments = new SparseDocumentCollection();
	for(size_t i = 0; i < number; i++)
	{
		nonadjustedDocuments->addDocument(new SparseDocument(*docs[i]));
	}
	frequencies = new DFStatistics(*nonadjustedDocuments);
	idf = new IDFStatistics(*frequencies);
	adjustedDocuments = new SparseDocumentCollection();
	// fill the new adjusted document collection.
	for(size_t i = 0; i < nonadjustedDocuments->size(); i++)
	{
		SparseDocument *document = new SparseDocument((*nonadjustedDocuments)[i]);
		document->adjustBy(*idf);
		document->normalize();
		adjustedDocuments->addDocument(document);
	}

	rawDataLoaded = true;
	adjustedDataLoaded = true;
};
void RetrainFrequencyMeasure::saveToBinaryFile(const char *preloadedSetFilename)
{
	std::cout << "RetrainFrequencyMeasure::saveToBinaryFile1" << std::endl;
	loadRawData();
	loadAdjustedData();
	pathString = preloadedSetFilename;
	saveRawData();
	saveAdjustedData();
	std::cout << "RetrainFrequencyMeasure::saveToBinaryFile2" << std::endl;
};
void RetrainFrequencyMeasure::readFromBinaryFile(const char *preloadedSetFilename)
{
	pathString = preloadedSetFilename;
	loadAdjustedData();
};
RetrainFrequencyMeasure::RetrainFrequencyMeasure()
{
	nonadjustedDocuments = NULL;
	frequencies = NULL;
	adjustedDocuments = NULL;
	idf = NULL;
	rawDataLoaded = false;
	adjustedDataLoaded = false;
	pathString = "";
}
RetrainFrequencyMeasure::~RetrainFrequencyMeasure()
{
		clean();
}
