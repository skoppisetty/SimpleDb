#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
#include "DBFile.h"
#include "Defs.h"

// stub file .. replace it with your own DBFile.cc

DBFile::DBFile () {
}

	
int DBFile::Create (char *f_path, fType f_type, void *startup) {
    f.Open(0,f_path);
    Page sample;
    f.AddPage (&sample, 0);
    curpage = 0;
    totalpages = 1;
}

void DBFile::Load (Schema &f_schema, char *loadpath) {
}

int DBFile::Open (char *f_path) {
	f.Open(1,f_path);
}

void DBFile::MoveFirst () {
}

int DBFile::Close () {
	f.Close();
}

void DBFile::Add (Record &rec) {
	Page p;
	f.GetPage(&p,curpage);
	if(!p.Append(&rec)){
		Page sample;
		curpage++;totalpages++;
		f.AddPage(&sample, curpage);
		sample.Append(&rec);
	}
}

int DBFile::GetNext (Record &fetchme) {
}

int DBFile::GetNext (Record &fetchme, CNF &cnf, Record &literal) {
}
