#ifndef _ALTNode_
#define _ALTNode_

#include <string>
#include <cstdlib>
#include "ALTTree.h"

using namespace std;

class ALTTree;
class ALTNode	{
private:
	string key, data;
	ALTNode *parent, *left, *right, *prev, *next, *cprev, *cnext;
	ALTTree *tree;
	unsigned long long subindex;
	bool black;
	mutex m;
	bool add(string k, string d, ALTNode *n, ALTNode *p, bool push);
	void push(string k, string d, ALTNode *n, ALTNode *p);
	void remove(bool verbose);
	unsigned long long getIndex(string k, unsigned long long i);
public:
	ALTNode(string k, string d, ALTTree *t);
	ALTNode(string k, string d, ALTTree *t, ALTNode *pa);
	~ALTNode();
	int ValidNode();
	void add(string k, string d);
	bool a_balance();
	void push(string k, string d);
	void remove();
	bool remove(string k);
	bool r_balance(ALTNode *p, ALTNode *m);
	string search(string k);
	string search(unsigned long long index);
	ALTNode *nodeSearch(string k);
	unsigned long long getIndex(string k);
	void rotate();
	void print();
	void lprint();
	void cprint();
	void rprint();
	void lrprint();
	void crprint();
	void setParent(ALTNode *pa);
	void setLeft(ALTNode *l);
	void setRight(ALTNode *r);
	void setPrev(ALTNode *p);
	void setNext(ALTNode *n);
	void setChronPrev(ALTNode *p);
	void setChronNext(ALTNode *n);
	void setChild(ALTNode *c, ALTNode *n);
	void setRed();
	void setBlack();
	unsigned long long updateSubindex();
	ALTNode *getParent();
	ALTNode *getLeft();
	ALTNode *getRight();
	ALTNode *getPrev();
	ALTNode *getNext();
	ALTNode *getChronPrev();
	ALTNode *getChronNext();
	ALTNode *getOppositeChild(ALTNode *t);
	ALTTree *getTree();
	bool getColor();
	bool getUncleColor();
	bool isDirectGrandchild();
	void setData(string d);
	void setKey(string k);
	void lock();
	void unlock();
	bool tryLock();
	string get();
	string getKey();
	vector<unsigned long long> allStats();
	pair<string, unsigned long long> nodeStats(string k);
	pair<string, unsigned long long> nodeStats(string k, unsigned long long i);
};


#endif
