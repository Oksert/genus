/*******************************************************************************

    Author:      Pavlov Anton
    Created:     2008/02/17
    Description: 

*******************************************************************************/

/**
 * @file
 * 
 */

#pragma once

#include "LibFrequencyMeasure.h"

#ifndef NULL
#define NULL (0)
#endif

#ifndef size_t
//typedef unsigned int size_t;
#endif

namespace Similarity
{
#ifndef REAL_DEFINED
	/**
	 * Type definitions for the library.
	 */
#ifdef PRECISION_DOUBLE
	typedef double real;
#else
	typedef float real;
#endif

#define REAL_DEFINED

#endif
}

