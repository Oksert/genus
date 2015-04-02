/*******************************************************************************

    Author:      Pavlov Anton
    Created:     2007/03/05
    Description: 

*******************************************************************************/

/**
 * @file
 * 
 */

#pragma once

#ifndef size_t
typedef unsigned int size_t;
#endif

struct TopicsArray
{
    /**
     * Number of topics in this array
     */
    size_t numberOfTopics;

    /**
     * The array that stores topics.
     *
     * Has length of numberOfTopics
     */
    size_t *topicsIdentifiers;

    /**
     * Adds a topic to the array.
     *
     * Destroys the old array with topics and creates a new one,
     * containing the new topic.
     *
     * @param topicID the new topic identifier
     */
    void addTopic(size_t topicID);

    /**
     * Copy Constructor
     */
    TopicsArray(const TopicsArray &arr);

    /**
     * Assignment operator.
     *
     * Creates a new storage array, and deletes old if neccessary.
     */
    TopicsArray &operator = (const TopicsArray &arr);

    TopicsArray();
    ~TopicsArray();
};
