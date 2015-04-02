#ifndef FPTREEMEASUREIMPL_H_
#define FPTREEMEASUREIMPL_H_

//#include "measure.h"

#include <fstream>

#include "fptreealg.h"
#include "fptreealgfactory.h"
#include "tfilereader.h"

#include <algorithm>

using namespace fplib;

RetrainMeasure *measure_load;
FPTreeAlg *alg_load;
int current_context_number1 = 0;


#define END_OF_TRANSACTION 0

class FPTreeMeasureImpl : public RetrainMeasure
{
private:
	typedef unsigned int uint;

	const std::string file_name_for_apriori_in;
	const std::string file_name_for_apriori_out;

	std::vector< std::vector<CModule::id> > episodes;

	int min_supp;
	int max_size;

	RetrainMeasure *measure;
	FPTreeAlg * alg;
	FPTreeAlgFactory * factory;

	/*
	bool TransactionContains(CModule::id elem, std::set<CModule::id> & transaction)
	{
		std::set<CModule::id>::iterator p = transaction.find(elem);
		if  (p != transaction.end()) 
			return true;
		else 
			return false;
	}*/
	
	
	/*bool TransactionContains(CModule::id elem, std::set<CModule::id> & transaction)
	{
			std::set<CModule::id>::iterator p = transaction.begin();

			while(p != transaction.end())
			{
				if(elem == *p) return true;
				p++;
			}

			return false;
	}*/

	/*bool CheckEpisodeInTransaction(std::vector<CModule::id> & episode, std::set<CModule::id> & transaction)
	{
		for(unsigned int i=0; i<episode.size(); i++)
		{
			if(!TransactionContains(episode[i],transaction)) return false;
		}

		return true;		
	}

	void ConvertTransaction(std::vector<CModule::id> & new_document, std::set<CModule::id> & transaction)
	{
		for(unsigned int i=0; i<this->episodes.size(); i++)
		{
			if(CheckEpisodeInTransaction(this->episodes[i],transaction)) new_document.push_back(i);
		}
	}*/
	void ConvertTransaction(std::vector<CModule::id> & new_document, std::set<CModule::id> & transaction)
	{
		Check(new_document,alg->prefixTree(),transaction,transaction.begin());
	}

	void Check(std::vector<CModule::id> & new_document, const PrefixNode * node,
		const std::set<CModule::id> & transaction, std::set<CModule::id>::const_iterator begin)
	{
		for (std::set<CModule::id>::const_iterator it = begin; it != transaction.end(); it++)
		{
			PrefixNode * child = node->getChild(*it);
			if (child == 0 ) continue;
			new_document.push_back(child->id);
			Check(new_document,child,transaction,++it);
			it--;
		}
	}
public:
	FPTreeMeasureImpl(const std::string file_in, const std::string file_out, int min_supp, int max_size)
		:file_name_for_apriori_in(file_in), file_name_for_apriori_out(file_out)
	{
		if(file_name_for_apriori_in == file_name_for_apriori_out)
		{
			cout << "\nError: \"file_name_for_apriori_in\" = \"file_name_for_apriori_out\"\n\n";
		}
		else
		{
			this->min_supp = min_supp;
			this->max_size = max_size;

			alg = new FPTreeAlg();
			factory = new FPTreeAlgFactory();
			alg->minSupport() = min_supp;
			alg->maxItemsetLen() = max_size;
			alg->itemsetsGrowthOnlyByTrainUp() = true;
			alg->noSupportLimitForTree() = true;
		}
	
		//Similarity::ValueListAllocator::setFastAllocationMethod();
		this->measure = new RetrainMeasureImpl(new Similarity::RetrainFrequencyMeasure());
	}

	int getPreloadedSetSize()
	{
		return this->measure->getPreloadedSetSize();
	}

