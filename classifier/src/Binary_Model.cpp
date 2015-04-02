#include "Binary_Model.h"
#include <Context.h>
#include <Logger.h>
#include <stdlib.h>

#define LOG_PATH "Classifier.BinaryModel"

Binary_Model::Binary_Model()
{
	state=1;
	number_of_SV=0;
	b=0;
	A=0.0;
	B=0.0;
}

Binary_Model::~Binary_Model() 
{
	delete [] coefficients;
	delete [] labels;
	delete [] numbers;
}

void Binary_Model::memory_allocate1 () 
{
	float *coefficients1;
	int *labels1, *numbers1;

	if (number_of_SV==0)
	{
		labels = new int [1];
		if (!labels) 
		{
			//log4cpp::Category::getInstance("classifier.BinaryModel").fatal("Memory allocation error 2");
			PyLogger::critical("Memory allocation error 2",LOG_PATH);
			exit(1);
		}

		numbers = new int [1];
		if (!numbers) 
		{
			//log4cpp::Category::getInstance("classifier.BinaryModel").fatal("Memory allocation error 3");
			PyLogger::critical("Memory allocation error 3",LOG_PATH);
			exit(1);
		}

		coefficients = new float [1];
		if (!coefficients) 
		{
			//log4cpp::Category::getInstance("classifier.BinaryModel").fatal("Memory allocation error 4");
			PyLogger::critical("Memory allocation error 4",LOG_PATH);
			exit(1);
		}
		coefficients[number_of_SV]=1;
	}
	else
	{
		labels1 = new int [number_of_SV+1];
		if (!labels1) 
		{
			//log4cpp::Category::getInstance("classifier.BinaryModel").fatal("Memory allocation error 6");
			PyLogger::critical("Memory allocation error 6",LOG_PATH);
			exit(1);
		}
		for (int i=0; i<number_of_SV; i++)
		{
			labels1[i]=labels[i];
		}
		delete [] labels;
		labels = labels1;

		numbers1 = new int [number_of_SV+1];
		if (!numbers1) 
		{
			//log4cpp::Category::getInstance("classifier.BinaryModel").fatal("Memory allocation error 7");
			PyLogger::critical("Memory allocation error 7",LOG_PATH);
			exit(1);
		}
		for (int i=0; i<number_of_SV; i++)
		{
			numbers1[i]=numbers[i];
		}
		delete [] numbers;
		numbers = numbers1;

		coefficients1 = new float [number_of_SV+1];
		if (!coefficients1) 
		{
			//log4cpp::Category::getInstance("classifier.BinaryModel").fatal("Memory allocation error 8");
			PyLogger::critical("Memory allocation error 8",LOG_PATH);
			exit(1);
		}
		for (int i=0; i<number_of_SV; i++)
		{
			coefficients1[i]=coefficients[i];
		}
		delete [] coefficients;
		coefficients=coefficients1;
		
		coefficients[number_of_SV]=1;
	}
}

void Binary_Model::delete_zero()
{
	int i=0, j=0;

	if (number_of_SV>0)
	{
		while ((coefficients[i]==0)&&(number_of_SV>0))
		{
			number_of_SV--;
			if (number_of_SV>0) 
			{
				coefficients++; 
				labels++;
				numbers++;
				//support_vectors++;
			}
		}
	}

	if (number_of_SV>0)
	{
		i++;
		while (i<number_of_SV)
		{
			if (coefficients[i]==0)
			{
				for (j=i; j<number_of_SV-1; j++)
				{
					coefficients[j]=coefficients[j+1];
					labels[j]=labels[j+1];
					numbers[j]=numbers[j+1];
					//support_vectors[j]=support_vectors[j+1];
				}
				number_of_SV--;
			}
			else i++;
		}
	}
}

void Binary_Model::read(char *f_model)
{
	FILE *f;
	int intch;
	
	Context context = Context::getThreadContext();
	f=fopen(context(f_model).c_str(),"r");
	
	if (!f)
	{
		//log4cpp::Category::getInstance("classifier.BinaryModel").error("file not found");
		PyLogger::error("File not found",LOG_PATH);
		return;
	}

	intch= getc(f);
	while ((intch!= -1)&&(intch!=' ')&&(intch!='\n')) intch= getc(f);
	fscanf(f,"%d",&state);
	//cout << state <<endl;
	
	intch= getc(f);intch= getc(f);
	while ((intch!= -1)&&(intch!=' ')&&(intch!='\n')) intch= getc(f);
	fscanf(f,"%d",&number_of_SV);
	//cout << number_of_SV <<endl;

	coefficients = new float [number_of_SV];
	labels = new int [number_of_SV];
	numbers = new int [number_of_SV];

	intch= getc(f);	intch= getc(f);
	while ((intch!= -1)&&(intch!=' ')&&(intch!='\n')) intch= getc(f);
	for (int i=0; i<number_of_SV;i++) 
	{
		fscanf(f,"%d",&numbers[i]);
		//cout << numbers[i] <<" ";
	}
	//cout <<endl;

	intch= getc(f);intch= getc(f);intch= getc(f);
	while ((intch!= -1)&&(intch!=' ')&&(intch!='\n')) intch= getc(f);
	for (int i=0; i<number_of_SV;i++) 
	{
		fscanf(f,"%d",&labels[i]);
		//cout << labels[i] <<" ";
	}
	//cout <<endl;
	
	intch= getc(f);intch= getc(f);intch= getc(f);
	while ((intch!= -1)&&(intch!=' ')&&(intch!='\n')) intch= getc(f);
	for (int i=0; i<number_of_SV;i++) 
	{
		fscanf(f,"%f",&coefficients[i]);
		//cout << coefficients[i] <<" ";
	}
	//cout <<endl;
	
	intch= getc(f);intch= getc(f);intch= getc(f);intch= getc(f);
	fscanf(f,"%f",&b);
	//cout << "b_b="<< b <<endl;

	intch= getc(f);intch= getc(f);intch= getc(f);intch= getc(f);
	fscanf(f,"%f",&A);
	//cout << "A="<< A <<endl;

	intch= getc(f);intch= getc(f);intch= getc(f);
	fscanf(f,"%f",&B);
	//cout << "B="<< B <<endl;


	fclose(f);

}

