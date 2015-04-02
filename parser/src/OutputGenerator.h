#pragma once

namespace HTMLParserLibrary
{
	struct OutputGenerator
	{
		/**
		 * The current number of pure (not link) identifiers.
		 */
		size_t curIndex;
		/**
		 * The total number of identifiers currently stored here.
		 */
		size_t curSize;
		/**
		 * The base url of the document.
		 * Used to collect links data from Genus Core.
		 */
		char *baseUrl;
		/**
		 * List of buffer where to store generated values.
		 */
		struct BufferList
		{
			static const size_t bufferSize = 16;
			/**
			 * The buffer of ids itself.
			 */
			size_t buffer[bufferSize];
			/**
			 * The number of ids used in this buffer.
			 */
			size_t usedNumber;
			BufferList *nextBuffer;
			void DeleteRecursively()
			{
				if(nextBuffer != NULL)
					nextBuffer->DeleteRecursively();
				delete nextBuffer;
				nextBuffer = NULL;
			}
			BufferList()
			{
				usedNumber = 0;
				nextBuffer = NULL;
			}
		};
		/**
		 * List of special buffers, which collect information
		 * about links data.
		 */
		struct LinkInfoList
		{
			/**
			 * The place in the sequence of pure ids, where to put
			 * this subsequence.
			 */
			size_t position;
			/**
			 * The id of subsequence.
			 * Subsequences with the same position differ by their
			 * number.
			 * 
			 * This value is probably redundant, because of the
			 * list structure.
			 */
			size_t number;
			/**
			 * The url, that the ids stored in this subsequence
			 * correspond to.
			 */
			char *url;
			LinkInfoList *next;
			
			size_t *linkData;
			size_t linkDataSize;
			
			void DeleteRecursively()
			{
				if(next != NULL)
					next->DeleteRecursively();
				delete next;
				next = NULL;
			}
			LinkInfoList()
			{
				position = 0;
				number = 0;
				url = NULL;
				linkData = NULL;
				linkDataSize = 0;
				next = NULL;
			};
			~LinkInfoList()
			{
				delete url;
				if(linkData)
					delete linkData;
			}
		};
		
		BufferList *currentBuffer;
		BufferList list;
		LinkInfoList *currentLink;
		LinkInfoList linkList;

		void SetBaseUrl(const char *url);
		char* GetBaseUrl();
		void PutValue(size_t val);
		void PutLink(const char *url);
		size_t Size();
		void Clear();
		void WriteToDest(size_t *dest);
		void PrintToCout();
		
		OutputGenerator();
		~OutputGenerator();
	};
}