	void preload(std::vector<std::vector<CModule::id> > &preloadedSet)
	{
		std::cout << "FPTree Measure preload" << std::endl;

		ofstream fout(this->file_name_for_apriori_in.c_str());

		if(!fout)
		{
			cout << "\n> Error: create file for apriori (\"" << this->file_name_for_apriori_in << "\")\n\n";
			return;
		}

		//??
		for(unsigned int i=0; i<preloadedSet.size(); i++)
		{
			// For duplicate remove
			std::set<CModule::id> buff;
			buff.clear();

			for(unsigned int j=0; j<preloadedSet[i].size(); j++)
			{
				if(preloadedSet[i][j] == END_OF_TRANSACTION)
				{
					std::set<CModule::id>::iterator p = buff.begin();
					while(p != buff.end())
					{
						fout << *p << ' ';
						p++;
					}
					buff.clear();

					fout << endl;
				}
				else
				{
					buff.insert(preloadedSet[i][j]);
				}
			}
		}

		fout.close();

		//  FPTree
		
		// ifstream f("test.txt");
		ifstream f(file_name_for_apriori_in.c_str());
		IDataReader * reader = new TFileReader(f);
		alg->train(reader);

		ofstream os(file_name_for_apriori_out.c_str(), ios_base::out | ios_base::binary);
		factory->save(alg, os);
		os.close();
		f.close();

std::cout << "file_name_for_apriori_in.c_str() = " << file_name_for_apriori_in.c_str() << std::endl;
std::cout << "file_name_for_apriori_out.c_str() = " << file_name_for_apriori_out.c_str() << std::endl;

		/*//   
		Itemsets Is = alg->itemsets();
std::cout << "alg->totalCases() = " << alg->totalCases() << std::endl;
std::cout << "episodes.size() = " << alg->itemsets().size() << std::endl;
		vector<Itemset*> FPTree_episodes(Is.begin(), Is.end());
		std::sort(FPTree_episodes.begin(), FPTree_episodes.end(), ItemsetIdLess());
		//ofstream fout("fout.txt", ios_base::out);
		//cout << "SIZE = " << FPTree_episodes.size() <<endl;
		//fout << "SIZE = " << FPTree_episodes.size() <<endl;
		this->episodes.clear();
		for(unsigned int i=0; i<FPTree_episodes.size(); i++)
		{
			//cout << "size = " << FPTree_episodes[i]->sortedItems()->size() <<"   ";
			//fout << "size = " << FPTree_episodes[i]->sortedItems()->size() <<"   ";
			vector<unsigned int> episode(FPTree_episodes[i]->sortedItems()->begin(), FPTree_episodes[i]->sortedItems()->end());
			this->episodes.push_back(episode);
			//for(unsigned int j=0; j<FPTree_episodes[i]->sortedItems()->size(); j++)
			//{
				//cout << episode[j] << ' ';
				//fout << episode[j] << ' ';
			//}
			//cout << '\n';
			//fout << '\n';
		}
		//getchar();

std::cout << "FPTree_episodes.size() = " << FPTree_episodes.size() << std::endl;*/
////////////////////////////////////////////////////////////////////////////////////////////////////
// PRINT
//FILE *fout2;
//fout2=fopen(context("set_of_episodes_print.txt").c_str(),"w");
/*
Context context = Context::getThreadContext();	
ofstream fout2(context("set_of_episodes_print.txt").c_str());

		for(unsigned int i=0; i<this->episodes.size(); i++)
		{
			for(unsigned int j=0; j<this->episodes[i].size(); j++)
			{
				fout2 << this->episodes[i][j] << ' ';
			}
			fout2 << '\n';
		}

		fout2.close();
*/
////////////////////////////////////////////////////////////////////////////////////////////////////
// CONVERT

		std::vector<std::vector<CModule::id> > new_preloadedSet;

		for(unsigned int i=0; i<preloadedSet.size(); i++)
		{
			// For duplicate remove
			std::set<CModule::id> buff;
			buff.clear();

			std::vector<CModule::id> new_document;
			new_document.clear();

			for(unsigned int j=0; j<preloadedSet[i].size(); j++)
			{
				if(preloadedSet[i][j] == END_OF_TRANSACTION)
				{
					ConvertTransaction(new_document,buff);
					buff.clear();
				}
				else
				{
					buff.insert(preloadedSet[i][j]);
				}
			}
			
			new_preloadedSet.push_back(new_document);
		}

////////////////////////////////////////////////////////////////////////////////////////////////////
// PRINT		

//FILE *fout3;	
//Context context = Context::getThreadContext();
//fout3=fopen(context("new_docs.txt").c_str(),"w");	
//ofstream fout3("new_docs.txt");
/*
ofstream fout3(context("new_docs.txt").c_str());		


		for(unsigned int i=0; i<new_preloadedSet.size(); i++)
		{
			for(unsigned int j=0; j<new_preloadedSet[i].size(); j++)
			{
				fout3 << new_preloadedSet[i][j] << ' ';
			}
			fout3 << '\n' << '\n';
		}

		fout3.close();
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

		std::cout << "FPTree preload succeeded" << std::endl;
		std::cout << "Starting Toha preload" << std::endl;
		this->measure->preload(new_preloadedSet);
	}

	void retrain(std::vector<std::vector<CModule::id> > &preloadedSet)
	{
		std::cout << "FPTree Measure preload1" << std::endl;

		ofstream fout(this->file_name_for_apriori_in.c_str());

		if(!fout)
		{
			cout << "\n> Error: create file for apriori (\"" << this->file_name_for_apriori_in << "\")\n\n";
			return;
		}

		//??
		for(unsigned int i=0; i<preloadedSet.size(); i++)
		{
			// For duplicate remove
			std::set<CModule::id> buff;
			buff.clear();

			for(unsigned int j=0; j<preloadedSet[i].size(); j++)
			{
				if(preloadedSet[i][j] == END_OF_TRANSACTION)
				{
					std::set<CModule::id>::iterator p = buff.begin();
					while(p != buff.end())
					{
						fout << *p << ' ';
						p++;
					}
					buff.clear();

					fout << endl;
				}
				else
				{
					buff.insert(preloadedSet[i][j]);
				}
			}
		}

		fout.close();

		//  FPTree
		
		ifstream is1(file_name_for_apriori_out.c_str(), ios_base::in |ios_base::binary);
		alg = factory->load(is1);
		is1.close();
		ifstream f1(file_name_for_apriori_in.c_str());
		IDataReader * reader1 = new TFileReader(f1);
		//
		alg->trainUp(reader1);

		ofstream os(file_name_for_apriori_out.c_str(), ios_base::out | ios_base::binary);
		factory->save(alg, os);
		os.close();
		f1.close();

std::cout << "file_name_for_apriori_in.c_str() = " << file_name_for_apriori_in.c_str() << std::endl;
std::cout << "file_name_for_apriori_out.c_str() = " << file_name_for_apriori_out.c_str() << std::endl;

		/*//   
		Itemsets Is = alg->itemsets();
std::cout << "alg->totalCases() = " << alg->totalCases() << std::endl;
std::cout << "episodes.size() = " << alg->itemsets().size() << std::endl;
		vector<Itemset*> FPTree_episodes(Is.begin(), Is.end());
		std::sort(FPTree_episodes.begin(), FPTree_episodes.end(), ItemsetIdLess());
		//ofstream fout("fout.txt", ios_base::out);
		//cout << "SIZE = " << FPTree_episodes.size() <<endl;
		//fout << "SIZE = " << FPTree_episodes.size() <<endl;
		this->episodes.clear();
		for(unsigned int i=0; i<FPTree_episodes.size(); i++)
		{
			//cout << "size = " << FPTree_episodes[i]->sortedItems()->size() <<"   ";
			//fout << "size = " << FPTree_episodes[i]->sortedItems()->size() <<"   ";
			vector<unsigned int> episode(FPTree_episodes[i]->sortedItems()->begin(), FPTree_episodes[i]->sortedItems()->end());
			this->episodes.push_back(episode);
			//for(unsigned int j=0; j<FPTree_episodes[i]->sortedItems()->size(); j++)
			//{
				//cout << episode[j] << ' ';
				//fout << episode[j] << ' ';
			//}
			//cout << '\n';
			//fout << '\n';
		}
std::cout << "FPTree_episodes.size() = " << FPTree_episodes.size() << std::endl;
		//getchar();
*/

////////////////////////////////////////////////////////////////////////////////////////////////////
// PRINT
/*
		ofstream fout2("set_of_episodes_print.txt");

		for(unsigned int i=0; i<this->episodes.size(); i++)
		{
			for(unsigned int j=0; j<this->episodes[i].size(); j++)
			{
				fout2 << this->episodes[i][j] << ' ';
			}
			fout2 << '\n';
		}

		fout2.close();
//*/
////////////////////////////////////////////////////////////////////////////////////////////////////
// CONVERT

		std::vector<std::vector<CModule::id> > new_preloadedSet;

		for(unsigned int i=0; i<preloadedSet.size(); i++)
		{
			// For duplicate remove
			std::set<CModule::id> buff;
			buff.clear();

			std::vector<CModule::id> new_document;
			new_document.clear();

			for(unsigned int j=0; j<preloadedSet[i].size(); j++)
			{
				if(preloadedSet[i][j] == END_OF_TRANSACTION)
				{
					ConvertTransaction(new_document,buff);
					buff.clear();
				}
				else
				{
					buff.insert(preloadedSet[i][j]);
				}
			}
			
			new_preloadedSet.push_back(new_document);
		}

////////////////////////////////////////////////////////////////////////////////////////////////////
// PRINT		
/*
		ofstream fout3("new_docs.txt");

		for(unsigned int i=0; i<new_preloadedSet.size(); i++)
		{
			for(unsigned int j=0; j<new_preloadedSet[i].size(); j++)
			{
				fout3 << new_preloadedSet[i][j] << ' ';
			}
			fout3 << '\n' << '\n';
		}

		fout3.close();
//*/
////////////////////////////////////////////////////////////////////////////////////////////////////

		std::cout << "FPTree preload succeeded2" << std::endl;
		std::cout << "Starting Toha preload3" << std::endl;
		this->measure->retrain(new_preloadedSet);
	}

