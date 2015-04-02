#include "Decomposition.h"
#include <Logger.h>
#include <stdlib.h>

#define LOG_PATH "Classifier.Decomposition"

Decomposition::Decomposition()
{
}

Decomposition::~Decomposition()
{
}

void Decomposition::decompose()
{
	int num_of_docs;
	int flag1=0, flag0=0;
	int *topics;
	int *labels, *numbers;
	Classified_Document *documents;

	num_of_docs=learning_docs->get_number_of_documents();
	documents=learning_docs->get_documents();

	binary_models = new Binary_Model [number_of_binary_problems];
	if (!binary_models) 
	{
		//log4cpp::Category::getInstance("classifier.Decomposition").fatal("Memory allocation error 12");
		PyLogger::critical("Memory allocation error 12",LOG_PATH);
		exit(1);
	}
	int n=0;

	for (int i=0; i<num_of_docs;i++)
	{
		topics=documents[i].get_topics();
		for (int j=0; j<number_of_binary_problems; j++) 
		{
			flag1=0; flag0=0;
			for (int k=0; k<documents[i].get_number_of_topics();k++)
			{	
				if (decomposition_matrix[j][topics[k]]==1)
				{
					flag1=1;
				}
				else if (decomposition_matrix[j][topics[k]]==-1)
				{
					flag0=1;
				}
			}
			if ((flag1==1))
			{
				n=binary_models[j].get_number_of_SV();
				binary_models[j].memory_allocate1();

				labels=binary_models[j].get_labels();
				numbers=binary_models[j].get_numbers();
					
				labels[n]=+1;
				numbers[n]=i;
			
				binary_models[j].put_labels(labels);
				binary_models[j].put_numbers(numbers);
				binary_models[j].put_number_of_SV(n+1);
				binary_models[j].put_b(0);
				binary_models[j].put_A(0);
				binary_models[j].put_B(0);				
			}
			else if (flag0==1)
			{
				n=binary_models[j].get_number_of_SV();
				binary_models[j].memory_allocate1();

				labels=binary_models[j].get_labels();
				numbers=binary_models[j].get_numbers();
					
				labels[n]=-1;
				numbers[n]=i;

				binary_models[j].put_labels(labels);
				binary_models[j].put_numbers(numbers);
				binary_models[j].put_number_of_SV(n+1);
				binary_models[j].put_b(0);
				binary_models[j].put_A(0);
				binary_models[j].put_B(0);
			}
		}
	}
}

void Decomposition::read_decomposition()
{
	binary_models = new Binary_Model [number_of_binary_problems];
	for (int j=0; j<number_of_binary_problems; j++) 
	{
		char ch,num[10];
		char *f_model;
		
		sprintf(num,"%5.5d",j);
		string s,s1;
		s1.erase();
		for (int i=0; i<5; i++)
		{
			ch=num[i];
			s1=s1+ch;
		}
		s="bmodel"+s1+".txt";
		f_model=(char *) s.c_str();
		binary_models[j].read(f_model);
	}
}

void Decomposition::save_decomposition()
{
	for (int j=0; j<number_of_binary_problems; j++) 
	{
		char ch,num[10];
		char *f_model;
		
		sprintf(num,"%5.5d",j);
		string s,s1;
		s1.erase();
		for (int i=0; i<5; i++)
		{
			ch=num[i];
			s1=s1+ch;
		}
		s="bmodel"+s1+".txt";
		f_model=(char *) s.c_str();
		
		binary_models[j].save(f_model);
	}
}

void Decomposition::show()
{
	learning_docs->show();
	
	//log4cpp::Category::getInstance("classifier.Decomposition").infoStream() << "Number Of Topics =" << number_of_topics << log4cpp::CategoryStream::ENDLINE;
	PyLogger::info("",LOG_PATH) << "Number Of Topics =" << number_of_topics;
	//log4cpp::Category::getInstance("classifier.Decomposition").infoStream() << "Number Of Binary Problems =" << number_of_binary_problems << log4cpp::CategoryStream::ENDLINE;
	PyLogger::info("",LOG_PATH) << "Number Of Binary Problems =" << number_of_binary_problems; 
	//log4cpp::Category::getInstance("classifier.Decomposition").infoStream() << "Decomposition Matrix" <<log4cpp::CategoryStream::ENDLINE;
	PyLogger::info("",LOG_PATH) << "Decomposition Matrix";
	for (int i=0; i<number_of_binary_problems;i++) 
	{
		PyLogger log = PyLogger::info("",LOG_PATH);
		for (int j=0; j<number_of_topics;j++)
			log << decomposition_matrix[i][j] <<" ";
	}
		
	for (int i=0; i<number_of_binary_problems;i++) 
	{
		//log4cpp::Category::getInstance("classifier.Decomposition").infoStream() << "binary model #" <<i<<log4cpp::CategoryStream::ENDLINE;
		PyLogger::info("",LOG_PATH) << "binary model #" <<i;
		binary_models[i].show();
	}
}

void Decomposition::delete_zero()
{
	int m=0, flag=0;
	int number_SVs;
	number_SVs=learning_docs->get_number_of_documents();
	number_of_SV=number_SVs;

	int *Del;
	Del= new int [number_of_SV];
	for (int l=0; l<number_of_SV; l++) Del[l]=1;
	//Del[l]:=0, если опорный вектор с номером l надо удалить из общей модели

	int *Recount;
	Recount= new int [number_of_SV];

	//цикл по опорным векторам в общей модели
	for (int i=0; i<number_of_SV;i++) 
	{
		flag=0;
		// цикл по бинарным моделям
		for (int j=0; j<number_of_binary_problems;j++) 
		{
			//цикл по опорным векторам бинарных моделей
			for (int k=0; k<binary_models[j].get_number_of_SV(); k++)
			{
				if ((binary_models[j].get_numbers()[k]==i)
					&&(binary_models[j].get_coefficients()[k]>0))
				{
					flag=1; break;
				}
			}
			if (flag==1) break;
		}
		if (flag==0) Del[i]=0;
	}

	//формирование массива пересчёта (по массиву Del)
	int count=0;
	for (int l=0; l<number_of_SV; l++) 
	{
		if (Del[l]==0) 
		{
			Recount[l]=-1;
			count++;
		}
		else Recount[l]=l-count;
	}

	//пересчёт всех numbers во всех бинарных моделях 
	//(на основе созданного массива пересчёта)	
	int *numbers;
	// цикл по бинарным моделям
	for (int j=0; j<number_of_binary_problems;j++) 
	{
		numbers=binary_models[j].get_numbers();
		//цикл по опорным векторам бинарных моделей
		for (int k=0; k<binary_models[j].get_number_of_SV(); k++)
		{
			numbers[k]=Recount[numbers[k]];
		}
		binary_models[j].put_numbers(numbers);
	}

	//удаление "ненужных" опорных векторов из общей модели
	if (number_SVs>0)
	{
		while ((Del[m]==0)&&(number_SVs>0))
		{
			number_SVs--;
			//if (number_SVs>0) supp_vectors++;
			m++;
		}
	}
	if (number_SVs>0)
	{
		while (m<number_of_SV-1)
		{
			m++;
			if (Del[m]==0)
			{
				//for (int l=m;l<number_SVs-1; l++)
					//supp_vectors[l]=supp_vectors[l+1];
				number_SVs--;
			}
		}
	}
	
	
	number_of_SV=number_SVs;
	//create_support_vectors_set();

	for (int j=0; j<number_of_binary_problems;j++) 
	{
		//binary_models[j].delete_zero();
	}

	delete [] Del;
	delete [] Recount;
}
