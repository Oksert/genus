#ifndef MEASURE_H_
#define MEASURE_H_

namespace CModule
{

class Measure
{
public:
	/**
	 * Returns the size of the set, preloaded in the preload method.
	 * 
	 * @return the size of the preloaded set.
	 */
	virtual int getPreloadedSetSize()
	{
		throw std::exception();
	}
	/**
	 * Preloads the set of documents.
	 * 
	 * Each document is an vector<id>. During preload phase each document
	 * is transformed into inner representation, optimized for calculating
	 * similarity between doucuments.
	 * 
	 * The preloaded set is in fact the training set for the classifier.
	 * The preloaded set must be stored (possibly in it's inner
	 * representation) in save() method.
	 * 
	 * @param preloadedSet the vector of documents to be preloaded.
	 */
	virtual void preload(std::vector<std::vector<id> > &preloadedSet)
	{
		throw std::exception();
	}
	/**
	 * Saves all the measure data to the file.
	 * 
	 * The data must include everything to calculate similarity,
	 * after the measure is restored using the load() method.
	 * 
	 * For instance, frequency measure stores tf-idf values, and
	 * all the weighted documents from the preloaded set, so that
	 * both similarity functions can be used after the measure is loaded
	 * from file.
	 * 
	 * @param filename the name of the file, where to put measure data 
	 */
	virtual void save(std::string filename)
	{
		throw std::exception();
	}
	/**
	 * Loads all the measure data from the file.
	 * 
	 * The file must be previously created by the save() method.
	 * 
	 * This method must restore all the data, required to use similarity functions.
	 * 
	 * @param filename the name of the file, where the measure is stored.
	 */
	virtual void load(std::string filename)
	{
		throw std::exception();
	}
	/**
	 * Calculates the similarity between two documents in the preloaded set.
	 * 
	 * This function can be used after preload() has been preformed, or after
	 * measure has been restored, using load() method.
	 * 
	 * @param i1 the index of the first document
	 * @param i2 the index of the second document
	 * @return the similarity of the two documents
	 */
	virtual float similarity(int i1, int i2)
	{
		throw std::exception();
	}
	/**
	 * Calculates the similarity between a document and all the documents
	 * of the preloaded set.
	 * 
	 * This function can be used after preload() has been preformed, or after
	 * measure has been restored, using load() method.
	 * 
	 * @param document the document to be compared with every document of the preloaded set
	 * @param results the array, where to put results of camparison. It's size should be no
	 * less than preloaded set size.
	 */
	virtual void similarity(std::vector<id> &document, float *results)
	{
		throw std::exception();
	}
	virtual ~Measure() {};
};

}

#endif /*MEASURE_H_*/
