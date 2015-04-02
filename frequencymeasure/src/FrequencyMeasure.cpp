/*******************************************************************************

    Author:      Pavlov Anton
    Created:     2006/11/28

*******************************************************************************/

#include "FrequencyMeasure.h"

using namespace Similarity;

#ifdef CONSTRUCTOR_CONTROL
size_t SparseDocumentOld::numCreated = 0;
size_t SparseDocumentOld::ValueList::numCreated = 0;
#endif

void *Similarity::SparseDocumentOld::ValueList::__operator_new(size_t v)
{
    return ::operator new(v);
}

void Similarity::SparseDocumentOld::ValueList::__operator_delete(void *ptr, size_t v)
{
    ::operator delete(ptr);
}

void *(*Similarity::SparseDocumentOld::ValueList::operator_new)(size_t) = Similarity::SparseDocumentOld::ValueList::__operator_new;
void (*Similarity::SparseDocumentOld::ValueList::operator_delete)(void *,size_t) = Similarity::SparseDocumentOld::ValueList::__operator_delete;

SparseDocumentOld::ValueList::ValueList(size_t ind)
{
#ifdef CONSTRUCTOR_CONTROL
    numCreated++;
#endif
    next = NULL;
    index = ind;
    value = 1.0;
}

SparseDocumentOld::ValueList::~ValueList()
{
#ifdef CONSTRUCTOR_CONTROL
    numCreated--;
#endif
}

void SparseDocumentOld::ValueList::DeleteNonRecursively()
{
    ValueList *nextIt = next;
    ValueList *tmp;
    while(nextIt != NULL)
    {
        tmp = nextIt->next;
        delete nextIt;
        nextIt = tmp;
    }
    next = NULL;
    value = 1.0;
    index = 0;
}

SparseDocumentOld::ValueList *SparseDocumentOld::ValueList::CopyRecursively()
{
    ValueList *newList;
    if(next != NULL)
    {
        newList = new ValueList(next->index);
        newList->value = next->value;
        newList->next = next->CopyRecursively();
    }
    else
    {
        newList = NULL;
    }
    return newList;
}

bool SparseDocumentOld::Empty()
{
    return list.next == NULL;
}

SparseDocumentOld *SparseDocumentOld::Copy()
{
    SparseDocumentOld *newDoc = new SparseDocumentOld;
    newDoc->valueSum = valueSum;
    newDoc->list.next = list.CopyRecursively();
    return newDoc;
}



void SparseDocumentOld::AddWord(size_t index)
{
    if(list.next == NULL)
    {
        list.next = new ValueList(index);
        valueSum = 1.0;
    }
    else
    {
        throw new std::exception(
#ifdef WIN32
            "The FrequencyMeasure::SparseDocumentOld::AddWord() function is supposed to operate with empty list."
#endif
            );
    }
}

void SparseDocumentOld::Add(SparseDocumentOld *doc)
{
    ValueList *l1 = &list;
    ValueList *l2 = &(doc->list);
    ValueList *temp;
    while(l1->next != NULL && l2->next != NULL)
    {
        if(l1->next->index < l2->next->index)
        {
            l1 = l1->next;
        }
        else if(l2->next->index < l1->next->index)
        {
            temp = l2->next;
            l2->next = l2->next->next;
            temp->next = l1->next;
            l1->next = temp;
            l1 = temp;
        }
        else
        {
            temp = l2->next;
            l2->next = l2->next->next;
            l1->next->value += temp->value;
            l1 = l1->next;
            delete temp;
        }
    }
    if(l1->next == NULL)
    {
        l1->next = l2->next;
    }

    doc->list.next = NULL;
    valueSum += doc->valueSum;
    doc->valueSum = 0.0;
}

void SparseDocumentOld::DivideAllBy(real val)
{
    ValueList *lst = &list;
    while(lst->next != NULL)
    {
        lst->next->value /= val;
        lst = lst->next;
    }
}

