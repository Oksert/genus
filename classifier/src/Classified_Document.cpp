#include "Classified_Document.h"
#include <Context.h>
#include <Logger.h>
#include <stdlib.h>

#define LOG_PATH "Classifier.ClassifiedDocument"

Classified_Document::Classified_Document()
{
	number_of_topics=0;
	topics=NULL;
}

Classified_Document::~Classified_Document()
{
	delete [] topics;
	delete [] lexems;
	//cout<<"~Classified_Document"<<endl;
}

void Classified_Document::cshow()
{
	cout << number_of_topics << " ";
	for (int i=0; i<number_of_topics; i++)
	{
		cout << topics[i] <<" ";
	}
	cout << length << " ";
	for (int i=0; i<length; i++)
	{
		cout << lexems[i] <<" ";
	}
	cout <<endl;
}

/**
 * сохранение документа
 */
void Classified_Document::csave(char *fname)
{
	ofstream fout;
	Context context = Context::getThreadContext();
	fout.open(context(fname).c_str(), ios::out|ios::binary|ios::trunc);
	if (!fout)
	{
		//log4cpp::Category::getInstance("classifier.ClassifiedDocument").error("file not found");
		PyLogger::error("File not found",LOG_PATH);
		return;
	}
	fout.close();

	fout.open(context(fname).c_str(), ios::out|ios::binary|ios::app);
	if (!fout)
	{
		//log4cpp::Category::getInstance("classifier.ClassifiedDocument").error("file not found");
		PyLogger::error("File not found",LOG_PATH);
		return;
	}
	fout << number_of_topics << ' ';
	for (int i=0; i<number_of_topics; i++) fout << topics[i] <<' ';
	fout << length << ' ';
	for (int i=0; i<length; i++) fout << lexems[i] <<' ';
	fout <<endl;

	fout.close();
}

/**
 * чтение документа
 */
void Classified_Document::cread(char *fname)
{
	FILE *f;
	int intch, next;
	char ch;
	
	Context context = Context::getThreadContext();
	f=fopen(context(fname).c_str(),"rb");
	if (!f)
	{
		//log4cpp::Category::getInstance("classifier.ClassifiedDocument").error("file not found");
		return;
	}
	
	// чтение релевантных тем
	intch= getc(f);
	ch= (char) intch;
	number_of_topics=0;
	while ((intch!= -1)&&(intch!=' '))
	{
		number_of_topics=10*number_of_topics+(ch-'0');
		intch= getc(f);
		ch=(char) intch;
	}
	put_number_of_topics(number_of_topics);
	
	topics= new int [number_of_topics];
	if (!topics) 
	{
		//log4cpp::Category::getInstance("classifier.ClassifiedDocument").fatal("Memory allocation error 9");
		PyLogger::critical("Memory allocation error 9",LOG_PATH);
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
	put_topics(topics);

	// чтение лексем
	intch= getc(f);
	ch= (char) intch;
	length=0;
	while ((intch!= -1)&&(intch!=' '))
	{
		length=10*length+(ch-'0');
		intch= getc(f);
		ch=(char) intch;
	}
	put_length(length);
	
	lexems= new int [length];
	if (!lexems) 
	{
		//log4cpp::Category::getInstance("classifier.ClassifiedDocument").fatal("Memory allocation error 10");
		PyLogger::critical("Memory allocation error 10",LOG_PATH);
		exit(1);
	}
	for (int i=0; i<length; i++)
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
		lexems[i]=next;
	}
	put_lexems(lexems);

	fclose(f);
}
