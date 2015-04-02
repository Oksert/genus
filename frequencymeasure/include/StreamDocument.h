/*******************************************************************************

    Author:      Pavlov Anton
    Created:     2006/11/22
    Description: 

*******************************************************************************/

/**
 * @file
 * Provides a sample implementation of a Document interface.
 */

#pragma once

#include "Types.h"
#include "Document.h"

namespace Similarity
{
    /**
     * Reads a document from a file.
     *
     * The file must contain sequence of positive non-zero numbers
     * in text format separated by end of line character.
     *
     * The sequence may be finished by 0 number, or by end of file.
     *
     * The first element of the file is treated as the
     * number of elements in document.
     */
    class StreamDocument : public Document
    {
    private:
        FILE *file;
        char buffer[20];
        size_t readBytes;
        size_t processedMarker;
        void readBatch();
        size_t getID();
		size_t documentSize;
    protected:
    public:
        /**
         * @see Document::read()
         */
        virtual size_t read(size_t *values, size_t maxRead);
		virtual size_t size();
        /**
         * Constructs an instance of a StreamDocument with a specified
         * file as an input.
         * 
         * @param filename the name of an input file
         */
        StreamDocument(const char *filename);
        ~StreamDocument();
    };
}


