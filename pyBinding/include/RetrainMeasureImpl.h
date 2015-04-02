#ifndef RETRAINMEASUREIMPL_H_
#define RETRAINMEASUREIMPL_H_

#include <stdlib.h>
//#include <sys/time.h>


class RetrainMeasureImpl : public RetrainMeasure
{
private:
	Similarity::RetrainFrequencyMeasure *measure;
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
		
//struct timespec startTime; 
//clock_gettime(CLOCK_REALTIME, &startTime);
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
//struct timespec endTime; 
//clock_gettime(CLOCK_REALTIME, &endTime);
//std::cout<<"MeasurePreloadTime=" << endTime.tv_sec-startTime.tv_sec<<" "<<endTime.tv_nsec-startTime.tv_nsec<<std::endl;
		
		PyLogger::debug("","pyBinding.Measure") << "Preloading complete.";
		delete arr;
	}
	void retrain(std::vector<std::vector<CModule::id> > &preloadedSet)
	{
		PyLogger::debug("","pyBinding.Measure") << "Retraining frequency measure.";
//struct timespec startTime; 
//clock_gettime(CLOCK_REALTIME, &startTime);	
		Similarity::Document **arr = new Similarity::Document*[preloadedSet.size()];
		for(int i = 0; i < preloadedSet.size(); i++)
		{
			arr[i] = new VectorDocument(preloadedSet[i]);
		}
		PyLogger::debug("","pyBinding.Measure") << "Running measure->retrain.";
		measure->retrain(arr,preloadedSet.size());
		
		for(int i = 0; i < preloadedSet.size(); i++)
		{
			delete arr[i];
		}
//struct timespec endTime; 
//clock_gettime(CLOCK_REALTIME, &endTime);
//std::cout<<"MeasureReTrainTime=" << endTime.tv_sec-startTime.tv_sec<<" "<<endTime.tv_nsec-startTime.tv_nsec<<std::endl;
		
		PyLogger::debug("","pyBinding.Measure") << "Retrain complete.";
		delete arr;
	}
	void save(std::string filename)
	{
//struct timespec startTime; 
//clock_gettime(CLOCK_REALTIME, &startTime);
		measure->saveToBinaryFile(filename.c_str());
//struct timespec endTime; 
//clock_gettime(CLOCK_REALTIME, &endTime);
//std::cout<<"MeasureSaveTime=" << endTime.tv_sec-startTime.tv_sec<<" "<<endTime.tv_nsec-startTime.tv_nsec<<std::endl;
	}
	void load(std::string filename)
	{
std::cout<<"MeasureLoadTime=" << endl;
//struct timespec startTime; 
//clock_gettime(CLOCK_REALTIME, &startTime);	
		PyLogger::debug("","pyBinding.Measure") << "Loading measure from file";
		measure->readFromBinaryFile(filename.c_str());
		PyLogger::debug("","pyBinding.Measure") << "Loaded measure from file";
//struct timespec endTime; 
//clock_gettime(CLOCK_REALTIME, &endTime);
//std::cout<<"MeasureLoadTime=" << endTime.tv_sec-startTime.tv_sec<<" "<<endTime.tv_nsec-startTime.tv_nsec<<std::endl;
	}
	float similarity(int i1, int i2)
	{
//cout <<"MeasureSimilarityTime=0" <<endl; 
//struct timespec startTime; 
//clock_gettime(CLOCK_REALTIME, &startTime);	
		return measure->similarity(i1,i2);
//struct timespec endTime; 
//clock_gettime(CLOCK_REALTIME, &endTime);
//std::cout<<"MeasureSimilarityTime=" << endTime.tv_sec-startTime.tv_sec<<" "<<endTime.tv_nsec-startTime.tv_nsec<<std::endl;
	}
	void similarity(std::vector<CModule::id> &document, float *results)
	{
		VectorDocument doc(document);
//struct timespec startTime; 
//clock_gettime(CLOCK_REALTIME, &startTime); 
		measure->similarity(doc,results);
//struct timespec endTime; 
//clock_gettime(CLOCK_REALTIME, &endTime);
//std::cout<<"MeasureSimilarityTime=" << endTime.tv_sec-startTime.tv_sec<<" "<<endTime.tv_nsec-startTime.tv_nsec<<std::endl;
	}
	RetrainMeasureImpl(Similarity::RetrainFrequencyMeasure *m)
	{
		Similarity::Element::setAllocator(Similarity::BlockAllocator<Similarity::Element>::getSingleton());
		measure = m;
	}
	virtual ~RetrainMeasureImpl()
	{
				std::cout <<"~REtrainMeasureImpl()"<<std::endl;

		if(measure) delete measure;
	};
};

#endif /*MEASUREIMPL_H_*/
