#pragma once

namespace DictionaryDB
{
	
	class DictionaryInterface
	{
	public:
		virtual int Write(std::string &str, int num) = 0;
		virtual int GetID(std::string &str) = 0;
		virtual bool Exists(std::string &str) = 0;
		
		virtual ~DictionaryInterface()
		{};
	};
	
//	class DictionaryAdaptor : public DictionaryInterface
//	{
//	private:
//		HashDictionary dict;
//	public:
//		int Write(std::string &str, int num)
//		{
//			std::cout << "Writing" << std::endl;
//			return dict.str2id(str.c_str());
//			std::cout << "Write Successful" << std::endl;
//		};
//		int GetID(std::string &str)
//		{
//			return dict.readid(str.c_str());
//		};
//		bool Exists(std::string &str)
//		{
//			return dict.readid(str.c_str()) != 0;
//		};
//		
//		DictionaryAdaptor(const char *filename)
//		{
//			dict.load(filename);
//		};
//		
//		virtual ~DictionaryAdaptor()
//		{};
//	};

}
