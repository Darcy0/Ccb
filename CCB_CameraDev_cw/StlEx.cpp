#include "StlEx.h"

CVoidVector::CVoidVector()
{
   clear();
}
CVoidVector::~CVoidVector()
{
   clear();
}
void CVoidVector::AddTailElement(void * ptr)
{
	push_back(ptr);
}
void CVoidVector::DeleteTailElement()
{
    pop_back();
}
bool CVoidVector::FindElement(void * ptr)
{
	for(Iterator=begin();Iterator!=end();Iterator++ )
	{
		if(ptr==(*Iterator))
		{
			return true;
		}
	}
	return false;
}
void CVoidVector::DeleteElement(void * ptr)
{
	for(Iterator=begin();Iterator!=end();Iterator++ )
	{
		if(ptr==*Iterator)
		{
			erase(Iterator);
			return;
		}
	}
}
void CVoidVector::DeleteAllElement()
{
	clear();
}
CVoidList::CVoidList()
{
   clear();
}
CVoidList::~CVoidList()
{
   clear();
}
void CVoidList::AddTailElement(void * ptr)
{
	push_back(ptr);
}
void CVoidList::DeleteTailElement()
{
	pop_back();
}
void CVoidList::AddFrontElement(void * ptr)
{
	push_front(ptr);
}
void CVoidList::DeleteFrontElement()
{
	pop_front();
}
bool CVoidList::FindElement(void * ptr)
{
	for(Iterator=begin();Iterator!=end();Iterator++ )
	{
		if(ptr==(*Iterator))
		{
			return true;
		}
	}
	return false;
}
void CVoidList::DeleteElement(void * ptr)
{
    remove(ptr);
}
void CVoidList::DeleteAllElement()
{
    clear();
}