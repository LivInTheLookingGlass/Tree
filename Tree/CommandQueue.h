#ifndef _CQUEUE_
#define _CQUEUE_

#include <string>
#include <cstdlib>
#include <boost/multiprecision/cpp_int.hpp>
#include "ALTTree.h"

using namespace std;
namespace bst = boost::multiprecision;

const int rm = 0, ad = 1;

class ALTTree;
class CNode;
class CommandQueue	{
private:
	mutex m;
	vector<thread> pool;
	CNode *head, *tail;
	ALTTree *t;
	bst::cpp_int sz;
	volatile bool alive;
public:
	CommandQueue(ALTTree *tree);
	CommandQueue(ALTTree *tree, int threads);
	~CommandQueue();
	void enqueue(int cmd, string input);
	CNode *dequeue();
	bool isEmpty();
	bst::cpp_int size();
	void process();
	void start();
	void kill();
};

class CNode	{
private:
	int cmd;
	string input;
	CNode *next, *prev;
public:
	CNode(int cmd, string input, CNode *next);
	~CNode();
	int getCMD();
	string getInput();
	CNode *getNext();
	void setNext(CNode *n);
	CNode *getLast();
	CNode *getPrev();
	void setPrev(CNode *p);
};

#endif
