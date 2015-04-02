#include "Document_Set.h"
#include <Context.h>
#include <Logger.h>
#include <stdlib.h>

#define LOG_PATH "Classifier.DocumentSet"

Document_Set::Document_Set()
{
	number_of_documents=0;
}

Document_Set::~Document_Set()
{
	delete [] documents;
	//cout<<"~Document_Set()"<<endl;
}


void Document_Set::show()
{
	//log4cpp::Category::getInstance("classifier.DocumentSet").infoStream() << "number_of_documents= "<< number_of_documents << log4cpp::CategoryStream::ENDLINE;
	PyLogger::info("",LOG_PATH) << "number_of_documents= "<< number_of_documents;
	for (int i=0; i<number_of_documents; i++)
	{
		documents[i].cshow();
	}
}

/**
 * Сохранение документа
 */
void Document_Set::save(char *fname)
{
	ofstream fout;
	
	Context context = Context::getThreadContext();
	fout.open(context(fname).c_str(), ios::out|ios::binary|ios::trunc);
	if (!fout)
	{
		//log4cpp::Category::getInstance("classifier.DocumentSet").error("file not found");
		PyLogger::error("file not found",LOG_PATH);
		return;
	}
	fout.close();

	fout.open(context(fname).c_str(), ios::out|ios::binary|ios::app);
	if (!fout)
	{
		//log4cpp::Category::getInstance("classifier.DocumentSet").error("file not found");
		PyLogger::error("file not found",LOG_PATH);
		return;
	}
	
	fout << number_of_documents << endl;
	for (int j=0; j<number_of_documents; j++)
	{
		fout << documents[j].get_number_of_topics() << ' ';
		for (int i=0; i<documents[j].get_number_of_topics(); i++) 
			fout << documents[j].get_topics()[i] <<' ';
		fout << documents[j].get_length() << ' ';
		for (int i=0; i<documents[j].get_length(); i++) 
			fout << documents[j].get_lexems()[i] <<' ';
		fout <<endl;
	}
	
	fout.close();
}

/** 
 * Чтение документа
 */
void Document_Set::read(char *fname)
{
	FILE *f;
	int intch, next;
	char ch;
	int number_of_topics, length;
	int *lexems;
	int *topics;
	
	Context context = Context::getThreadContext();
	f=fopen(context(fname).c_str(),"rb");
	if (!f)
	{
		//log4cpp::Category::getInstance("classifier.DocumentSet").error("file not found");
		PyLogger::error("file not found",LOG_PATH);
		return;
	}

    //чтение количества документов 
	intch= getc(f);
	ch= (char) intch;
	number_of_documents=0;
	while ((intch!= -1)&&(intch!='\n'))
	{
		number_of_documents=10*number_of_documents+(ch-'0');
		intch= getc(f);
		ch=(char) intch;
	}
	put_number_of_documents(number_of_documents);

	documents=new Classified_Document [number_of_documents];
	if (!documents) 
	{
		//log4cpp::Category::getInstance("classifier.DocumentSet").fatal("Memory allocation error 14");
		PyLogger::critical("Memory allocation error 14",LOG_PATH);
		exit(1);
	}
	
	for (int j=0; j<number_of_documents; j++)
	{	
		//чтение релевантных тем
		intch= getc(f);
		ch= (char) intch;
		number_of_topics=0;
		while ((intch!= -1)&&(intch!=' '))
		{
			number_of_topics=10*number_of_topics+(ch-'0');
			intch= getc(f);
			ch=(char) intch;
		}
		documents[j].put_number_of_topics(number_of_topics);

		topics= new int [number_of_topics];
		if (!topics) 
		{
			//log4cpp::Category::getInstance("classifier.DocumentSet").fatal("Memory allocation error 15");
			PyLogger::critical("Memory allocation error 15",LOG_PATH);
			exit(1);
		}
		for (int i=0; i<number_of_topics; i++)
		{
			intch= getc(f);
			ch= (char) intch;
			next=0;
			while ((intch!= -1)&&(intch!=' '))
			{
				next=10*next+(ch-'0');
				intch= getc(f);
				ch=(char) intch;
			}
			topics[i]=next;
		}
		documents[j].put_topics(topics);
		
		//чтение лексем
		intch= getc(f);
		ch= (char) intch;
		length=0;
		while ((intch!= -1)&&(intch!=' '))
		{
			length=10*length+(ch-'0');
			intch= getc(f);
			ch=(char) intch;
		}
		documents[j].put_length(length);
		
		lexems= new int [length];
		if (!lexems) 
		{
			//log4cpp::Category::getInstance("classifier.DocumentSet").fatal("Memory allocation error 16");
			PyLogger::critical("Memory allocation error 16",LOG_PATH);
			exit(1);
		}
		for (int i=0; i<length; i++)
		{
			intch= getc(f);
			ch= (char) intch;
			next=0;
			while ((intch!= -1)&&(intch!=' ')&&(intch!='\n'))
			{
				next=10*next+(ch-'0');
				intch= getc(f);
				ch=(char) intch;
			}
			lexems[i]=next;
		}
		documents[j].put_lexems(lexems);

		delete [] topics;
		delete [] lexems;

		intch= getc(f);
	}

	fclose(f);

}
