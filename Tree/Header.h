#ifndef _Head_
#define _HEAD_

#include <string>
#include <iostream>
#include <cstdlib>
#include "ALTTree.h"
#include "DRT.h"

using namespace std;

int main(int argv, char *argc);
bool testF();
void queue(unsigned long long start, unsigned long long end, bool add);
bool getInput();
void getCaps(string &input);

#endif
