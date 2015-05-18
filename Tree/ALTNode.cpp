#include <string>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <cstdarg>
#include "ALTNode.h"
#ifndef NAN
	#define NAN 18446744073709551615
#endif

using namespace std;

ALTNode::ALTNode(string k, string d, ALTTree *t)	{
	key = k;
	data = d;
	tree = t;
	parent = left = right = prev = next = cprev = cnext = NULL;
	subindex = 0;
	black = false;
	owned = NULL;
}

ALTNode::ALTNode(string k, string d, ALTTree *t, ALTNode *pa)	{
	key = k;
	data = d;
	tree = t;
	parent = pa;
	left = right = prev = next = cprev = cnext = NULL;
	subindex = 0;
	black = false;
	owned = NULL;
}

//could probably be multithreaded
ALTNode::~ALTNode()	{
	try	{
		delete left;
		delete right;
		unlock();
	}
	catch (exception e)	{
		cout << e.what() << endl;
	}
}

//could probably be multithreaded
int ALTNode::ValidNode () {
	int lc = 0, rc = 0, r;

	m.lock();
	if (!black && parent && !parent->getColor())
		r= -1;
	else if (left && left->getParent() != this) 
		r= -1;
	else if (left && left->getKey() >= key) 
		r= -1;
	else if (right && right->getParent() != this) 
		r= -1;
	else if (right && right->getKey() <= key) 
		r= -1;
	else if (left)
		lc = left->ValidNode();
	else 
		lc = 0;

	if (lc==-1) 
		r= -1;
	else if (right)
		rc = right->ValidNode();
	else
		rc = 0;
	
	if (rc==-1) 
		r= -1;
	else if (lc != rc) 
		r = -1;
	else if (black)
		r = lc+1;
	else
		r = lc;
	m.unlock();

	return r;
}

void ALTNode::add(string k, string d)	{
	add(k,d,false);
}

bool ALTNode::add(string k, string d, bool push)	{
	bool balanced = false, a, b;
	//locking logic

	try	{
		while (!tryLock())
			this_thread::sleep_for(chrono::milliseconds(25));
		a = k < key;
		b = k > key;
	}
	catch	(exception e)	{
		cout << e.what() << endl;
		if (parent)
			return parent->add(k,d,push);
		else if (tree)
			tree->add(k,d);
		else
			return false;
	}
	if ((a && !left) || (b && !right))	{
		ALTNode *tmp = new ALTNode(k,d,tree,this);
		try	{
			if (!push)	{
				tmp->setChronPrev(tree->getChronTail());
				tmp->getChronPrev()->setChronNext(tmp);
				tree->setChronTail(tmp);
			}
			else	{
				tmp->setChronNext(tree->getChronHead());
				tmp->getChronNext()->setChronPrev(tmp);
				tree->setChronHead(tmp);
			}
			if (a)	{
				tmp->setNext(this);
				tmp->setPrev(prev);
				if (prev)
					prev->setNext(tmp);
				if (this == tree->getHead())
					tree->setHead(tmp);
				prev = left = tmp;
			}
			else if (b)	{
				tmp->setNext(next);
				tmp->setPrev(this);
				if (next)
					next->setPrev(tmp);
				if (this == tree->getTail())
					tree->setTail(tmp);
				next = right = tmp;
			}
			balanced = tmp->a_balance();
			unlock();
		}
		catch (exception e)	{
			cout << e.what() << endl;
			system ("pause");
		}
	}
	else	{
		unlock();
		//end locking logic
		if (a && left)	{
			balanced = left->add(k,d,push);
		}
		else if (b && right)	{
			balanced = right->add(k,d,push);
		}
		else if (!a && !b)	{ //multithreading might belong in this case
			lock();
			cprev = tree->getChronTail();
			cprev->setChronNext(this);
			tree->setChronTail(this);
			data = d;
			unlock();
			return true;
		}
	}
	return balanced;
}