void Binary_Model::save (char *f_mod) 
{
	FILE *f_model;
	
	Context context = Context::getThreadContext();
	f_model=fopen(context(f_mod).c_str(),"w");
	if (!f_model)
	{
		//log4cpp::Category::getInstance("classifier.BinaryModel").error("file not found");
		PyLogger::error("File not found",LOG_PATH);
		return;
	}
	
	fprintf(f_model, "State= ");
	fprintf(f_model, "%d\n",state);

	fprintf(f_model, "Number_Of_Support_Vectors= ");
	fprintf(f_model, "%d\n",number_of_SV);

	fprintf(f_model, "Document_Numbers_in_Support_Vectors_Set\n");

	for (int i=0; i<number_of_SV;i++) 
	{
		fprintf(f_model, "%d ",numbers[i]);
		//cout <<numbers[i]<<" ";
	}
	fprintf(f_model, "\nLabels_of_Support_Vectors\n");
	for (int i=0; i<number_of_SV;i++) 
	{
		fprintf(f_model, "%d ",labels[i]);
	}
	fprintf(f_model, "\nCoefficients_For_Support_Vectors\n");
	for (int i=0; i<number_of_SV;i++) 
	{
		if ((coefficients[i]>0.0001)||(coefficients[i]<-0.0001))
			fprintf(f_model, "%.16g ",coefficients[i]);
		else 
			fprintf(f_model, "0 ");
	}
	fprintf(f_model, "\n");

	if ((b>0.0001)||(b<-0.0001))
		fprintf(f_model, "b= %.16g\n",b);
	else 
		fprintf(f_model, "b= 0\n");

	if ((A>0.0001)||(A<-0.0001))
		fprintf(f_model, "A= %.16g\n",A);
	else 
		fprintf(f_model, "A= 0\n");

	if ((B>0.0001)||(B<-0.0001))
		fprintf(f_model, "B= %.16g\n",B);
	else 
		fprintf(f_model, "B= 0\n");
	fclose(f_model);
}

void Binary_Model::show()
{
	//log4cpp::Category::getInstance("classifier.BinaryModel").infoStream()<< "State =" << state<< log4cpp::CategoryStream::ENDLINE;
	PyLogger::info("",LOG_PATH) << "State =" << state;
	//log4cpp::Category::getInstance("classifier.BinaryModel").infoStream()<< "Number Of Support Vectors =" << number_of_SV << log4cpp::CategoryStream::ENDLINE;
	PyLogger::info("",LOG_PATH) << "Number Of Support Vectors =" << number_of_SV;
	//log4cpp::Category::getInstance("classifier.BinaryModel").infoStream()<< "Support Vectors With Coefficients" << log4cpp::CategoryStream::ENDLINE;
	PyLogger::info("",LOG_PATH) << "Support Vectors With Coefficients";
	for (int i=0; i<number_of_SV;i++) 
	{
		//log4cpp::Category::getInstance("classifier.BinaryModel").infoStream()<<"coefficient= "<<coefficients[i] << log4cpp::CategoryStream::ENDLINE;
		PyLogger::info("",LOG_PATH) <<"coefficient= "<<coefficients[i];
		//log4cpp::Category::getInstance("classifier.BinaryModel").infoStream()<<"labels ="<<labels[i] << log4cpp::CategoryStream::ENDLINE;
		PyLogger::info("",LOG_PATH)<<"labels ="<<labels[i];
		//log4cpp::Category::getInstance("classifier.BinaryModel").infoStream()<<"numbers ="<<numbers[i] << log4cpp::CategoryStream::ENDLINE;
		PyLogger::info("",LOG_PATH)<<"numbers ="<<numbers[i];
	}
	//log4cpp::Category::getInstance("classifier.BinaryModel").infoStream()<<"b= "<<b << log4cpp::CategoryStream::ENDLINE;
	PyLogger::info("",LOG_PATH)<<"b= "<<b;
}

