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
    try{
	    f.Open(0,f_path);
	    f.AddPage (&p, 0);
	    curpage = 0;
	    totalpages = 1;
	    return 1;
    }
    catch(...){
    	return 0;
    }
}

void DBFile::Load (Schema &f_schema, char *loadpath) {
	p.EmptyItOut();
	f.Open(1,loadpath);
	f.GetPage(&p,0);
	curpage = 0;
    totalpages = f.GetLength();
}

int DBFile::Open (char *f_path) {
	try{
		f.Open(1,f_path);
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
	cout << f.GetLength() << "\n" ;
	try{
		f.AddPage(&p, curpage);
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
	// cout << "Done" << "\n";
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
			return 1;
		}
		return 0;
	}
	
}

int DBFile::GetNext (Record &fetchme, CNF &cnf, Record &literal) {
	ComparisonEngine comp;
	while(GetNext(fetchme)){
		if (comp.Compare (&fetchme, &literal, &cnf)){
			return 1;
		}	
	}
	return 0;	
}
