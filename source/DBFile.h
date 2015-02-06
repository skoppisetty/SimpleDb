#ifndef DBFILE_H
#define DBFILE_H

#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
#include "gmock/gmock.h"

typedef enum {heap, sorted, tree} fType;

typedef enum {DBFILE_R, DBFILE_W} mode;

// stub DBFile header..replace it with your own DBFile.h 

class DBFile {
private:
	File f;
	Page p;
	int curpage;
	mode status;
	int totalpages;
	void Switchmode(mode);

public:
	DBFile (); 
	virtual ~DBFile();
	virtual int Create (char *fpath, fType file_type, void *startup);
	virtual int Open (char *fpath);
	virtual int Close ();

	virtual void Load (Schema &myschema, char *loadpath);

	virtual void MoveFirst ();
	virtual void Add (Record &addme);
	virtual int GetNext (Record &fetchme);
	virtual int GetNext (Record &fetchme, CNF &cnf, Record &literal);

};
#endif
