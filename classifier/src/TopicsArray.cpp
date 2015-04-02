/*******************************************************************************

    Author:      Pavlov Anton
    Created:     2007/03/05

*******************************************************************************/

#include "TopicsArray.h"

#ifndef NULL
#define NULL 0
#endif

void TopicsArray::addTopic(size_t topicID)
{
    // check, whether th topic is already contained
    for(size_t i = 0; i < numberOfTopics; i++)
    {
        if(topicsIdentifiers[i] == topicID)
            return;
    }

    // create a new array
    size_t *tmp = new size_t[numberOfTopics + 1];
    // copy old array to a new one
    for(size_t i = 0; i < numberOfTopics; i++)
        tmp[i] = topicsIdentifiers[i];
    // write a new value
    tmp[numberOfTopics] = topicID;

    // increase number of topics
    numberOfTopics++;

    // delete old array
    if(topicsIdentifiers)
        delete topicsIdentifiers;

    topicsIdentifiers = tmp;
};

TopicsArray::TopicsArray(const TopicsArray &arr)
{
    numberOfTopics = arr.numberOfTopics;
    topicsIdentifiers = new size_t[numberOfTopics];
    for(size_t i = 0; i < numberOfTopics; i++)
    {
        topicsIdentifiers[i] = arr.topicsIdentifiers[i];
    }
};

TopicsArray &TopicsArray::operator = (const TopicsArray &arr)
{
    if(topicsIdentifiers)
        delete topicsIdentifiers;

    numberOfTopics = arr.numberOfTopics;
    topicsIdentifiers = new size_t[numberOfTopics];
    for(size_t i = 0; i < numberOfTopics; i++)
    {
        topicsIdentifiers[i] = arr.topicsIdentifiers[i];
    }

    return *this;
};
TopicsArray::TopicsArray()
{
    numberOfTopics = 0;
    topicsIdentifiers = NULL;
};
TopicsArray::~TopicsArray()
{
    if(topicsIdentifiers)
        delete topicsIdentifiers;
};