void SparseDocumentOld::NormalizeWithIDF(real *idf, size_t maxIndex)
{
    ValueList *lst = &list;
    real squareSum = 0;
    valueSum = 0;
    while(lst->next != NULL)
    {
        if(lst->next->index > maxIndex)
        {
            lst->DeleteNonRecursively();
            lst->next = NULL;
            continue;
        }
        lst->next->value *= idf[lst->next->index];

        squareSum += lst->next->value * lst->next->value;
        valueSum += lst->next->value;

        lst = lst->next;
    }

    if(squareSum > 0)
    {
        valueSum = sqrt(squareSum); // setting the length of the vector
    }
    else
    {
    	valueSum = 1;
    }
}

real SparseDocumentOld::Dot(SparseDocumentOld &doc)
{
    ValueList *l1 = &list;
    ValueList *l2 = &(doc.list);

    real dotSum = 0;

    while(l1->next != NULL && l2->next != NULL)
    {
        if(l1->next->index > l2->next->index)
            l2 = l2->next;
        else if(l1->next->index < l2->next->index)
            l1 = l1->next;
        else
        {
            dotSum += l1->next->value * l2->next->value;
            l1 = l1->next;
            l2 = l2->next;
        }
    }

    dotSum /= valueSum;
    dotSum /= doc.valueSum;

    return dotSum;
}

SparseDocumentOld::SparseDocumentOld()
:list(0)
{
#ifdef CONSTRUCTOR_CONTROL
    numCreated++;
#endif
    valueSum = 0.0;
}

SparseDocumentOld::~SparseDocumentOld()
{
#ifdef CONSTRUCTOR_CONTROL
    numCreated--;
#endif
    list.DeleteNonRecursively();
}

FrequencyMeasure::DocumentReader::Stack::Stack()
{
    doc = NULL;
    next = NULL;
}

bool FrequencyMeasure::DocumentReader::ReadyToMerge()
{
    if(readStack != NULL && readStack->next != NULL)
    {
        return readStack->doc->valueSum >= readStack->next->doc->valueSum;
    }
    else
    {
        return false;
    }
}

void FrequencyMeasure::DocumentReader::Merge()
{
    readStack->next->doc->Add(readStack->doc);
    Stack *temp = readStack;
    readStack = readStack->next;
    delete temp->doc;
    delete temp;
}

void FrequencyMeasure::DocumentReader::Push(SparseDocumentOld *doc)
{
    Stack *st = new Stack;
    st->doc = doc;
    st->next = readStack;
    readStack = st;
}

SparseDocumentOld *FrequencyMeasure::DocumentReader::GenerateDocument()
{
    for(size_t i = 0; i < idsRead; i++)
    {
        if(readBuffer[i] > maxIndex)
            maxIndex = readBuffer[i];
        (generatedDocuments[i])->AddWord(readBuffer[i]);
    }

    size_t step = 2;
    size_t look = 1;

    for(size_t i = 0; i < generateDepth; i++)
    {
        for(size_t index = 0; index + look < idsRead; index += step)
        {
            (generatedDocuments[index])->Add(generatedDocuments[index + look]);
        }
        step += step;
        look += look;
    }

    SparseDocumentOld *newDoc = new SparseDocumentOld;
    newDoc->Add(generatedDocuments[0]);

    return newDoc;
}

SparseDocumentOld *FrequencyMeasure::DocumentReader::ReadDocument(Document *doc)
{
    readStack = NULL;
    while(true)
    {
        idsRead = doc->read(readBuffer,blockSize);
        if(idsRead == 0)
            break;

        Push(GenerateDocument());
        while(ReadyToMerge())
            Merge();

        if(idsRead < blockSize)
            break;
    }
    
    if(readStack == NULL)
    	return new SparseDocumentOld;

    while(readStack->next != NULL)
        Merge();

    return readStack->doc;
}

size_t FrequencyMeasure::DocumentReader::GetMaxIndex()
{
    return maxIndex;
}