	void save(std::string filename)
	{
		if(strcmp(file_name_for_apriori_in.c_str(),filename.c_str()) == 0)
		{
			cout << "\nError: save measure (\"filename\" = \"file_name_for_apriori_in\"\n\n";
			return;
		}
		if(strcmp(file_name_for_apriori_out.c_str(),filename.c_str()) == 0)
		{
			cout << "\nError: save measure (\"filename\" = \"file_name_for_apriori_out\"\n\n";
			return;
		}
		
		this->measure->save(filename);
		cout << "\nSave measure ... OK\n";
	}

	void load(std::string filename)
	{
		string s = Context::getThreadContext()(" ");
		int context_number=0;
		sscanf(&s.c_str()[7],"%d",&context_number);
		cout << "current_context_number1 = "<<current_context_number1<<endl;
		cout << "s = " <<s<<"context_number= "<<context_number<<endl;
	
		if (current_context_number1 != context_number)
		{
			//if (alg != 0) delete alg;
			//if (factory != 0) delete factory;
			//if (measure) delete measure;
			current_context_number1 = context_number;
			//ifstream is("model.txt", ios_base::in |ios_base::binary);
			ifstream is(file_name_for_apriori_out.c_str(), ios_base::in |ios_base::binary);
			alg_load = factory->load(is);
			is.close();

			/*//  episodes  
			Itemsets Is = alg->itemsets();
			vector<Itemset*> FPTree_episodes(Is.begin(), Is.end());
			std::sort(FPTree_episodes.begin(), FPTree_episodes.end(), ItemsetIdLess());
			this->episodes.clear();
			for(unsigned int i=0; i<FPTree_episodes.size(); i++)
			{
				vector<unsigned int> episode(FPTree_episodes[i]->sortedItems()->begin(), FPTree_episodes[i]->sortedItems()->end());
				this->episodes.push_back(episode);
			}*/

			//measure_load->load(filename);
			cout << "\nLoad measure ... OK\n";
		}
		alg = alg_load;
		////this->measure = measure_load;
		this->measure->load(filename);
	}

