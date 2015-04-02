#ifndef CLASSIFIER_H_
#define CLASSIFIER_H_

#include <stdlib.h>
//#include <sys/time.h>


namespace CModule
{

class Measure;

class TrainingSetTopicsData
{
public:
	struct DocTopics;
private:
	std::vector<DocTopics> topics;
	int maxTopic;
public:
	struct DocTopics
	{
		int topicsNum;
		int *topics;
		int &operator [] (int index)
		{
			return topics[index];
		}
		DocTopics(const DocTopics &dt)
		{
			topicsNum = dt.topicsNum;
			topics = NULL;
			if(topicsNum > 0)
				topics = new int[topicsNum];
			for(int i = 0; i < topicsNum; i++)
				topics[i] = dt.topics[i];
		}
		DocTopics &operator =(const DocTopics &dt)
		{
			if(topics)
				delete topics;
			topicsNum = dt.topicsNum;
			topics = NULL;
			if(topicsNum > 0)
				topics = new int[topicsNum];
			for(int i = 0; i < topicsNum; i++)
				topics[i] = dt.topics[i];
			return *this;
		}
		DocTopics removeTopic(int topic)
		{
			if(topicsNum == 0)
			{
				return *this;
			}
			DocTopics other;
			bool found = false;
			for(int i = 0; i < topicsNum; i++)
			{
				if(topics[i] == topic)
				{
					found = true;
					break;
				}
			}
			if(found)
			{
				other.topicsNum = topicsNum - 1;
				other.topics = new int[other.topicsNum];
			}
			else
			{
				other.topicsNum = topicsNum;
				other.topics = new int[other.topicsNum];
			}
			int lastTopic = 0;
			for(int i = 0; i < topicsNum; i++)
			{
				if(topics[i] < topic)
				{
					other[lastTopic] = topics[i];
					lastTopic++;
				}
				else if(topics[i] > topic)
				{
					other[lastTopic] = topics[i] - 1;
					lastTopic++;
				}
			}
			return other;
		}
		void print()
		{
			for(int i = 0; i < topicsNum; i++)
			{
				std::cout << " " << topics[i];
			}
			std::cout << std::endl;
		}
		DocTopics(int len = 0)
		{
			topicsNum = len;
			topics = NULL;
			if(len > 0)
				topics = new int[len];
		}
		~DocTopics()
		{
			if(topics)
				delete topics;
		}
	};
	void addDocument(int numberOfTopics)
	{
		topics.push_back(DocTopics(numberOfTopics));
	}
	void setDocumentTopic(int docNumber, int topicIndex, int topicId)
	{
		if(topicId >= maxTopic)
			maxTopic = topicId + 1;
		//PyLogger::debug("","pyBinding.TSTD") << "Adding topic[" << topicIndex << "]=" << topicId << " to the " << docNumber << " document; maxtopic=" << maxTopic;
		topics[docNumber][topicIndex] = topicId;
	}
	int getTopicsNumber()
	{
		return maxTopic;
	}
	int getNumberOfDocuments()
	{
		return topics.size();
	}
	DocTopics &getDocumentTopics(int index)
	{
		return topics[index];
	}
	void saveToFile(std::string filename)
	{
		std::ofstream fout(filename.c_str());
		fout << topics.size() << std::endl;
		for(int i = 0; i < topics.size(); i++)
		{
			fout << topics[i].topicsNum << std::endl;
			for(int k = 0; k < topics[i].topicsNum; k++)
			{
				fout << topics[i][k] << std::endl;
			}
		}
		fout.close();
	}
	void loadFromFile(std::string filename)
	{
		std::ifstream fin(filename.c_str());
		int docsNumber = 0;
		fin >> docsNumber;
		for(int i = 0; i < docsNumber; i++)
		{
			int numTopics = 0;
			fin >> numTopics;
			addDocument(numTopics);
			for(int k = 0; k < numTopics; k++)
			{
				int topicId = 0;
				fin >> topicId;
				setDocumentTopic(i,k,topicId);
			}
		}
	}
	void deleteTopic(int index)
	{
		for(int i = 0; i < topics.size(); i++)
		{
			topics[i] = topics[i].removeTopic(index);
		}
	}
	TrainingSetTopicsData() :maxTopic(-1) {};
	~TrainingSetTopicsData() {};
};


struct ClassifierResults
{
	/**
	 * The i-th element of the weights array is the weight of 
	 * i-th most relevant topic. Thus the weights array is
	 * sorted.
	 */
	std::vector<float> weights;
	/**
	 * The i-th element of the ranks array is the index of the
	 * i-th most relevant topic.
	 */
	std::vector<int> ranks;
	/**
	 * Contains indices of topics, that are relevant for this
	 * document (with weight greater than threshold).
	 */
	std::vector<int> resultingTopics;
};

class Classifier
{
private:
	MLClassification classifier;
public:
	void train(Measure &measure, TrainingSetTopicsData &top)
	{
		topics *arr = new topics[measure.getPreloadedSetSize()];
		for(int i = 0; i < measure.getPreloadedSetSize(); i++)
		{
			arr[i].number = top.getDocumentTopics(i).topicsNum;
			arr[i].theames = top.getDocumentTopics(i).topics;
			//PyLogger::debug("","pyBinding.TSTD") << "arr[" << i << "]: number=" << arr[i].number;
			//PyLogger::debug("","pyBinding.TSTD") << "Themes=";
			//for (int j = 0; j < arr[i].number; j++)
			//{
			//	PyLogger::debug("","pyBinding.TSTD") << arr[i].theames[j] << ",";
			//}
		}
		PyLogger::debug("Starting train.","pyBinding") << " Number of topics=" << top.getTopicsNumber();
		//long startTime = GetTickCount();
//struct timespec startTime; 
//clock_gettime(CLOCK_REALTIME, &startTime); 
//std::printf("%d.%09li\n", startTime.tv_sec, startTime.tv_nsec);
		//PyLogger::debug("startTrainTime") << " startTrainTime=" << startTime;
		classifier.train(top.getTopicsNumber(),arr,&measure);
//struct timespec endTime; 
//clock_gettime(CLOCK_REALTIME, &endTime);
//std::cout<<"TrainTime=" << endTime.tv_sec-startTime.tv_sec<<" "<<endTime.tv_nsec-startTime.tv_nsec<<std::endl;
		//PyLogger::debug("TrainTime") << " TrainTime=" << System.currentTimeMillis()- startTime;
		PyLogger::debug("Train finished.","pyBinding");
		delete []arr;
	}
	