FrequencyMeasure::DocumentReader::DocumentReader()
{
    maxIndex = 0;
    readBuffer = new size_t[blockSize];
    generatedDocuments = new pSparseDocumentOld[blockSize];
    for(size_t i = 0; i < blockSize; i++)
        generatedDocuments[i] = new SparseDocumentOld;
}

FrequencyMeasure::DocumentReader::~DocumentReader()
{

    for(size_t i = 0; i < blockSize; i++)
        delete generatedDocuments[i];
    delete []generatedDocuments;
    delete []readBuffer;

}

FrequencyMeasure::FrequencyMeasure(bool _useCache, size_t cache_size)
{
    useCache = _useCache;
    idf = NULL;
    setSize = 0;
    preloadedSet = NULL;

    cached = 0;
    cacheSize = cache_size;
    if(useCache)
    {
        cache = new real[cacheSize * (cacheSize - 1) / 2];
        cacheAccess = new preal[cacheSize];
        size_t step = 0;
        size_t addr = 0;
        for(size_t i = 0; i < cacheSize; i++, addr += step, step++)
        {
            cacheAccess[i] = &(cache[addr]);
        }
        cachedIndices = new size_t[cacheSize];
        cacheWeights = new real[cacheSize];

        cachedDocuments = new pSparseDocumentOld[cacheSize];
        cachedDocumentsLists = new SparseDocumentOld::pValueList[cacheSize];
    }
    else
    {
        cache = NULL;
        cacheAccess = NULL;
        cachedDocumentsLists = NULL;
        cachedDocuments = NULL;
        cachedIndices = NULL;
        cacheWeights = NULL;
    }
    
}

FrequencyMeasure::~FrequencyMeasure()
{

    if(idf)
        delete idf;

    for(size_t i = 0; i < setSize; i++)
    {

        delete preloadedSet[i];
    }

    if(preloadedSet)
        delete preloadedSet;

    if(cache)
        delete cache;
    if(cacheAccess)
        delete cacheAccess;
    if(cachedIndices)
        delete cachedIndices;
    if(cacheWeights)
        delete cacheWeights;
    if(cachedDocuments)
        delete cachedDocuments;
    if(cachedDocumentsLists)
        delete cachedDocumentsLists;
}

void FrequencyMeasure::similarity(Similarity::Document &doc, real *dest)
{
    SparseDocumentOld *sdoc1 = reader.ReadDocument(&doc);
    sdoc1->NormalizeWithIDF(idf,maxIDFIndex);

    for(size_t i = 0; i < setSize; i++)
    {
        if(i == 5638)
        {
            int k = 0;
            k++;
        }
        dest[i] = sdoc1->Dot(*preloadedSet[i]);
    }

    delete sdoc1;
}

void FrequencyMeasure::similarity(SparseDocumentOld &doc, real *dest)
{
    for(size_t i = 0; i < setSize; i++)
    {
        dest[i] = doc.Dot(*preloadedSet[i]);
    }
}

void FrequencyMeasure::similarity(size_t k, real *dest)
{
    SparseDocumentOld *sdoc1 = preloadedSet[k];

    for(size_t i = 0; i < setSize; i++)
    {
        dest[i] = sdoc1->Dot(*preloadedSet[i]);
    }
}

real FrequencyMeasure::similarity(Similarity::Document &doc1, Similarity::Document &doc2)
{
    SparseDocumentOld *sdoc1 = reader.ReadDocument(&doc1);
    SparseDocumentOld *sdoc2 = reader.ReadDocument(&doc2);

    sdoc1->NormalizeWithIDF(idf,maxIDFIndex);
    sdoc2->NormalizeWithIDF(idf,maxIDFIndex);

    real result = sdoc1->Dot(*sdoc2);

    delete sdoc1;
    delete sdoc2;
    return result;
}

size_t FrequencyMeasure::preloadedSetSize()
{
    return setSize;
}