//could probably have partial multithreading
bool ALTNode::a_balance()	{
	if (!parent || tree->getRoot() == this)	{
		black = true;
		return false;
	}
	else if (parent->getColor())
		return false;
	else if (!getUncleColor())	{ //multithreading would belong in this case
		parent->getParent()->getOppositeChild(parent)->setBlack();
		parent->setBlack();
		parent->getParent()->setRed();
		return parent->getParent()->a_balance();
	}
	else if (isDirectGrandchild())	{
		ALTNode *gp = NULL;
		try	{
			(gp = parent->getParent());//->lock();
			parent->rotate();
		}
		catch (exception e)	{
			cout << e.what() << endl;
		}
		try	{
			if (parent)	{
				try	{
					parent->unlock();
					/*if (gp)
						gp->unlock();*/		//This line causes trouble (occasional unlock of unowned mutex). Why?
				}
				catch (exception e)	{
					cout << e.what() << endl;
				}
				if (parent->getParent())
					parent->getParent()->updateSubindex();
				else
					parent->updateSubindex();
			}
			else
				updateSubindex();
		}
		catch (exception e)	{
			cout << e.what() << endl;
		}
		return true;
	}
	else	{
		ALTNode *gp = NULL;
		if (parent && parent->getParent())
			(gp = parent->getParent())->lock();
		lock();
		try	{
			rotate();
		}
		catch (exception e)	{
			cout << e.what() << endl;
		}
		try	{
			rotate();
		}
		catch (exception e)	{
			cout << e.what() << endl;
		}
		try	{
			if (gp)
				gp->unlock();
			unlock();
			if (parent)
				if (parent->getParent())
					parent->getParent()->updateSubindex();
				else
					parent->updateSubindex();
			else
				updateSubindex();
		}
		catch (exception e)	{
			cout << e.what() << endl;
		}
		return true;
	}
	return false;
}

void ALTNode::push(string k, string d)	{
	add(k,d,true);
}

bool ALTNode::remove(string k)	{
	bool r = false;
	if (k > key)	{
		if (right)
			r = right->remove(k);
	}
	else if (k < key)	{
		if (left)	{
			r = left->remove(k);
			if (r)
				subindex--;
		}
	}
	else	{
		lock_guard<mutex> lock(m);
		remove();
		r = true;
	}
	return r;
}

bool ALTNode::remove(string k, bool verbose)	{
	bool r = false;
	if (k > key)	{
		if (right)
			r = right->remove(k,verbose);
	}
	else if (k < key)	{
		if (left)	{
			r = left->remove(k,verbose);
			if (r)
				subindex--;
		}
	}
	else	{
		remove(verbose);
		r = true;
	}
	return r;
}

void ALTNode::remove()	{
	remove(true);
}

//could probably be multithreaded
void ALTNode::remove(bool verbose)	{
	if (verbose)
		cout << "one copy of " << key << " has been removed" << endl;
	if (!left || !right)	{
		//(n)one child case
		ALTNode *p = parent, *m = (left) ? left : right;
		bool c = black;
		
		//locking logic
		try	{
			lock();
		}
		catch (exception e)	{
			cout << e.what() << endl;
		}
		try	{
			if (parent) parent->lock();
		}
		catch (exception e)	{
			cout << e.what() << endl;
		}
		try	{
			if (m) m->lock();
		}
		catch (exception e)	{
			cout << e.what() << endl;
		}
		if (parent)
			parent->setChild(this,m);
		else	
			tree->setRoot(m);

		if (m)	m->setParent(parent);

		if (prev)	prev->setNext(next);
		if (next)	next->setPrev(prev);

		if (tree->getTail() == this)	tree->setTail(prev);
		if (tree->getHead() == this)	tree->setHead(next);

		if (cprev)	cprev->setChronNext(cnext);
		if (cnext)	cnext->setChronPrev(cprev);

		if (tree->getChronTail() == this)	tree->setChronTail(cprev);
		if (tree->getChronHead() == this)	tree->setChronHead(cnext);

		left = NULL;
		right = NULL;

		//unlocking logic
		if (parent) parent->unlock();
		if (m) m->unlock();
		unlock();
		try	{
			//delete this;
			//cout << "A memory leak has occurred (for testing purposes, of course)." << endl;
		}
		catch (exception e)	{
			cout << e.what() << endl;
		}
		if (c)	r_balance(p,m);
	}
	//two child case
	else	{
		//locking logic
		ALTNode *k = (next) ? next : prev;
		try	{
			lock();
		}
		catch (exception e)	{
			cout << e.what() << endl;
		}
		try	{
			if (parent) parent->lock();
		}
		catch (exception e)	{
			cout << e.what() << endl;
		}
		try	{
			//ALTNode *k = (next) ? next : prev;
			if (k && k != parent) k->lock();
		}
		catch (exception e)	{
			cout << e.what() << endl;
		}

		if (cprev)	cprev->setChronNext(cnext);
		if (cnext)	cnext->setChronPrev(cprev);
		data = k->get();
		key = k->getKey();
		if (k->getNext())	k->getNext()->setPrev(this);
		cnext = k->getChronNext();
		cprev = k->getChronPrev();
		k->setChronNext(NULL);
		k->setChronPrev(NULL);
		if (cnext)	cnext->setChronPrev(this);
		if (cprev)	cprev->setChronNext(this);
		if (k == tree->getHead())	tree->setHead(this);
		if (k == tree->getTail())	tree->setTail(this);
		if (k == tree->getChronHead())	tree->setChronHead(this);
		if (k == tree->getChronTail())	tree->setChronTail(this);

		//unlocking logic
		if (parent) parent->unlock();
		if (k) k->unlock();
		unlock();

		k->remove(false);
	}
}

