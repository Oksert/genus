/*******************************************************************************

    Author:      Pavlov Anton
    Created:     2006/12/14

*******************************************************************************/

#include "FrequencyMeasure.h"

using namespace Similarity;

SparseDocumentOld *FrequencyMeasure::DocumentReader::ReadPartial(Similarity::Document *doc, size_t sz)
{
    readStack = NULL;

    // read as always, until it is time to stop
    while(sz > 0)
    {
        idsRead = doc->read(readBuffer,(blockSize < sz)? (blockSize) : (sz));
        if(idsRead == 0)
            break;

        Push(GenerateDocument());
        while(ReadyToMerge())
            Merge();

        if(idsRead < blockSize)
            break;

        sz -= idsRead;
    }

    if(readStack == NULL)
        return new SparseDocumentOld;
    while(readStack->next != NULL)
        Merge();

    return readStack->doc;
}

void FrequencyMeasure::similarityStats(Document &doc, size_t step, const char *output)
{
    FILE *out;
    out = fopen(output,"w");
    SparseDocumentOld *sdoc = new SparseDocumentOld;
    size_t count = 0;
    while(true)
    {
        SparseDocumentOld *newPart = reader.ReadPartial(&doc,step);
        count += (size_t)(newPart->valueSum);
        
        if(newPart->Empty())
            break;

        sdoc->Add(newPart);
        SparseDocumentOld *temp = sdoc->Copy();
        temp->NormalizeWithIDF(idf,maxIDFIndex);

        fprintf(out,"#%i\t",count);
        //out << "#" << count << "\t";

        for(size_t i = 0; i < setSize; i++)
        {
            fprintf(out,"%f\t",temp->Dot(*preloadedSet[i]));
            //out << std::setprecision(4) << temp->Dot(*preloadedSet[i]) << "\t";
        }

        fprintf(out,"\n");
        //out << std::endl;

        delete temp;
    }

    fclose(out);

    delete sdoc;
}