void FrequencyMeasure::preload(Similarity::Document **docs, size_t number)
{
    preloadedSet = new pSparseDocumentOld[number];
    for(size_t i = 0; i < number; i++)
    {
        preloadedSet[i] = reader.ReadDocument(docs[i]);
    }

    setSize = number;

    /**
     * Getting maximum encountered index and adding 1, to get the length of
     * the idf array.
     */
    maxIDFIndex = reader.GetMaxIndex() + 1;
    idf = new real[maxIDFIndex];

    GenerateIDF();
    
    for(size_t i = 0; i <number; i++)
    {
        preloadedSet[i]->NormalizeWithIDF(idf,maxIDFIndex);
    }

	//log4cpp::Category::getInstance("frequencymeasure.FrequencyMeasure").infoStream() <<"Maximum idf index: " << maxIDFIndex << log4cpp::CategoryStream::ENDLINE; 
}

#define MAX_IDF (10000000)

void FrequencyMeasure::GenerateIDF()
{
    for(size_t i = 0; i < maxIDFIndex; i++)
    {
        idf[i] = 0;
    }

    for(size_t i = 0; i < setSize; i++)
    {
        SparseDocumentOld::ValueList *list = &(preloadedSet[i]->list);
        while(list->next != NULL)
        {
            idf[list->next->index] += 1.0;
            list = list->next;
        }
    }

    for(size_t i = 0; i < maxIDFIndex; i++)
    {
        if(idf[i] > 0)
            idf[i] = log((real)setSize / idf[i]);
    }
}

real FrequencyMeasure::similarity(size_t i1, size_t i2)
{
    if(!useCache)
    {
        SparseDocumentOld *sdoc1, *sdoc2;
        sdoc1 = preloadedSet[i1];
        sdoc2 = preloadedSet[i2];
        return sdoc1->Dot(*sdoc2);
    }



    if(i1 == i2)
        return 1.0;         // Should we calculate this case?
    bool i1Found = false;
    size_t i1Position;
    bool i2Found = false;
    size_t i2Position;

    // Searching the cache for these indices.
    for(size_t i = 0; i < cached; i++)
    {
        if(cachedIndices[i] == i1)
        {
            i1Position = i;
            i1Found = true;
        }
        if(cachedIndices[i] == i2)
        {
            i2Position = i;
            i2Found = true;
        }
    }

    if(i1Found && i2Found)
    {
        // both found fetch value
        size_t temp = /*max(i1,i2); */(i1Position > i2Position)?(i1Position):(i2Position);
        i1Position = /*min(i1Position,i2Position);*/ (i1Position < i2Position)?(i1Position):(i2Position);
        i2Position = temp;
        updateCacheWeights(i1Position,i2Position);
        return cacheAccess[i2Position][i1Position];
    }
    else
    {
        if(!i1Found)
        {
            // Select position, which to substitute
            i1Position = selectLeastUsed();
            // Check whether we are expanding cache.
            if(i1Position >= cached)
            {
                cached = i1Position + 1;
            }
            // Set weight to 0 so that selectLeastUsed function won't choose it
            // for substitution.
            cacheWeights[i1Position] = 0;
            // Update indices and all supplementary arrays.
            cachedIndices[i1Position] = i1;
            cachedDocuments[i1Position] = preloadedSet[i1];
            cachedDocumentsLists[i1Position] = &(preloadedSet[i1]->list);
            cacheNewDocument(i1Position);
        }

        if(!i2Found)
        {
            i2Position = selectLeastUsed();
            if(i2Position >= cached)
            {
                cached = i2Position + 1;
            }
            cacheWeights[i2Position] = 0;
            cachedIndices[i2Position] = i2;
            cachedDocuments[i2Position] = preloadedSet[i2];
            cachedDocumentsLists[i2Position] = &(preloadedSet[i2]->list);
            cacheNewDocument(i2Position);
        }

        size_t temp = /*max(i1,i2); */(i1Position > i2Position)?(i1Position):(i2Position);
        i1Position = /*min(i1Position,i2Position);*/ (i1Position < i2Position)?(i1Position):(i2Position);
        i2Position = temp;
        updateCacheWeights(i1Position,i2Position);
        return cacheAccess[i2Position][i1Position];
    }
}

