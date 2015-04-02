/*******************************************************************************

    Author:      Pavlov Anton
    Created:     2006/11/26
    Description: 

*******************************************************************************/

/**
 * @file
 * Contains declaration FrequencyMeasure class, which implements
 * SimilarityMeasure interface.
 */

#pragma once

#include "Types.h"
#include "Document.h"
#include "SimilarityMeasure.h"

namespace Similarity
{
    /**
     * The sparse document class, is used for storing weighted
     * documents in memory.
     *
     * Every document is presented as a sequence of
     * pairs (index;value), where index is a number of an identifier
     * and value is a real value, corresponding to frequency of
     * an identifier with a given number.
     *
     * Once SparseDocumentOld is weighted, it stores tf/idf values, and should not
     * be modified.
     *
     * Instances of this class should be created using FrequencyMeasure.
     */
    struct SparseDocumentOld
    {
#ifdef CONSTRUCTOR_CONTROL
        static size_t numCreated;
#endif

        /**
         * This structure is used as an element of a list
         * of (index;value) pairs.
         */
        struct ValueList 
        {
#ifdef CONSTRUCTOR_CONTROL
            static size_t numCreated;
#endif

            ValueList *next;
            size_t index;
            real value;

            void DeleteNonRecursively();
            ValueList *CopyRecursively();
            ValueList(size_t ind = 0);
            ~ValueList();


            // Memory allocation functions and static members to allow
            // customizeable memory allocation.
            //
            // Memory allocation is controlled through the ValueListAllocator class.
            static void *(*operator_new)(size_t);
            static void (*operator_delete)(void *,size_t);

            static void *__operator_new(size_t v);
            static void __operator_delete(void *ptr, size_t v);

            inline void *operator new (size_t v) {return operator_new(v);};
            inline void operator delete (void *ptr, size_t v) {operator_delete(ptr,v);};
        };

        typedef ValueList* pValueList;
        real valueSum;
        ValueList list;

        void AddWord(size_t index);
        void Add(SparseDocumentOld *doc);
        void DivideAllBy(real value);
        void NormalizeWithIDF(real *idf, size_t maxIndex);
        void Normalize();
        real Dot(SparseDocumentOld &doc);
        SparseDocumentOld();

        /**
         * Checks, whether this Document contains any values.
         *
         * @return true if the document is empty, false otherwise
         */
        bool Empty();
        /**
         * Creates a copy of this document.
         *
         * @return a new SparseDocumentOld identical to this one
         */
        SparseDocumentOld *Copy();
        ~SparseDocumentOld();
    };

    typedef SparseDocumentOld* pSparseDocumentOld;

    /**
     * An implementation of SimilarityMeasure, that uses tf/idf weighting
     * and dot product of resulting vectors as a similarity measure between
     * two documents.
     */
    class FrequencyMeasure : public SimilarityMeasure
    {
    protected:
        typedef real* preal;

        /**
         * The class that reads contents of a Document class and efficiently
         * constructs an instance of SparseDocumentOld, for a Document.
         */
        class DocumentReader
        {
        private:
            struct Stack 
            {
                Stack *next;
                SparseDocumentOld *doc;
                Stack();
            };
            static const size_t blockSize = 64;
            static const size_t generateDepth = 6;
            bool ReadyToMerge();
            void Merge();
            void Push(SparseDocumentOld *doc);
            SparseDocumentOld *GenerateDocument();

            Stack *readStack;
            size_t *readBuffer;
            size_t idsRead;
            size_t maxIndex;
            SparseDocumentOld **generatedDocuments;
        protected:
        public:
            /**
             * Builds a SparseDocumentOld from a part of a Document.
             *
             * @param doc the Document to be processed
             *
             * @param sz the number of identifiers, which are read from
             *     the Document.
             *
             * @return the resulting SparseDocumentOld
             */
            SparseDocumentOld *ReadPartial(Document *doc, size_t sz);
            /**
             * Builds a SparseDocumentOld from a Document.
             *
             * @param doc the Document to be processed
             *
             * @return the resulting SparseDocumentOld
             */
            SparseDocumentOld *ReadDocument(Document *doc);
            /**
             * Returns the maximum index, which occurred while
             * processing all documents. Used when calculating idf.
             */
            size_t GetMaxIndex();

            DocumentReader();
            ~DocumentReader();
        };


        /*-------------------------- Cache ----------------------------------*/

        /**
         * Defines whether to use cache at all.
         */
        bool useCache;
        /**
         * The array, where cache is formed.
         */
        real *cache;
        /**
         * The array, which is used to access cache
         */
        real **cacheAccess;
        /**
         * Size of cache
         */
        size_t cacheSize;
        /**
         * Number of loaded elements
         */
        size_t cached;
        /**
         * The array of indices of cached documents
         */
        size_t *cachedIndices;
        /**
         * The frequency of each cached document
         */
        real *cacheWeights;