bool ALTNode::r_balance(ALTNode *p, ALTNode *m)	{
	if (m && !m->getColor())	{ //rule 4
		m->setBlack();
		return true;
	}
	else if (!p) //rule 5
		return true;
	else	{
		ALTNode *w = p->getOppositeChild(m);//rule 6
		if (w && !w->getColor())	{ //rule 7
			w->rotate();
			if (w->getParent())
				if (w->getParent()->getParent())
					w->getParent()->getParent()->updateSubindex();
				else
					w->getParent()->updateSubindex();
			else
				w->updateSubindex();
			return r_balance(p,m);
		}
		else if (w && (!w->getLeft() || w->getLeft()->getColor()) && (!w->getRight() || w->getRight()->getColor()))	{ //rule 8
			w->setRed();
			m = p;
			p = p->getParent();
			return r_balance(p,m);
		}
		else if (w && (p->getLeft() == w && w->getLeft() && !w->getLeft()->getColor() || p->getRight() == w && w->getRight() && !w->getRight()->getColor()))	{//rule 9
			if (p->getLeft() == w)	
				w->getLeft()->setBlack();
			else	
				w->getRight()->setBlack();
			w->rotate();
			if (w->getParent())
				if (w->getParent()->getParent())
					w->getParent()->getParent()->updateSubindex();
				else
					w->getParent()->updateSubindex();
			else
				w->updateSubindex();
			return true;
		}
		else if (w)	{ //rule 10
			ALTNode *z = (p->getLeft() == w) ? w->getRight() : w->getLeft();
			z->rotate();
			z->rotate();
			if (z->getParent())
				if (z->getParent()->getParent())
					z->getParent()->getParent()->updateSubindex();
				else
					z->getParent()->updateSubindex();
			else
				z->updateSubindex();
			w->setBlack();
			return true;
		}
	}
}

string ALTNode::search(string k)	{
	if (k < key && left)
		return left->search(k);
	else if (k > key && right)
		return right->search(k);
	else if (key == k)
		return data;
	else
		return "";
}

string ALTNode::search(unsigned long long index)	{
	if (subindex == index)
		return key;
	else if (left && subindex > index)
		return left->search(index);
	else if (right && subindex < index)
		return right->search(index-subindex-1);
	else
		return "";
}

ALTNode *ALTNode::nodeSearch(string k)	{
	if (k < key && left)
		return left->nodeSearch(k);
	else if (k > key && right)
		return right->nodeSearch(k);
	else if (key == k)
		return this;
	else
		return nullptr;
}

unsigned long long ALTNode::getIndex(string k)	{
	return getIndex(k,0);
}

unsigned long long ALTNode::getIndex(string k, unsigned long long i)	{
	if (k < key && left)
		return left->getIndex(k,i);
	else if (k > key && right)
		return right->getIndex(k,i+subindex);
	else if (key == k)
		return i+subindex;
	else
		return NAN;
}

//could probably have partial multithreading
void ALTNode::rotate()	{
	ALTNode *Gp = parent->getParent(),
			*A = parent,
			*C = this,
			*Z = NULL;
	bool r = false;
	if (A->getRight() == C)
		Z = C->getLeft();
	else	
		Z = C->getRight();
	C->setParent(Gp);
	A->setParent(C);
	if (A->getRight() == C)
		C->setLeft(A);
	else
		C->setRight(A);
	A->setChild(C,Z);
	if (Z)
		Z->setParent(A);
	if (A == tree->getRoot())
		tree->setRoot(C);
	else
		Gp->setChild(A,C);
	bool tmp = A->getColor();
	if (black)
		A->setBlack();
	else
		A->setRed();
	black = tmp;
}

void ALTNode::print()	{
	if (left)
		left->print();
	cout << key << "	:	" << data << "	:	" << subindex << endl;
	if (right)
		right->print();
}

void ALTNode::lprint()	{
	cout << key << "	:	" << data << "	:	" << subindex << endl;
	if (next)
		next->lprint();
}

void ALTNode::cprint()	{
	cout << key << "	:	" << data << "	:	" << subindex << endl;
	if (cnext)
		cnext->cprint();
}

void ALTNode::rprint()	{
	if (right)
		right->rprint();
	cout << key << "	:	" << data << "	:	" << subindex << endl;
	if (left)
		left->rprint();
}

void ALTNode::lrprint()	{
	cout << key << "	:	" << data << "	:	" << subindex << endl;
	if (prev)
		prev->lrprint();
}

void ALTNode::crprint()	{
	cout << key << "	:	" << data << "	:	" << subindex << endl;
	if (cprev)
		cprev->crprint();
}

void ALTNode::setParent(ALTNode *p)	{
	parent = p;
}

void ALTNode::setLeft(ALTNode *l)	{
	left = l;
}