void FrequencyMeasure::cacheNewDocument(size_t index)
{
    // set changing values to 0.
    for(size_t i = 0; i < index; i++)
    {
        cacheAccess[index][i] = 0;
    }
    for(size_t i = index + 1; i < cached; i++)
    {
        cacheAccess[i][index] = 0;
    }

    // complex method - simultaneous calculation of dot products.
    
    // select the leading document (the one which must be calculated)
    SparseDocumentOld::ValueList *leading = cachedDocumentsLists[index];
    // calculating dot product for all documents simultaneously.
    while(1)
    {
        // reached the end of the leading document - break the loop
        if(leading->next == NULL)
            break;
        // the documents with index lower than of the leading document
        for(size_t i = 0; i < index; i++)
        {
            // find the element with the same index, as the leading document's
            // current index.
            while(cachedDocumentsLists[i]->next != NULL && cachedDocumentsLists[i]->next->index < leading->next->index)
                cachedDocumentsLists[i] = cachedDocumentsLists[i]->next;
            // this document is finished already - skipping it
            if(cachedDocumentsLists[i]->next == NULL)
                continue;
            
            if(cachedDocumentsLists[i]->next->index == leading->next->index)
            {
                // we found the element - updating the dot product
                cacheAccess[index][i] += cachedDocumentsLists[i]->next->value * leading->next->value;
            }
        }

        for(size_t i = index + 1; i < cached; i++)
        {
            while(cachedDocumentsLists[i]->next != NULL && cachedDocumentsLists[i]->next->index < leading->next->index)
                cachedDocumentsLists[i] = cachedDocumentsLists[i]->next;
            if(cachedDocumentsLists[i]->next == NULL)
                continue;
            if(cachedDocumentsLists[i]->next->index == leading->next->index)
            {
                cacheAccess[i][index] += cachedDocumentsLists[i]->next->value * leading->next->value;
            }
        }

        // selecting next element of the leading document
        leading = leading->next;
    }

    // normalizing calculated values
    for(size_t i = 0; i < index; i++)
    {
        cacheAccess[index][i] /= cachedDocuments[i]->valueSum * cachedDocuments[index]->valueSum;
    }
    for(size_t i = index + 1; i < cached; i++)
    {
        cacheAccess[i][index] /= cachedDocuments[i]->valueSum * cachedDocuments[index]->valueSum;
    }

    // restoring the cachedDocumentsLists
    for(size_t i = 0; i < cached; i++)
    {
        cachedDocumentsLists[i] = &(cachedDocuments[i]->list);
    }
/*
    // The simple method - a bunch of dot products

    for(size_t i = 0; i < index; i++)
    {
        cacheAccess[i][index] = cachedDocuments[index]->Dot(*cachedDocuments[i]);
    }

    for(size_t i = index + 1; i < cached; i++)
    {
        cacheAccess[index][i] = cachedDocuments[index]->Dot(*cachedDocuments[i]);
    }*/
}

void FrequencyMeasure::updateCacheWeights(size_t updIndex1, size_t updIndex2)
{
    cacheWeights[updIndex1] += 1.0;
    cacheWeights[updIndex2] += 1.0;
}

size_t FrequencyMeasure::selectLeastUsed()
{
    if(cached < cacheSize)
        return cached;

    real minValue;
    size_t minIndex;
    if(cacheWeights[0] > 0)
    {
        minValue = cacheWeights[0];
    }
    else
    {
        minValue = cacheWeights[1];
    }
    for(size_t i = 0; i < cached; i++)
    {
        if(cacheWeights[i] > 0 && cacheWeights[i] <= minValue)
        {
            minIndex = i;
            minValue = cacheWeights[i];
        }
    }

    return minIndex;
}

