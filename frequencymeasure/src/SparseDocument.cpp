/*******************************************************************************

    Author:      Pavlov Anton
    Created:     2008/02/17

*******************************************************************************/

#include "SparseDocument.h"
#include <cmath>

using namespace Similarity;

template<class C> Allocator *AltAllocated<C>::altAlloc = NULL;

void SparseDocument::adjustBy(IDFStatistics &stats)
{
	Element *lst = list.next;
	while(lst != NULL)
	{
		if(stats.getMaxIdentifierNumber() <=lst->index)
			break;
		lst->weight *= stats[lst->index];
		lst = lst->next;
	}
}

real SparseDocument::dot(const SparseDocument &other) const
{
	if((list.next == NULL) && (other.list.next == NULL))
	{
		return 1.0; // documents are equal, but empty
	}
	real result = 0;

	Element *lstThis = list.next;
	Element *lstOther = other.list.next;

	while(lstThis != NULL && lstOther != NULL)
	{
		if(lstThis->index == lstOther->index)
		{
			result += lstThis->weight * lstOther->weight;
			lstThis = lstThis->next;
			lstOther = lstOther->next;
		}
		else if(lstThis->index > lstOther->index)
		{
			lstOther = lstOther->next;
		}
		else
		{
			lstThis = lstThis->next;
		}
	}
	return result;
}

size_t SparseDocument::getMaxIndex()
{
	return maxLocalIndex;
}

Element::Element(size_t ind, real w)
{
	next = NULL;
	index = ind;
	weight = w;
}

void SparseDocument::mergeIn(SparseDocument &other)
{
	Element *prevThis = &list;
	Element *lstOther = other.list.next;

	while(prevThis->next != NULL && lstOther != NULL)
	{
		if(prevThis->next->index == lstOther->index)
		{
			// sum weights of identical indices.
			prevThis->next->weight += lstOther->weight;
			Element *tmp = lstOther;
			lstOther = lstOther->next;
			prevThis = prevThis->next;
			delete tmp;
		}
		else if(prevThis->next->index > lstOther->index)
		{
			// move the element from the other sparse document.
			Element *tmp = prevThis->next; //  0-X-0-0-0	// prevThis->X
			prevThis->next = lstOther;     //      Q-0-0	// lstOther->Q
			lstOther = lstOther->next;     //      V		//
			prevThis->next->next = tmp;    //  0-X-Q-0-0-0	//
		}								   //        0-0	//
		else
		{
			prevThis = prevThis->next;
		}
	}

	// attach the end of the second document.
	if(prevThis->next == NULL && lstOther != NULL)
	{
		prevThis->next = lstOther;
	}

	if(other.getMaxIndex() > maxLocalIndex)
	{
		maxLocalIndex = other.getMaxIndex();
	}

	other.list.next = NULL;
}

void SparseDocument::normalize()
{
	real sum = 0;
	Element *lst = list.next;
	while(lst != NULL)
	{
		sum += lst->weight * lst->weight;
		lst = lst->next;
	}
	sum = sqrt(sum);
	if(sum < 0.001)
		sum = 0.001;

	lst = list.next;
	while(lst != NULL)
	{
		lst->weight /= sum;
		lst = lst->next;
	}
}

void SparseDocument::serialize(Serializer &ser)
{
	Element *lst = list.next;
	while(lst != NULL)
	{
		ser.write((int)lst->index);
		ser.write(lst->weight);
		lst = lst->next;
	}
	ser.write(-1);
}

SparseDocument::SparseDocument(Deserializer &deser)
{
	Element *last = &list;
	int value = deser.readInt();
	for(; value != -1; value = deser.readInt())
	{
#ifdef PRECISION_DOUBLE
		last->next = new Element(value,deser.readDouble());
#else
		last->next = new Element(value,deser.readFloat());
#endif
		maxLocalIndex = value;
		last = last->next;
	}
}

SparseDocument::SparseDocument(size_t index, real weight)
{
	list.next = new Element(index,weight);
	maxLocalIndex = index;
}

