/*******************************************************************************

    Author:      Pavlov Anton
    Created:     2006/12/04
    Description: 

*******************************************************************************/

/**
 * @file
 * Contains a utility implementation of Document class, which
 * can be used to store Document data in memory.
 */

#pragma once

#include "Types.h"
#include "Document.h"
#include "AlternativeAllocator.h"

namespace Similarity
{
    /**
     * This implementation of Document class is used to store
     * contents of another document in memory.
     */
    class ReadWholeDocument : public Document
    {
	public:
		struct Queue : public AltAllocated<Queue>
		{
			Queue *next;
			size_t id;
			Queue(size_t i)
			{
				next = NULL;
				id = i;
			};
			void DeleteNonRecursively()
			{
				Queue *ptr = next;
				Queue *del;
				while(ptr != NULL)
				{
					del = ptr;
					ptr = ptr->next;
					delete del;
				}
			};
		};
    private:
		

        Queue queue;
		Queue *iter;
        Queue *lastElement;
        bool empty()
        {
            return iter->next == NULL;
        }
        void add(size_t i)
        {
            lastElement->next = new Queue(i);
            lastElement = lastElement->next;
        }
        size_t get()
        {
            size_t id = iter->next->id;
			iter = iter->next;
			return id;
        }

		size_t _size;

    protected:
    public:
        /**
         * @see Document::read()
         */
        virtual size_t read(size_t *values, size_t maxRead)
        {
            size_t readBytes = 0;
            while(!empty() && maxRead > 0)
            {
                *values = get();
                values++;
                maxRead--;
                readBytes++;
            }
            return readBytes;
        }
		virtual size_t size()
		{
			return _size;
		}
        /**
         * Constructs an instance of ReadWholeDocument.
         * Reads the whole contents of input Document, and
         * stores its data in a buffer.
         *
         * The pointer to the input document is no longer used after
         * calling constructor, thus resources allocated for the input document
         * can be freed.
         * 
         * @param inputDoc the document to store in memory
         */
        ReadWholeDocument(Document *inputDoc)
            :queue(0)
        {
			iter = &queue;
			_size = 0;

            lastElement = &queue;
            size_t input;
            size_t read_ids = inputDoc->read(&input,1);
			_size += read_ids;
            while(read_ids != 0)
            {
                add(input);
                read_ids = inputDoc->read(&input,1);
				_size += read_ids;
            }
        };

		void reset()
		{
			iter = &queue;
		}
        ~ReadWholeDocument()
        {
            queue.DeleteNonRecursively();
        };
    };
}
