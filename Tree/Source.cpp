#include <string>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include "Header.h"

using namespace std;

int init = 1000;

ALTTree T; 

int main(int argv, char *argc)	{
	time_t t = time(NULL);
	while(testF())
		if (!T.Valid())	{
			cout << "Tree Invalid" << endl;
			system ("pause");
		}
	cout << difftime(time(NULL),t) << " seconds." << endl;
	system ("pause");
}

bool testF()	{
	cout << "Populating initial queue ... " << endl;
	string tmp = "        ";
	T.pauseQueue();
	auto start = chrono::high_resolution_clock::now();

	queue(0,init,true);
	//queue(1,init,false);

	chrono::duration<double> elapsed = chrono::high_resolution_clock::now() - start;
	T.startQueue();
	cout << endl << "Queue population took " << elapsed.count() << " seconds for " << init << " nodes." << endl;
	while(!T.isQueueEmpty())	{
		cout << (1.0 - T.queueSize().convert_to<double>() / init) * 100 << "% complete. " << endl;
		this_thread::sleep_for(chrono::seconds(1));
	}
	cout << "Tree population compelete. Total time " << ((chrono::duration<double>)(chrono::high_resolution_clock::now() - start)).count() << " seconds." << endl;
	while(getInput())
		;

	queue(init,18446744073709551615,true);

	return false;
}

void queue(unsigned long long i, unsigned long long end, bool add)	{
	string tmp = "        ";
	for (; i < end; i++)	{
		tmp[0] = i % 256;
		tmp[1] = (i / 256) % 256;
		tmp[2] = (i / 65536) % 256;
		tmp[3] = (i / 16777216) % 256;
		tmp[4] = (i / 4294967296) % 256;
		tmp[5] = (i / 1099511627776) % 256;
		tmp[6] = (i / 281474976710656) % 256;
		tmp[7] = (i / 72057594037927936) % 256;
		add ? T.Queue(1, tmp + ",1") : T.Queue(0, tmp);
	}
}

bool getInput()	{
	string input;
	bool c = true;
	getline(cin,input);
	getCaps(input);
	if (input.substr(0,3) == "ADD" && input.length() > 4)
		T+=input.substr(4);
	else if (input.substr(0,6) == "REMOVE" && input.length() > 7)
		T-=input.substr(7);
	else if (input.substr(0,4) == "PUSH" && input.length() > 5)
		T<input.substr(5);
	else if (input == "POP")
		cout << T.pop() << endl;
	else if (input.substr(0,6) == "QUEUE" && input.length() > 7)
		T.enqueue(input.substr(7));
	else if (input == "DEQUEUE")
		cout << T.dequeue();
	else if (input == "UNDO")
		T.undo();
	else if (input == "COUNT")
		cout << T.size() << " nodes in the tree." << endl;
	else if (input == "STATS")	{
		vector<unsigned long long> stat = T.allStats();
		cout << "There are " << stat[0] << " nodes in the tree." << endl
			<< "The shortest path is " << stat[1] << " nodes." << endl
			<< "The longest path is " << stat[2] << " nodes." << endl;
	}
	else if (input == "VALIDATE")	{
		if (T.Valid())
			cout << "The tree is Red-Black valid." << endl;
		else	{
			cout << "This tree is invalid." << endl;
			system ("pause");
		}
	}
	else if (input == "TPRINT")
		T.tprint();
	else if (input == "PRINT")
		T.print();
	else if (input == "LPRINT")
		T.lprint();
	else if (input == "CPRINT")
		T.cprint();
	else if (input == "RPRINT")
		T.rprint();
	else if (input == "LRPRINT")
		T.lrprint();
	else if (input == "CRPRINT")
		T.crprint();
	else if (input.substr(0,6) == "APRINT")	{
		string a = T[atol(input.substr(7).c_str())];
		if (a != "")
			cout << a << endl;
		else
			cout << "Item not found" << endl;
	}
	else if (input.substr(0,7) == "CAPRINT")	{
		string a = T(atol(input.substr(8).c_str()));
		if (a != "")
			cout << a << endl;
		else
			cout << "Item not found" << endl;
	}
	else if (input == "EXIT")
		c = false;
	else
		cout << "unknown command or too few arguments" << endl;
	return c;
}

void getCaps(string &input)	{
	for (int i = 0; i < (int)input.length(); i++)
		if (input[i] >= 'a' && input[i] <= 'z')
			input[i] += 'A' - 'a';
}
