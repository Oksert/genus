#ifndef MEASUREIMPL_H_
#define MEASUREIMPL_H_

class MeasureImpl : public CModule::Measure
{
private:
	Similarity::SimilarityMeasure *measure;
	class VectorDocument : public Similarity::Document
	{
	private:
		std::vector<CModule::id> &vec;
		size_t curRead;
	public:
		virtual size_t read(size_t *values, size_t maxRead)
		{
			size_t index = 0;
			for(; (curRead < vec.size()) && (index < maxRead); curRead++, index++)
			{
				values[index] = vec[curRead];
			}
			return index;
		}
		virtual size_t size() {return vec.size();};
		VectorDocument(std::vector<CModule::id> &v) :vec(v) {curRead = 0;};
	};
public:
	int getPreloadedSetSize()
	{
		return measure->preloadedSetSize();
	}
	void preload(std::vector<std::vector<CModule::id> > &preloadedSet)
	{
		PyLogger::debug("","pyBinding.Measure") << "Preloading frequency measure.";
		
		Similarity::Document **arr = new Similarity::Document*[preloadedSet.size()];
		for(int i = 0; i < preloadedSet.size(); i++)
		{
			arr[i] = new VectorDocument(preloadedSet[i]);
		}
		
		measure->preload(arr,preloadedSet.size());
		
		for(int i = 0; i < preloadedSet.size(); i++)
		{
			delete arr[i];
		}
		
		PyLogger::debug("","pyBinding.Measure") << "Preloading complete.";
		delete arr;
	}
	void save(std::string filename)
	{
		measure->saveToBinaryFile(filename.c_str());
	}
	void load(std::string filename)
	{
		PyLogger::debug("","pyBinding.Measure") << "Loading measure from file";
		measure->readFromBinaryFile(filename.c_str());
		PyLogger::debug("","pyBinding.Measure") << "Loaded measure from file";
	}
	float similarity(int i1, int i2)
	{
		return measure->similarity(i1,i2);
	}
	void similarity(std::vector<CModule::id> &document, float *results)
	{
		VectorDocument doc(document);
		measure->similarity(doc,results);
	}
	MeasureImpl(Similarity::SimilarityMeasure *m)
	{
		Similarity::ValueListAllocator::setFastAllocationMethod();
		measure = m;
	}
	virtual ~MeasureImpl()
	{
		if(measure) delete measure;
		Similarity::ValueListAllocator::destroyAllInstances();
		Similarity::ValueListAllocator::setCommonAllocationMethod();
	};
};

#endif /*MEASUREIMPL_H_*/
