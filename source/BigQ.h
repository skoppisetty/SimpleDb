#ifndef BIGQ_H
#define BIGQ_H
#include <pthread.h>
#include <iostream>
#include <queue>
#include <vector>
#include <algorithm>
#include "Pipe.h"
#include "File.h"
#include "Record.h"

using namespace std;

class BigQ {
	File f;
	Page p;
	int curpage;
	int totalpages;
	int runlength;
	int curSizeInBytes;
	int Create();
	int savelist(vector<Record*>);
	int GetNext(Record & );
public:
	BigQ (Pipe &in, Pipe &out, OrderMaker &sortorder, int runlen);
	~BigQ ();
};

#endif
