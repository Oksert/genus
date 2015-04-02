#pragma once

#include "Types.h"
#include "SimilarityMeasure.h"
#include "stree.h"

using namespace std;

namespace Similarity
{

    class KSpectrum : public SimilarityMeasure {
    public:
        KSpectrum(size_t k, Document **docs, size_t cache_size);
        KSpectrum();
        ~KSpectrum();
        real similarity(Document &doc1, Document &doc2);
        void similarity(Document &doc, real *dest);
        real similarity(size_t i1, size_t i2);
        void similarity(size_t i1, real *dest);
        size_t preloadedSetSize();
        void preload(Document **docs, size_t number);
        void saveToBinaryFile(const char *preloadedSetFilename);
        void readFromBinaryFile(const char *preloadedSetFilename);
        void clear();
    private:
        size_t kernel;
        Document **docs;
        size_t **strings_from_docs;
        size_t *strings_from_docs_size;
        size_t number_of_examples;
        void DocsToStrings(size_t **strings_from_docs, Document **docs);
        void CreateSuffixTree(size_t *doc, size_t size, BuildSuffixTree *tree);
        float CountWithTrees(size_t *str, size_t str_size, BuildSuffixTree *tree, size_t *source);
        bool IntStrcmp(size_t *str1, size_t *str2);

        //все что отвечает за кэш
        void MakeCache(size_t cache_size);
        size_t cache_size;
        size_t actual_size;
        BuildSuffixTree **ex;
        float **cache;
        size_t *cache_dependency;
        size_t *cache_priority;
        float GetElementOfMatrix(size_t i, size_t j);
        void RaisePriority(size_t i);
        size_t ChangeElementInCache(size_t i);
    };
}
