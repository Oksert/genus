/*******************************************************************************

    Author:      Pavlov Anton
    Created:     2006/11/29

*******************************************************************************/

#include "StreamDocument.h"
#include <fstream>

using namespace Similarity;

size_t StreamDocument::read(size_t *values, size_t maxRead)
{
    size_t read = 0;

    while(!feof(file) && read < maxRead)
    {
        read++;
        values[read-1] = getID();
        if(values[read-1] == 0)
        {
        	return read - 1;
        }
    }
    return read;
}

StreamDocument::StreamDocument(const char *filename)
{
	// removing number of ids
    file = fopen(filename,"r");
	if(file == NULL)
	{
		std::cout << "Can't open file: " << filename << std::endl;
	}
    readBytes = 0;
    readBatch();
	documentSize = getID();

}

void StreamDocument::readBatch()
{
    readBytes = fread(buffer,1,20,file);
    processedMarker = 0;
}

size_t StreamDocument::getID()
{
    char buf[21] = {0};
    size_t count = 0;
    while (true)
    {
        if(processedMarker >= readBytes)
        {
            if(feof(file))
                break;
            readBatch();
        }
        if(!isdigit(buffer[processedMarker]))
            break;
        buf[count] = buffer[processedMarker];
        count++;
        processedMarker++;
    }
    if(buf[0] == 0)
        return 0;
    processedMarker++;
    return atol(buf);
}

StreamDocument::~StreamDocument()
{
    fclose(file);
}

size_t StreamDocument::size()
{
	return documentSize;
}
