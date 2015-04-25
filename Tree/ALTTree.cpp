#include <string>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include "ALTTree.h"
#ifndef NAN
	#define NAN 18446744073709551615
#endif

using namespace std;

ALTTree::ALTTree()	{
	Q = new CommandQueue(this,2);
	root = head = tail = chead = ctail = NULL;
}

ALTTree::ALTTree(ALTNode *r)	{
	Q = new CommandQueue(this);
	(root = head = tail = chead = ctail = r)->a_balance(); //sets all pointers to r, then calls balance
}

ALTTree::~ALTTree()	{
	delete Q;
	delete root;
}

int ALTTree::Valid () {
	m.lock();
	int i = 0;
	if (!root)
		i = 1;
	else if (!root->getColor()) 
		i = 0;
	else 
		i = (root->ValidNode() != -1);
	m.unlock();
	return i;
}

void ALTTree::Queue(int cmd, string input)	{
	Q->enqueue(cmd,input);
}

void ALTTree::add(string key, string data)	{
	add(key,data,true);
}

void ALTTree::add(string key, string data, bool verbose)	{
	if (root)
		root->add(key,data);
	else	{
		if (m.try_lock())	{
			(root = head = tail = chead = ctail = new ALTNode(key,data,this))->a_balance(); //sets all pointers to root, then calls balance
			m.unlock();
		}
		else	{
			this_thread::sleep_for(chrono::milliseconds(1));
			add(key,data,verbose);
		}
	}
	if (verbose)
		cout << "One copy of " << key << " has been added" << endl;
}

void ALTTree::push(string key)	{
	if (root)
		root->push(key,"1");
}

void ALTTree::enqueue(string key)	{
	add(key,"1");
}

bool ALTTree::remove(string key)	{
	if (root)
		return root->remove(key);
	else 
		return false;
}

bool ALTTree::undo()	{
	if (ctail)	{
		ctail->remove();
		return true;
	}
	else
		return false;
}

string ALTTree::search(string key)	{
	if (!root)
		return "";
	else
		return root->search(key);
}

string ALTTree::search(unsigned long long index)	{
	if (!root)
		return "";
	else
		return root->search(index);
}

unsigned long long ALTTree::getIndex(string key)	{
	if (root)
		return root->getIndex(key);
	else
		return NAN;
}

string ALTTree::dequeue()	{
	//must remember traceback/traceforward
	return "not yet implemented";
}

string ALTTree::pop()	{
	//must remember traceback/traceforward
	return "not yet implemented";
}

void ALTTree::print()	{
	if (root)
		root->print();
	else
		cout << "Tree is empty." << endl;
}

void ALTTree::lprint()	{
	if (head)
		head->lprint();
	else
		cout << "Tree is empty." << endl;
}

void ALTTree::cprint()	{
	if (chead)
		chead->cprint();
	else
		cout << "Tree is empty." << endl;
}

void ALTTree::rprint()	{
	if (root)
		root->rprint();
	else
		cout << "Tree is empty." << endl;
}

void ALTTree::lrprint()	{
	if (tail)
		tail->lrprint();
	else
		cout << "Tree is empty." << endl;
}

void ALTTree::crprint()	{
	if (ctail)
		ctail->crprint();
	else
		cout << "Tree is empty." << endl;
}

void ALTTree::tprint()	{
	if (root)
		cout << "Not yet implemented." << endl;
	else
		cout << "Tree is empty." << endl;
}

void ALTTree::setRoot(ALTNode *r)	{
	root = r;
}

void ALTTree::setHead(ALTNode *h)	{
	head = h;
}

void ALTTree::setTail(ALTNode *t)	{
	tail = t;
}

void ALTTree::setChronHead(ALTNode *h)	{
	chead = h;
}

void ALTTree::setChronTail(ALTNode *t)	{
	ctail = t;
}

void ALTTree::updateSubindex()	{
	if (root)
		root->updateSubindex();
}

ALTNode *ALTTree::getRoot()	{
	return root;
}

ALTNode *ALTTree::getHead()	{
	return head;
}

ALTNode *ALTTree::getTail()	{
	return tail;
}

ALTNode *ALTTree::getChronHead()	{
	return chead;
}

ALTNode *ALTTree::getChronTail()	{
	return ctail;
}

string ALTTree::operator[](unsigned long long index)	{
	return search(index);
}

unsigned long long ALTTree::operator[](string key)	{
	return getIndex(key);
}

string ALTTree::operator()(unsigned long long index)	{
	//return chron_search(index);
	return "not yet implemented";
}

string ALTTree::operator()(string key)	{
	return search(key);
}

void ALTTree::operator+=(string key)	{
	add(key,"1");
}

bool ALTTree::operator-=(string key)	{
	return remove(key);
}

void ALTTree::operator<(string key)	{
	push(key);
}

vector<unsigned long long> ALTTree::allStats()	{
	if (root)	{
		vector<unsigned long long> a;
		m.lock();
		a = root->allStats();
		m.unlock();
		return a;
	}
	else	{
		vector<unsigned long long> t = vector<unsigned long long>(3);
		t[0] = t[1] = t[2] = 0;
		return t;
	}
}

pair<string, unsigned long long> ALTTree::nodeStats(string k)	{
	if (root)
		return root->nodeStats(k);
	else
		return pair<string, unsigned long long>("",0);
}

unsigned long long ALTTree::size()	{
	return root->updateSubindex();
}

bool ALTTree::isQueueEmpty()	{
	return Q->isEmpty();
}

unsigned long long ALTTree::queueSize()	{
	return Q->size();
}

void ALTTree::startQueue()	{
	Q->start();
}

void ALTTree::pauseQueue()	{
	Q->kill();
}