	void incrementive_learning(Measure &measure, TrainingSetTopicsData &top, int newDocsNumber)
	{
		topics *arr = new topics[measure.getPreloadedSetSize()];
		for(int i = 0; i < measure.getPreloadedSetSize(); i++)
		{
			arr[i].number = top.getDocumentTopics(i).topicsNum;
			arr[i].theames = top.getDocumentTopics(i).topics;
		}
		PyLogger::debug("Starting incrementive train.","pyBinding") << " Number of topics=" << top.getTopicsNumber();
//struct timespec startTime; 
//clock_gettime(CLOCK_REALTIME, &startTime); 
		classifier.incrementive_learning(top.getTopicsNumber(),newDocsNumber,arr,&measure);
//struct timespec endTime; 
//clock_gettime(CLOCK_REALTIME, &endTime);
//std::cout<<"ReTrainTime=" << endTime.tv_sec-startTime.tv_sec<<" "<<endTime.tv_nsec-startTime.tv_nsec<<std::endl;
		std::cout<<"Train finished" <<std::endl;
		PyLogger::debug("Train finished.","pyBinding");
		delete []arr;
	}
	
	void delete_topic(Measure &measure, TrainingSetTopicsData &top, int topicNumber)
	{
		topics *arr = new topics[measure.getPreloadedSetSize()];
		for(int i = 0; i < measure.getPreloadedSetSize(); i++)
		{
			arr[i].number = top.getDocumentTopics(i).topicsNum;
			arr[i].theames = top.getDocumentTopics(i).topics;
		}
		PyLogger::debug("Starting deleting topic.","pyBinding") << " Number of topics=" << top.getTopicsNumber();
		classifier.delete_topic(topicNumber,top.getTopicsNumber(),measure.getPreloadedSetSize(),arr,&measure);
		PyLogger::debug("Delete finished.","pyBinding");
		delete []arr;
	}
	
	ClassifierResults *predict(ParsedContent &content, Measure &measure, int topicsNumber)
	{
//struct timespec startTime1; 
//clock_gettime(CLOCK_REALTIME, &startTime1); 
		
		PyLogger::debug("","pyBinding") << "Starting predict.";
		float *simArr = new float[measure.getPreloadedSetSize()];
		std::vector<id> vec = content.getIds();
//struct timespec startTime2; 
//clock_gettime(CLOCK_REALTIME, &startTime2);
		measure.similarity(vec,simArr);
//struct timespec endTime2; 
//clock_gettime(CLOCK_REALTIME, &endTime2);
//std::cout<<"SimilarityMeasureFullTime=" << endTime2.tv_sec-startTime2.tv_sec<<" "<<endTime2.tv_nsec-startTime2.tv_nsec<<std::endl;
		
		int *ranks = new int[topicsNumber];
		float *weights = new float[topicsNumber];
		topics *resultingArray = new topics;
		resultingArray->theames = NULL;
		resultingArray->number = 0;


		PyLogger::debug("","pyBinding") << "Predicting.";	
//struct timespec startTime; 
//clock_gettime(CLOCK_REALTIME, &startTime); 
		classifier.predict(simArr, resultingArray, ranks, weights);
//struct timespec endTime; 
//clock_gettime(CLOCK_REALTIME, &endTime);
//std::cout<<"PredictTime=" << endTime.tv_sec-startTime.tv_sec<<" "<<endTime.tv_nsec-startTime.tv_nsec<<std::endl;
		PyLogger::debug("","pyBinding") << "Predicted successfully.";
		ClassifierResults *ret = new ClassifierResults;
		for(int i = 0; i < topicsNumber; i++)
		{
			ret->weights.push_back(weights[i]);
			ret->ranks.push_back(ranks[i]);
		}
		for(int i = 0; i < resultingArray->number; i++)
		{
			ret->resultingTopics.push_back(resultingArray->theames[i]);
		}
		
		delete ranks;
		delete weights;
		if(resultingArray->theames)
			delete resultingArray->theames;
		delete resultingArray;
		delete simArr;
		
		PyLogger::debug("Results constructed.","pyBinding");
//struct timespec endTime1; 
//clock_gettime(CLOCK_REALTIME, &endTime1);
//std::cout<<"PredictTimeFull (measure + classifier)=" << endTime1.tv_sec-startTime1.tv_sec<<" "<<endTime1.tv_nsec-startTime1.tv_nsec<<std::endl;		
		return ret;
	}
	
	void put_decomposition_type(int decompType) {
		classifier.put_decomposition_type(decompType);
	}
	
	void put_method(int classType) {
		classifier.put_method(classType);
	}
};
}

#endif /*CLASSIFIER_H_*/