void FrequencyMeasure::loadCache(size_t *numbers, size_t arrNumber, real freq)
{
    if(!useCache)
        return;
    cached = 0;
    for(size_t i = 0; i < ((arrNumber < cacheSize)?(arrNumber):(cacheSize)); i++)
    {
        cached++;
        cacheWeights[i] = freq;
        cachedIndices[i] = numbers[i];
        cachedDocuments[i] = preloadedSet[numbers[i]];
        cachedDocumentsLists[i] = &(preloadedSet[numbers[i]]->list);
        cacheNewDocument(i);
    }
}

SparseDocumentOld *FrequencyMeasure::generateSparseDocument(Similarity::Document &doc)
{
    SparseDocumentOld *d = reader.ReadDocument(&doc);
    d->NormalizeWithIDF(idf,maxIDFIndex);
    return d;
}
/*
void FrequencyMeasure::saveToFile(const char *preloadedSetFilename)
{
    try
    {
        std::ofstream output(preloadedSetFilename);

        output << setSize << " " << maxIDFIndex << std::endl;

        for(size_t i = 1; i < maxIDFIndex; i++)
        {
            output << idf[i] << " ";
        }
        output << std::endl;

        for(size_t i = 0; i < setSize; i++)
        {
            if(preloadedSet[i]->Empty())
            {
                output << "#" << std::endl;
                continue;
            }
            output << preloadedSet[i]->valueSum << " ";

            SparseDocumentOld::ValueList *ptr = preloadedSet[i]->list.next;
            if(ptr != NULL)
            {
                output << ptr->index << ":" << ptr->value;
                ptr = ptr->next;
            }
            while(ptr != NULL)
            {
                output << " " << ptr->index << ":" << ptr->value;
                ptr = ptr->next;
            }
            output << std::endl;
        }
    }
    catch (std::exception *)
    {
        std::cerr << "An exception has occured, while saving measure data to a file." << std::endl;
        std::cerr << "Filename: " << preloadedSetFilename << std::endl;
    }
    catch (std::exception &)
    {
        std::cerr << "An exception has occured, while saving measure data to a file." << std::endl;
        std::cerr << "Filename: " << preloadedSetFilename << std::endl;
    }
}

void FrequencyMeasure::readFromFile(const char *preloadedSetFilename)
{
    // Reading from file
    try
    {
        char temp[10];

        std::ifstream preloadedSetFile(preloadedSetFilename);

        // Read major characteristics
        preloadedSetFile >> setSize;
        preloadedSetFile >> maxIDFIndex;

        // Read idf array
        // Warning! idf indexes start with 1, not with 0.
        idf = new float[maxIDFIndex + 1];
        for(size_t i = 1; i < maxIDFIndex; i++)
        {
            preloadedSetFile >> idf[i];
        }

        // Skip the \n and \r symbols
        preloadedSetFile.getline(temp,8);

        // Read all documents
        preloadedSet = new pSparseDocumentOld[setSize];
        for(size_t i = 0; i < setSize; i++)
        {
            preloadedSet[i] = new SparseDocumentOld();

            if(preloadedSetFile.peek() == '#')
            {
                preloadedSetFile.getline(temp,8);
                preloadedSet[i]->valueSum = 1.0;
                continue;
            }

            preloadedSetFile >> preloadedSet[i]->valueSum;

            SparseDocumentOld::ValueList *last = &(preloadedSet[i]->list);
            // Each document on a single line
            while(preloadedSetFile.peek() != '\n' && preloadedSetFile.peek() != '\r' && !preloadedSetFile.eof())
            {
                last->next = new SparseDocumentOld::ValueList;
                last = last->next;
                preloadedSetFile >> last->index;
                // Indexes and values are separated by a symbol (actually by a ':')
                preloadedSetFile.get();
                preloadedSetFile >> last->value;
            }

            // Skip the \n and \r symbols
            if(!preloadedSetFile.eof())
                preloadedSetFile.getline(temp,8);
        }
    }
    catch (std::exception *)
    {
        std::cerr << "An exception has occured while reading measure from a file." << std::endl;
        std::cerr << "Filename: " << preloadedSetFilename << std::endl;
        throw;
    }
    catch (std::exception &)
    {
        std::cerr << "An exception has occured while reading measure from a file." << std::endl;
        std::cerr << "Filename: " << preloadedSetFilename << std::endl;
        throw;
    }
}
*/
#define CH(val) (&(val))


