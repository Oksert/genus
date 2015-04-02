#pragma once

#ifndef PARSERLIBRARY_H_
#define PARSERLIBRARY_H_

namespace HTMLParserLibrary
{
	enum ProcessingType
	{
		PROCESSING_NONE,
		PROCESSING_STEMMING,
		PROCESSING_GRAMM
	};
		
	struct Parameters
	{
		/**
		 * training mode - adding words to the dictionary.
		 */
		bool train;
		/**
		 * post processing - NONE, STEMMING, N-GRAMM
		 */
		ProcessingType procType;
		/**
		 * gramm length
		 */
		int grammNumber;
		/**
		 * convert encodings?
		 */
		bool conv;
		/**
		 * input encoding
		 */
		const char *enc_name;
		/**
		 * Extract punctuation marks?
		 */
		bool extractPunct;
		 
		/**
		 * Process multiple chunks of text in one processing batch?
		 * Chunks must be delimetered by the 0x26 ASCII symbol. Any such symbols in text will result in a segmentation fault.
		 * When this flag is set to false, there are no limitations on using 0x26 symbol in texts.
		 */
		bool multiBlockPackage;
		
		Parameters()
		{
			train = false;
			procType = PROCESSING_NONE;
			grammNumber = 3;
			conv = false;
			enc_name = "UTF-8";
			extractPunct = false;
			multiBlockPackage = false;
		};
	};
	
	void Parse(char *text, HTMLParserLibrary::Parameters *param, OutputGenerator *gen);
		
	class BufferByteStream : public ByteStream
	{
	private:
		char *buffer;
	public:
		BufferByteStream(char *buf)
		{
			buffer = buf;
		};
		virtual int get()
		{
			if(*buffer == 0)
				return -1;
			else
				return *buffer++;
		};
		~BufferByteStream()
		{};
	};
}


#endif /*PARSERLIBRARY_H_*/
