#include "ParserStartup.h"
#include "DictionaryIncludes.h"
#include <cstdlib>

using namespace DictionaryDB;

using namespace std;

const string _MAX_ID = "_MAX_ID";


Dictionary::Dictionary() : db(NULL, 0) {

	u_int32_t oFlags = DB_CREATE;
	number_of_elem = 0;
	boundary_delete = 0;
	boundary_unused = 0;

	ReadConfigFile();
	try { 
		db.open(NULL, dbFileName.c_str(), NULL, DB_HASH, oFlags, 0);
		CountMaxID();
	} catch(DbException &e) {
		exit(-1);
		// Error handling code goes here
	} catch(std::exception &e) {
		exit(-1);
		// Error handling code goes here
	}
}

//creating/opening database
Dictionary::Dictionary (string dbName, int b_d, int b_u) : db(NULL, 0) {

		u_int32_t oFlags = DB_CREATE;
		number_of_elem = 0;
		boundary_delete = b_d;
		boundary_unused = b_u;
		dbFileName = dbName;
		try { 
			db.open(NULL, dbFileName.c_str(), NULL, DB_HASH, oFlags, 0);
			CountMaxID();
		} catch(DbException &e) {
			exit(-1);
			// Error handling code goes here
		} catch(std::exception &e) {
			exit(-1);
			// Error handling code goes here
		}

	}

void Dictionary::CountMaxID() {
	Dbt key((void *)_MAX_ID.c_str(),_MAX_ID.length()+1);
	Dbt data;
	int check = db.get(NULL, &key, &data, 0);
	if (check == 0) {
		Data_Element *temp = (Data_Element *)data.get_data();
		number_of_elem = temp->freq;
	}
}

/*
void Dictionary::CountMaxID() {
	db.cursor(NULL, &cursor, 0);
	int ret;
	int curs;
	Dbt key, data;
	while ((cursor->get(&key, &data, DB_NEXT) == 0)) {
		if ((curs = ((Data_Element *)data.get_data())->id) > number_of_elem)
			number_of_elem = curs;
	}
	if (cursor != NULL)
		cursor->close();
	cout << "NUMBER OF ELEMENTS: " << number_of_elem << endl << endl;
}
*/

void Dictionary::ReadConfigFile() {
	ifstream fin("dictionary.conf");
	if (fin) {
		fin >> dbFileName;
		fin >> boundary_delete;
		fin >> boundary_unused;
		fin.close();
	}
	else {
		exit(0);
	}
}


//our own closing database function used
Dictionary::~Dictionary() {close();}


// Close the db
void Dictionary::close() {
	WriteMaxID();
	try
	{
		db.close(0);
	}
	catch(DbException &e)
	{
	}
	catch(exception &e)
	{
	}
}


//in train phase
//if lexeme already exists, adding frequency; if not - creating record in database
//----------------returns ID of written element; -1 if error occured
int Dictionary::Write(string & str, int num) {

	Dbt key((void *)str.c_str(),str.length()+1);
	Dbt data;
	int check = db.get(NULL, &key, &data, 0);
	
	if (check == 0) {
		Data_Element *data_temp = (Data_Element *)data.get_data();
		data_temp->freq += num;
		data.set_data(data_temp);
		
		db.put(NULL, &key, &data, 0);
		
		return data_temp->id;
	}
	Data_Element elem(number_of_elem+1, num);
	if (CheckLink(str)) elem.link = true; // if link lexeme
	Dbt data2(&elem, sizeof(elem)+1);
	int u;
	try
	{
		db.del(NULL,&key,0);
		u = db.put(NULL, &key, &data2, 0);
	}
	catch(DbException *e)
	{
		std::cerr << e->what() << std::endl;
		throw;
	}
	catch(DbException &e)
	{
		std::cerr << e.what() << std::endl;
		throw;
	}
	if (u != 0) {
		db.err(u,"Failed to write new element!");
		//std::cerr << "Error here" << std::endl;
		return -1;
	}
	number_of_elem++;
	
	WriteMaxID();
	
	return number_of_elem;
}

//checking if specified lexeme str exists in database
//----------------returns true if lexeme exists; false otherwise
bool Dictionary::Exists(string & str) {
	Dbt key((void *)str.c_str(),str.length()+1);
	Dbt data;
	int check = db.get(NULL, &key, &data, 0);
	Data_Element *temp = (Data_Element *)data.get_data();
	if (check == 0) {
		if ((temp->link) || (temp->freq > boundary_unused)) return true;
	}
	return false;
}


//test phase
//getting ID of specified lexeme str
//----------------returns ID of written lexeme; -1 if error occured
int Dictionary::GetID(string & str) {
	Dbt key((void *)str.c_str(),str.length()+1);
	Dbt data;
	int check = db.get(NULL, &key, &data, 0);
	if (check != 0) {
//		cout << "Getting ID failed: element not found!" << endl;
		return -1;
	}
	return ((Data_Element *)data.get_data())->id;
}

int Dictionary::GetFrequency(string & str) {
	Dbt key((void *)str.c_str(),str.length()+1);
	Dbt data;
	int check = db.get(NULL, &key, &data, 0);
	if (check != 0) {
		//		cout << "Getting ID failed: element not found!" << endl;
		return -1;
	}
	return ((Data_Element *)data.get_data())->freq;
}


void Dictionary::DeleteLexemes() {

	db.cursor(NULL, &cursor, 0);

	int ret;
	Dbt key, data;
	Data_Element *temp;
	while (ret = cursor->get(&key, &data, DB_NEXT) == 0) {
		temp = (Data_Element *)data.get_data();
		if ((!temp->link) && (temp->freq <= boundary_delete))
			cursor->del(0);
	}
	if (cursor != NULL)
		cursor->close();
}



void Dictionary::DeleteDictionary() {
	db.truncate(NULL, 0, 0);
}

bool Dictionary::CheckLink(string & str) {
	if (str[0] == '.') return true;
	return false;
}


void Dictionary::WriteMaxID() {
	Dbt key((void *)_MAX_ID.c_str(),_MAX_ID.length()+1);
	Dbt data;
	int check = db.get(NULL, &key, &data, 0);
	if (check == 0) {
		Data_Element *data_temp = (Data_Element *)data.get_data();
		data_temp->freq  = number_of_elem;
		data.set_data(data_temp);
		db.put(NULL, &key, &data, 0);
	}
	else {
		Data_Element elem(number_of_elem,number_of_elem+1);
		Dbt data2(&elem, sizeof(elem)+1);
		int u = db.put(NULL, &key, &data2, 0);
		if (u != 0) {
			db.err(u,"Failed to write new element!");
			return;
		}
		//number_of_elem++;
	}
}


void Dictionary::COUTALL() {
	db.cursor(NULL, &cursor, 0);

	int ret;
	Dbt key, data;
	Data_Element *temp;
	while (ret = cursor->get(&key, &data, DB_NEXT) == 0) {
		Data_Element *elem = (Data_Element *)data.get_data();
		cout << (char *)key.get_data() << endl;
	}
	if (cursor != NULL)
		cursor->close();
	db.cursor(NULL, &cursor, 0);
}