void FrequencyMeasure::saveToBinaryFile(const char *preloadedSetFilename)
{


    FILE *file;
    file = fopen(preloadedSetFilename,"wb");

    if(!file)
    {
    	//log4cpp::Category::getInstance("frequencymeasure.FrequencyMeasure").errorStream() << "Could not open the " << preloadedSetFilename << " file. Write failed." << log4cpp::CategoryStream::ENDLINE;
        return;
    }

    fwrite(CH(setSize),sizeof(size_t),1,file);
    fwrite(CH(maxIDFIndex),sizeof(size_t),1,file);

    for(size_t i = 1; i < maxIDFIndex; i++)
    {
        fwrite(CH(idf[i]),sizeof(real),1,file);
    }

    for(size_t i = 0; i < setSize; i++)
    {
        fwrite(CH(preloadedSet[i]->valueSum),sizeof(real),1,file);

        SparseDocumentOld::ValueList *ptr = preloadedSet[i]->list.next;
        if(ptr != NULL)
        {
            fwrite(CH(ptr->index),sizeof(size_t),1,file);
            fwrite(CH(ptr->value),sizeof(real),1,file);
            ptr = ptr->next;
        }
        while(ptr != NULL)
        {
            fwrite(CH(ptr->index),sizeof(size_t),1,file);
            fwrite(CH(ptr->value),sizeof(real),1,file);
            ptr = ptr->next;
        }
        char tmp[10] = {0};
        fwrite(tmp,1,sizeof(size_t),file);
    }
    fclose(file);
}

void FrequencyMeasure::readFromBinaryFile(const char *preloadedSetFilename)
{
    char buf[10];

    FILE *file;
    file = fopen(preloadedSetFilename,"rb");

    if(!file)
    {
        //log4cpp::Category::getInstance("frequencymeasure.FrequencyMeasure").errorStream() << "Could not open the " << preloadedSetFilename << " file. Read failed." << log4cpp::CategoryStream::ENDLINE;
        
        return;
    }

    // Read major characteristics
    fread(CH(setSize),sizeof(size_t),1,file);
    fread(CH(maxIDFIndex),sizeof(size_t),1,file);

    // Read idf array
    // Warning! idf indexes start with 1, not with 0.
    idf = new real[maxIDFIndex];
    for(size_t i = 1; i < maxIDFIndex; i++)
    {
        fread(CH(idf[i]),sizeof(real),1,file);
    }

    // Read all documents
    preloadedSet = new pSparseDocumentOld[setSize];
    for(size_t i = 0; i < setSize && !feof(file); i++)
    {
        preloadedSet[i] = new SparseDocumentOld();

        fread(CH(preloadedSet[i]->valueSum),sizeof(real),1,file);

        SparseDocumentOld::ValueList *last = &(preloadedSet[i]->list);
        while(!feof(file))
        {
            fread(buf,1,sizeof(size_t),file);
            bool allZero = true;
            for(size_t k = 0; k < sizeof(size_t); k++)
                if(buf[k] != 0)
                {
                    allZero = false;
                    break;
                }

            if(allZero)
                break;

            last->next = new SparseDocumentOld::ValueList;
            last = last->next;
            memcpy(CH(last->index),buf,sizeof(size_t));
            fread(CH(last->value),sizeof(real),1,file);
        }
    }

    fclose(file);
}

#undef CH
