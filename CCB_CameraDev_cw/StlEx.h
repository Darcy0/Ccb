#ifndef STLEX_H
#define STLEX_H
#include <vector>
#include <list>
using namespace std;
class CVoidVector:public vector<void *>
{
public:
	CVoidVector();
	~CVoidVector();
	vector<void *>::iterator Iterator;
protected:
	void AddTailElement(void * ptr);
	void DeleteTailElement();
	void AddFrontElement(void * ptr);
	bool FindElement(void * ptr);
	void DeleteElement(void * ptr);
	void DeleteAllElement();
};
class CVoidList:public list<void *>
{
public:
	CVoidList();
	~CVoidList();
	list<void *>::iterator Iterator;
protected:
	void AddTailElement(void * ptr);
	void DeleteTailElement();
	void AddFrontElement(void * ptr);
	void DeleteFrontElement();
	bool FindElement(void * ptr);
	void DeleteElement(void * ptr);
	void DeleteAllElement();
};
#endif
