#include "Document.h"
#include <Context.h>
#include <Logger.h>
#include <stdlib.h>

#define LOG_PATH "Classifier.Document"

Document::Document()
{
	length=0;
	lexems=NULL;
}

Document::~Document()
{
	delete [] lexems;
}

void Document::show()
{
	cout << length << " ";
	for (int i=0; i<length; i++)
	{
		cout << lexems[i] <<" ";
	}
	cout <<endl;
}

/**
 * Сохранение документа
 */
void Document::save(char *fname)
{
	ofstream fout;
	Context context = Context::getThreadContext();
	fout.open(context(fname).c_str(), ios::out|ios::binary|ios::trunc);
	if (!fout)
	{
		//log4cpp::Category::getInstance("classifier.Document").error("file not found");
		PyLogger::error("file not found",LOG_PATH);
		return;
	}
	fout.close();

	fout.open(context(fname).c_str(), ios::out|ios::binary|ios::app);
	if (!fout)
	{
		//log4cpp::Category::getInstance("classifier.Document").error("file not found");
		PyLogger::error("file not found",LOG_PATH);
		return;
	}

	fout << length << ' ';
	for (int i=0; i<length; i++) fout << lexems[i] <<' ';
	fout <<endl;

	fout.close();
}

/**
 * Чтение документа
 */
void Document::read(char *fname)
{
	FILE *f;
	int intch, next;
	char ch;
	
	Context context = Context::getThreadContext();
	f=fopen(context(fname).c_str(),"rb");
	if (!f)
	{
		//log4cpp::Category::getInstance("classifier.Document").error("file not found");
		PyLogger::error("file not found",LOG_PATH);
		return;
	}

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
		//log4cpp::Category::getInstance("classifier.Document").fatal("Memory allocation error 13");
		PyLogger::critical("Memory allocation error 13",LOG_PATH);
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


