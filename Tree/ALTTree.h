#ifndef _ALTTree_
#define _ALTTree_

#include <string>
#include <cstdlib>
#include <thread>
#include <vector>
#include <mutex>
#include <boost/multiprecision/cpp_int.hpp>
#include "ALTNode.h"
#define _experimental_ 1
#if _experimental_ == 1
	#include "CommandQueue.h"
#endif

using namespace std;
namespace bst = boost::multiprecision;

class ALTNode;
class CommandQueue;
class ALTTree	{
private:
#if _experimental_ == 1
	CommandQueue *Q;
#endif
	ALTNode *root, *head, *tail, *chead, *ctail;
	string search(unsigned long long index);
	unsigned long long sz;
	bst::cpp_int szz;
	mutex m;
public:
	ALTTree();
	ALTTree(ALTNode *r);
	~ALTTree();
	int Valid();
	void add(string key, string data);
	void add(string key, string data, bool verbose);
	void push(string key);
	void enqueue(string key);
	bool remove(string key);
	bool remove(string key, bool verbose);
	bool undo();
	string search(string key);
	unsigned long long getIndex(string key);
	string dequeue();
	string pop();
	void print();
	void lprint();
	void cprint();
	void rprint();
	void lrprint();
	void crprint();
	void tprint();
	void setRoot(ALTNode *r);
	void setHead(ALTNode *h);
	void setTail(ALTNode *t);
	void setChronHead(ALTNode *h);
	void setChronTail(ALTNode *t);
	void updateSubindex();
	ALTNode *getRoot();
	ALTNode *getHead();
	ALTNode *getTail();
	ALTNode *getChronHead();
	ALTNode *getChronTail();
	string operator[](unsigned long long index);
	unsigned long long operator[](string key);
	string operator()(unsigned long long index);
	string operator()(string key);
	void operator+=(string key);
	bool operator-=(string key);
	void operator<(string key);
	vector<unsigned long long> allStats();
	pair<string, unsigned long long> nodeStats(string k);
	unsigned long long size();
#if _experimental_ == 1
	void Queue(int cmd, string input);
	bool isQueueEmpty();
	bst::cpp_int queueSize();
	void startQueue();
	void pauseQueue();
#endif
};

#endif