class Queue : private std::list<SparseDocument*>
{
private:
protected:
public:
	void pushBack(SparseDocument *doc)
	{
		push_back(doc);
	}
	SparseDocument *popFront()
	{
		SparseDocument *doc = *begin();
		pop_front();
		return doc;
	}
	size_t size()
	{
		return std::list<SparseDocument*>::size();
	}
};

struct Reader
{
	size_t buffer[32];
	size_t valuesRead;
	bool readLess;
	Document &doc;
	void readBatch()
	{
		valuesRead = doc.read(buffer,32);
		if(valuesRead < 32)
		{
			readLess = true;
		}
	}
	bool hasNext()
	{
		if(valuesRead == 0)
		{
			return false;
		}
		return true;
	}
	size_t getNext()
	{
		size_t result = buffer[valuesRead - 1];
		valuesRead--;
		if(valuesRead == 0 && !readLess)
		{
			readBatch();
		}
		return result;
	}
	Reader(Document &dc) :doc(dc)
	{
		valuesRead = 0;
		readLess = false;
		readBatch();
	};
};

SparseDocument::SparseDocument(Document &doc)
{
	maxLocalIndex = 0;
	Reader reader(doc);
	Queue q;
	while(reader.hasNext())
	{
		q.pushBack(new SparseDocument(reader.getNext(),1.0));
	}

	while(q.size() > 1)
	{
		SparseDocument *d1 = q.popFront();
		SparseDocument *d2 = q.popFront();
		d1->mergeIn(*d2);
		delete d2;
		q.pushBack(d1);
	}

	if(q.size() != 0)
	{
		SparseDocument *generatedDoc = q.popFront();

		list.next = generatedDoc->list.next;
		generatedDoc->list.next = NULL;

		maxLocalIndex = generatedDoc->maxLocalIndex;
		generatedDoc->maxLocalIndex = 0;

		delete generatedDoc;
	}
}

SparseDocument::SparseDocument()
{
	maxLocalIndex = 0;
}

SparseDocument::~SparseDocument()
{
	Element *lst = list.next;
	while(lst != NULL)
	{
		Element *tmp = lst;
		lst = lst->next;
		delete tmp;
	}
}

SparseDocument::SparseDocument(const Similarity::SparseDocument &other)
{
	Element *last = &list;
	Element *otherIter = other.list.next;
	while(otherIter != NULL)
	{
		last->next = new Element(otherIter->index,otherIter->weight);
		last = last->next;
		otherIter = otherIter->next;
	}
	maxLocalIndex = other.maxLocalIndex;
}


SparseDocument &SparseDocument::operator =(const Similarity::SparseDocument &other)
{
	// clear all values.
	Element *lst = list.next;
	while(lst != NULL)
	{
		Element *tmp = lst;
		lst = lst->next;
		delete tmp;
	}
	list.next = NULL;

	// load new values.
	Element *last = &list;
	Element *otherIter = other.list.next;
	while(otherIter != NULL)
	{
		last->next = new Element(otherIter->index,otherIter->weight);
		last = last->next;
		otherIter = otherIter->next;
	}
	maxLocalIndex = other.maxLocalIndex;
	return *this;
}

SparseDocument &SparseDocument::operator +=(const Similarity::SparseDocument &other)
{
	SparseDocument s(other);
	mergeIn(s);
	return *this;
}


SparseDocument &SparseDocument::operator /=(const real val)
{
	Element *curEl = list.next;
	while(curEl != NULL)
	{
		curEl->weight /= val;
		curEl = curEl->next;
	}
	return *this;
}

real SparseDocument::dist(const SparseDocument &other) const
{
	real result = 0;

	Element *lstThis = list.next;
	Element *lstOther = other.list.next;

	while(lstThis != NULL && lstOther != NULL)
	{
		if(lstThis->index == lstOther->index)
		{
			result += (lstThis->weight - lstOther->weight) * (lstThis->weight - lstOther->weight);
			lstThis = lstThis->next;
			lstOther = lstOther->next;
		}
		else if(lstThis->index > lstOther->index)
		{
			lstOther = lstOther->next;
		}
		else
		{
			lstThis = lstThis->next;
		}
	}
	return sqrt(result);
}