	float similarity(int i1, int i2)
	{
		//std::cout << "similarity";
		return this->measure->similarity(i1,i2);
	}

	void similarity(std::vector<CModule::id> &document, float *results)
	{
		// For duplicate remove
		std::set<CModule::id> buff;
		buff.clear();

		std::vector<CModule::id> new_document;
		new_document.clear();

//struct timespec startTime; 
//clock_gettime(CLOCK_REALTIME, &startTime);
		for(unsigned int j=0; j<document.size(); j++)
		{
			if(document[j] == END_OF_TRANSACTION)
			{
				ConvertTransaction(new_document,buff);
				buff.clear();
			}
			else
			{
				buff.insert(document[j]);
			}
		}
//struct timespec endTime; 
//clock_gettime(CLOCK_REALTIME, &endTime);
//std::cout<<"ConvertTransactionTime=" << endTime.tv_sec-startTime.tv_sec<<" "<<endTime.tv_nsec-startTime.tv_nsec<<std::endl;
//////////////////		
		//std::cout << "new_document:  " << new_document.size() << " ";
		//for(unsigned int j=0; j<new_document.size(); j++)
		//{
		//	std::cout << new_document[j] << ' ';
		//}
///////////////
		document.clear();

		this->measure->similarity(new_document,results);
	}
	
	virtual ~FPTreeMeasureImpl()
	{
		//std::cout <<"~FPTreeMeasureImpl()"<<std::endl;
		if(measure) delete measure;
		//delete alg;
		//delete factory;
	}
};


#endif /*FPTREEMEASUREIMPL_H_*/

