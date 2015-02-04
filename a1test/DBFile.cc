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

DBFile::~DBFile () {
}
	
int DBFile::Create (char *f_path, fType f_type, void *startup) {
    try{
	    f.Open(0,f_path);
	    f.AddPage (&p, 0);
	    curpage = 0;
	    // status = DBFILE_W;
	    totalpages = 1;
	    return 1;
    }
    catch(...){
    	return 0;
    }
}

void DBFile::Load (Schema &f_schema, char *loadpath) {
    FILE *tableFile = fopen (loadpath, "r");
    Record temp;
    int counter = 0;
    while (temp.SuckNextRecord (&f_schema, tableFile ) == 1) {
		counter++;
		// if (counter % 10000 == 0) {
		// 	cerr << counter << "\n";
		// }
		Add(temp);
    }
    cout << "Number of records loaded: " << counter << endl;
    f.AddPage(&p, curpage);
}

int DBFile::Open (char *f_path) {
	try{
		p.EmptyItOut();
		f.Open(1,f_path);
		f.GetPage(&p,0);
		curpage = 0;
		// status = DBFILE_R;
		totalpages = f.GetLength();
		return 1;
	}
	catch(...){
		return 0;
	}
	
}

void DBFile::MoveFirst () {
	p.EmptyItOut();
	f.GetPage(&p,0);
}

int DBFile::Close () {
	// cout << f.GetLength() << "\n" ;
	try{
		p.EmptyItOut();
		f.Close();
		return 1;
	}
	catch(...){
		return 0;
	}


}

void DBFile::Add (Record &rec) {
	// Schema mySchema ("catalog", "region");p.Append(&rec)
	// rec.Print (&mySchema);
	if(!p.Append(&rec)){
		// cout << "new page" << "\n";
		f.AddPage(&p, curpage);
		p.EmptyItOut();
		curpage++;totalpages++;
		p.Append(&rec);
	}
	cout << "Add" << "\n";
}

int DBFile::GetNext (Record &fetchme) {
	//cout << curpage << endl;
	if(p.GetFirst(&fetchme)){
		return 1;
	}
	else{
		//cout << "pagechange" << curpage << " " << totalpages << endl;
		if(curpage < totalpages - 2){
			curpage++;
			f.GetPage(&p,curpage);
			if(p.GetFirst(&fetchme)){
				return 1;
			}
		}
		return 0;
	}
	
}

int DBFile::GetNext (Record &fetchme, CNF &cnf, Record &literal) {
	ComparisonEngine comp;
	// cout << f.GetLength() << endl;
	while(GetNext(fetchme)){
		// cout << "status" << endl;
		if (comp.Compare (&fetchme, &literal, &cnf)){
			return 1;
		}	
	}
	cout << "end" << endl;
	return 0;	
}