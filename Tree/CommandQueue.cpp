#include <string>
#include <cstdlib>
#include <iostream>
#include <thread>
#include "CommandQueue.h"

using namespace std;

CommandQueue::CommandQueue(ALTTree *tree)	{
	pool = vector<thread>(thread::hardware_concurrency());
	t = tree;
	for (int i = 0; i < pool.size(); i++)	{
		pool[i] = thread([this](){process();});
		cout << "Thread " << i << " initialized on id " << pool[i].get_id() << endl;
	}
	head = tail = NULL;
	alive = true;
	sz = 0;
}

CommandQueue::CommandQueue(ALTTree *tree, int threads)	{
	pool = vector<thread>(threads);
	t = tree;
	for (int i = 0; i < pool.size(); i++)	{
		pool[i] = thread([this](){process();});
		cout << "Thread " << i << " initialized on id " << pool[i].get_id() << endl;
	}
	head = tail = NULL;
	alive = true;
	sz = 0;
}

CommandQueue::~CommandQueue()	{
	alive = false;
	//for (CNode *tmp = head, *n = (head ? head->getNext() : NULL); tmp; tmp = n, n = (n ? n->getNext() : NULL))
		//delete tmp;
	//for (CNode *tmp = head, *p = (head ? head->getPrev() : NULL); tmp; tmp = p, p = (p ? p->getPrev() : NULL))
		//delete tmp;
        while (head) head->dequeue();
	//delete head;
}

void CommandQueue::enqueue(int cmd, string input)	{
	m.lock();
	tail = new CNode(cmd, input, tail);
	if (!head)
		head = tail;
	++sz;
	m.unlock();
}

CNode *CommandQueue::dequeue()	{
	CNode *tmp;
	m.lock();
	if (tmp = head)	{ //checks to see if head is NULL while setting tmp
		--sz;
		if (tmp != tail)	//for some reason if (head) wasn't working properly. This is
			head = head->getPrev();
		else 
			head = tail = NULL;
	}
	m.unlock();
	return tmp;
}

bool CommandQueue::isEmpty()	{
	return !head;
}

bst::cpp_int CommandQueue::size()	{
	bst::cpp_int i;
	m.lock();
	i = sz;
	m.unlock();
	return i;
}

void CommandQueue::process()	{
	try	{
		int i = 0;
		this_thread::sleep_for(chrono::seconds(3));
		cout << "Thread awake." << endl;
		while (alive)	{
			CNode *input = dequeue();
			if (input)	{
				if (input->getCMD() == rm)	
					t->remove(input->getInput());
				else if (input->getCMD() == ad)	{
					string k, d, in = input->getInput();
					for (int i = in.length() - 1; i >= 0; i--)	{
						if (in[i] == ',')	{
							k = in.substr(0,i);
							d = in.substr(i+1);
							break;
						}
					}
					t->add(k,d,false);
				}
			}
			else
				this_thread::sleep_for(chrono::microseconds(250));
			if (input)	{
				input->setNext(NULL);
				delete input;
			}
		}
		while (!alive)
			this_thread::sleep_for(chrono::milliseconds(10));
		process();
	}
	catch (exception e)	{
		cout << e.what() << endl;
		system ("pause");
	}
}

void CommandQueue::kill()	{
	alive = false;
}

void CommandQueue::start()	{
	alive = true;
}

CNode::CNode(int c, string i, CNode *n)	{
	cmd = c;
	input = i;
	next = n;
	if (next)
		next->setPrev(this);
}

CNode::~CNode()	{
	delete next;
}

int CNode::getCMD()	{
	return cmd;
}

string CNode::getInput()	{
	return input;
}

CNode *CNode::getNext()	{
	return next;
}

void CNode::setNext(CNode *n)	{
	next = n;
}

CNode *CNode::getLast()	{
	if (next)
		return next->getLast();
	else
		return this;
}

CNode *CNode::getPrev()	{
	return prev;
}

void CNode::setPrev(CNode *p)	{
	prev = p;
}
