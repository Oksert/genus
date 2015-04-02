#pragma once

namespace DictionaryDB
{
    
    class Dictionary : public DictionaryInterface
	{
	public:
		virtual int Write(std::string & str, int num);//str - lexeme name; num - frequency to add
		virtual int GetID(std::string & str);// str - lexeme name
		int GetFrequency(std::string & str);
		virtual bool Exists(std::string & str);//str - lexeme name
		Dictionary(std::string dbName, int b_d = 0, int b_u = 0);//dbName - database name; b_d - boundary for delete; b_u - boundary for unused
		Dictionary();
		void DeleteLexemes();
		void DeleteDictionary();
		virtual ~Dictionary();
	
		void COUTALL();
	private:
		Db db;
		Dbc *cursor;
		u_int32_t cFlags;
		std::string dbFileName;
		int number_of_elem;
		int boundary_delete;
		int boundary_unused;
	//	Dbt key;
	//	Dbt data;
		bool CheckLink(std::string & str);
		void WriteMaxID();
		void CountMaxID();
		void ReadConfigFile();
		void close();
	};

}
