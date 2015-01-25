#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
#include "DBFile.h"
#include "Defs.h"
#include <iostream>
// stub file .. replace it with your own DBFile.cc

DBFile::DBFile () {
}

	
int DBFile::Create (char *f_path, fType f_type, void *startup) {
    f.Open(0,f_path);
    f.AddPage (&p, 0);
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
	f.AddPage(&p, curpage);
	f.Close();
}

void DBFile::Add (Record &rec) {
	cout << "Inside add" << "\n";
	f.GetPage(&p,curpage);
	cout << "after page" << "\n";
	Schema mySchema ("catalog", "region");
	rec.Print (&mySchema);
	if(!p.Append(&rec)){
		cout << "new page" << "\n";
		f.AddPage(&p, curpage);
		p.EmptyItOut();
		curpage++;totalpages++;
		p.Append(&rec);
	}

	cout << "Done" << "\n";
}

int DBFile::GetNext (Record &fetchme) {
}

int DBFile::GetNext (Record &fetchme, CNF &cnf, Record &literal) {
}