void ALTNode::setRight(ALTNode *r)	{
	right = r;
}

void ALTNode::setPrev(ALTNode *p)	{
	prev = p;
}

void ALTNode::setNext(ALTNode *n)	{
	next = n;
}

void ALTNode::setChronNext(ALTNode *n)	{
	cnext = n;
}

void ALTNode::setChronPrev(ALTNode *p)	{
	cprev = p;
}

void ALTNode::setChild(ALTNode *c, ALTNode *n)	{
	if (left == c)
		left = n;
	else
		right = n;
}

void ALTNode::setRed()	{
	black = false;
}

void ALTNode::setBlack()	{
	black = true;
}

unsigned long long ALTNode::updateSubindex()	{
	unsigned long long l = 0, r = 0;
	//while (!m.try_lock() && (left || right))
	//	this_thread::sleep_for(chrono::milliseconds(1));
	if (left)
		l = left->updateSubindex();
	if (right)
		r = right->updateSubindex();
	subindex = l;
	//unlock();
	return l + r + 1;
}

ALTNode *ALTNode::getParent()	{
	return parent;
}

ALTNode *ALTNode::getLeft()	{
	return left;
}

ALTNode *ALTNode::getRight()	{
	return right;
}

ALTNode *ALTNode::getPrev()	{
	return prev;
}

ALTNode *ALTNode::getNext()	{
	return next;
}

ALTNode *ALTNode::getChronPrev()	{
	return cprev;
}

ALTNode *ALTNode::getChronNext()	{
	return cnext;
}

ALTNode *ALTNode::getOppositeChild(ALTNode *t)	{
	if (t && (!left || !right))
		return NULL;
	else if (left == t)
		return right;
	else
		return left;
}

bool ALTNode::getColor()	{
	return black;
}

bool ALTNode::getUncleColor()	{
	if (parent)
		if (parent->getParent())
			if (parent->getParent()->getOppositeChild(parent))
				return parent->getParent()->getOppositeChild(parent)->getColor();
	return true;
}

bool ALTNode::isDirectGrandchild()	{
	bool a = false;
	if (parent)
		if (parent->getParent())	{
			if (parent->getParent()->getLeft())
				a = parent->getParent()->getLeft()->getLeft() == this;
			if (parent->getParent()->getRight() && !a)
				a = parent->getParent()->getRight()->getRight() == this;
		}
	return a;
}

void ALTNode::setData(string d)	{
	data = d;
}

void ALTNode::setKey(string k)	{
	key = k;
}

string ALTNode::get()	{
	return data;
}

string ALTNode::getKey()	{
	return key;
}

//is multithreaded
vector<unsigned long long> ALTNode::allStats()	{
	vector<unsigned long long> r = vector<unsigned long long>(3), l = vector<unsigned long long>(3), f = vector<unsigned long long>(3);
	l[0] = r[0] = l[1] = r[1] = l[2] = r[2] = 0;

	m.lock();
	if (left)
		l = left->allStats();
	if (right)
		r = right->allStats();
	m.unlock();

	f[0] = l[0] + r[0] + 1;
	f[1] = ((l[1] < r[1]) ? l[1] : r[1]) + 1;
	f[2] = ((l[2] > r[2]) ? l[2] : r[2]) + 1;
	return f;
}

pair<string, unsigned long long> ALTNode::nodeStats(string k)	{
	return nodeStats(k,0);
}

pair<string, unsigned long long> ALTNode::nodeStats(string k, unsigned long long i)	{
	if (k < key && left)	{
		return left->nodeStats(k,i);
	}
	else if (k > key && right)
		return right->nodeStats(k,i+subindex);
	else if (key == k)
		return pair<string, unsigned long long>(data,i+subindex);
	else
		return pair<string, unsigned long long>("",NAN);
}

vector<ALTNode*> ALTNode::vectorize()	{
	va_list ap;
	va_start(ap);
	vector<ALTNode*> n (1);
	ALTNode *t = va_arg(ap,ALTNode*);
	while(t)	{
		n.push_back(t);
		t = va_arg(ap,ALTNode*);
	}
	return n;
}

void ALTNode::safeLock(vector<ALTNode*> n)	{
	for (unsigned int i = 0; i < n.size(); i++)	{
		if (!n[i]->tryLock())	{
			for (unsigned int j = i - 1; j >= 0; j++)
				n[j]->unlock();
			i = -1;
		}
	}
}

void ALTNode::lock()	{
	//owned = this_thread::get_id().hash();
	m.lock();
}

void ALTNode::unlock()	{
	try	{
		m.try_lock();	
		m.unlock();
	}
	catch (exception e)	{
		cout << "unlock error: " << e.what() << endl;
	}
}

bool ALTNode::tryLock()	{
	return m.try_lock();
}