        /**
         * Arrays, which are used when calculating values.
         * Placed here to avoid creating them every time.
         */
        SparseDocumentOld **cachedDocuments;
        SparseDocumentOld::ValueList **cachedDocumentsLists;

        /**
         * Calculates the values for one of the documents in cache.
         */
        void cacheNewDocument(size_t index);
        /**
         * Updates the frequencies of all documents, increasing
         * the values for two of them.
         */
        void updateCacheWeights(size_t updIndex1, size_t updIndex2);
        /**
         * Selects the document in cache, which is used least of all.
         */
        size_t selectLeastUsed();

        /*--------------------------- IDF -----------------------------------*/

        /**
         * The array of idf values for each identifier.
         */
        real *idf;
        /**
         * The maximum identifier.
         */
        size_t maxIDFIndex;

        /*--------------------------- Misc ----------------------------------*/

        /**
         * Used for reading documents.
         */
        DocumentReader reader;
        /**
         * the preloaded set of documents.
         */
        SparseDocumentOld **preloadedSet;
        /**
         * The size of preloaded set.
         */
        size_t setSize;

        /**
         * Generates idf out of preloaded set.
         */
        void GenerateIDF();
    public:
        /**
         * @see declaration of SimilarityMeasure::similarity()
         */
        virtual real similarity(Document &doc1, Document &doc2);
        /**
         * @see declaration of SimilarityMeasure::similarity()
         */
        virtual void similarity(Document &doc, real *dest);
        /**
         * @see declaration of SimilarityMeasure::similarity()
         */
        virtual void similarity(size_t i, real *dest);
        /**
         * @see declaration of SimilarityMeasure::similarity()
         */
        virtual real similarity(size_t i1, size_t i2);
        /**
         * @see declaration of SimilarityMeasure::preloadedSetSize()
         */
        virtual size_t preloadedSetSize();
        /**
         * @see declaration of SimilarityMeasure::preload()
         */
        virtual void preload(Document **docs, size_t number);

        /**
         * Creates a SparseDocumentOld from a given document.
         * Multiple calculations of similarity measure
         * using SparseDocumentOld is more efficient.
         *
         * @param doc the document to be processed
         *
         * @return resulting SparseDocumentOld
         */
        SparseDocumentOld *generateSparseDocument(Document &doc);

        /**
         * Calculates similarity measure between a sparse document
         * and a set of preloaded documents.
         * 
         * Using this function is more efficient than using
         * similarity(Document &, real *) function.
         * 
         * @see generateSparseDocument()
         *
         * @param doc the SparseDocument, to be compared with preloaded set
         *
         * @param dest the array, where to put resulting similarity values
         * must have length no less than preloadedSetSize().
         */
        void similarity(SparseDocumentOld &doc, real *dest);

        /**
         * Loads the specified documents into cache.
         *
         * @param numbers the array with identifiers of documents, which
         * will be loaded into cache.
         *
         * @param arrNumber the size of the numbers array.
         *
         * @param freq the initial frequency, to use for these documents.
         */
        void loadCache(size_t *numbers, size_t arrNumber, real freq);

        /**
         * Function, that outputs changes of similarity measure after
         * reading each batch of input document.
         * The output format is as follows:
         *  #250 0.00354 0.134 ...
         * The first number is number of ids read.
         * The following numbers are similarity measures between the preloaded
         * documents, and the document being processed.
         * 
         * @param doc the document, which is processed.
         *
         * @param step the number of elements in each step.
         *
         * @param output the name of the file, where to write output.
         */
        void similarityStats(Document &doc, size_t step, const char *output);

        /**
         * Writes the preloaded set to a file in text format.
         *
         * @param preloadedSetFilename the name of a file where to put the set.
         */
        //void saveToFile(const char *preloadedSetFilename);

        /**
         * Reads the measure data from a file, which was previously
         * generated by saveToFile() function.
         *
         * @param preloadedSetFilename the name of the file, which contains
         * measure data
         */
        //void readFromFile(const char *preloadedSetFilename);

        /**
         * Writes the preloaded set to a file in binary format.
         *
         * @param preloadedSetFilename the name of a file where to put the set.
         */
        void saveToBinaryFile(const char *preloadedSetFilename);

        /**
         * Reads the measure data from a file, which was previously
         * generated by saveToBinaryFile() function.
         *
         * @param preloadedSetFilename the name of the file, which contains
         * measure data
         */
        void readFromBinaryFile(const char *preloadedSetFilename);

        /**
         * Constructs the class. The preloaded set must be loaded using
         * the preload() function.
         *
         * @param useCache true if cache support is needed, false otherwise
         *
         * @param cacheSize the number of documents to be cached
         */
        FrequencyMeasure(bool useCache = true, size_t cacheSize = 1001);
        ~FrequencyMeasure();
    };
}

