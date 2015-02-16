#ifndef BIGQ_H
#define BIGQ_H
#include <pthread.h>
#include <iostream>

#include "Pipe.h"
#include "File.h"
#include "Record.h"
#include "ComparisonEngine.h"


using namespace std;

class BigQ {
	void Sorted_insert(Record &,int, int, OrderMaker &,int);
	void insert_at_offset(Record *, int , int );
	Page p;
	File f;
	int curpage = 0;
	int currlength = 0;
	OrderMaker sortorder;
public:

	BigQ (Pipe &in, Pipe &out, OrderMaker &sortorder, int runlen);
	~BigQ ();
};

#endif
