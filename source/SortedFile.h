#ifndef SORTEDFILE_H
#define SORTEDFILE_H

#include "DBFile.h"

typedef enum {init, merg } Smode;

typedef struct{
	Pipe *myPipe;
	int runLen;
	OrderMaker *so;
	char * loadpath;
	char * db_path;
	Schema * my_schema;
	Smode flag;
	File *f;
}loadinfo;

class SortedFile : public GenericDBFile {
private:
	static const int pipeSize = 100;
	int runLen;
	OrderMaker so;
	char * db_path;
	Pipe* toBigQ;
	Pipe* fromBigQ;
	BigQ* bq;
	void Merge();

public:
	SortedFile(){}
	~SortedFile(){}
	int Create (char *fpath, fType file_type, void *startup);
	int Open (char *fpath);
	int Close ();

	void Load (Schema &myschema, char *path);

	void MoveFirst ();
	void Add (Record &addme);
	int GetNext (Record &fetchme);
	int GetNext (Record &fetchme, CNF &cnf, Record &literal);
	void Switchmode(mode);

};

#endif