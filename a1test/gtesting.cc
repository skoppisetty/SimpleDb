#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "DBFile.h"

extern "C" {
	int yyparse(void);   // defined in y.tab.c
}

using ::testing::AtLeast; 
using ::testing::_;

extern struct AndList *final;

class MockDBfile : public DBFile {
	public:
		MockDBfile() { }
	    virtual ~MockDBfile() { }
		// MOCK_METHOD1(Add, void(Record &addme));
		// MOCK_METHOD2(Load,void(Schema &myschema, char *loadpath));
		// MOCK_METHOD3(Create,int(char *fpath, fType file_type, void *startup));
		// MOCK_METHOD1(Open,int(char *fpath));
		// MOCK_METHOD0(Close,int());
		// MOCK_METHOD0(MoveFirst,void());
		// MOCK_METHOD1(GetNext,int(Record &fetchme));
		// MOCK_METHOD3(GetNext,int(Record &fetchme, CNF &cnf, Record &literal));
};


// make sure that the file path/dir information below is correct
char *dbfile_dir = ""; // dir where binary heap files should be stored
char *tpch_dir ="../DATA/10M/"; // dir where dbgen tpch files (extension *.tbl) can be found
char *catalog_path = "catalog"; // full path of the catalog file

int load_test(DBFile &db, char * fname, Schema &mySchema){
    try{
	    char rpath[100];
	    sprintf (rpath, "%s%s.bin", dbfile_dir , fname);
	    db.Create(rpath,heap,NULL);
	    char tbl_path[100]; // construct path of the tpch flat text file
		sprintf (tbl_path, "%s%s.tbl", tpch_dir, fname);
	    cout << "Loading data from " <<  tbl_path << endl;
	    db.Load(mySchema,tbl_path);
	    cout << "Enter in your CNF: ";
	  	if (yyparse() != 0) {
			cout << "Can't parse your CNF.\n";
			exit (1);
		}

		// suck up the schema from the file
		Schema lineitem ("catalog", "lineitem");

		// grow the CNF expression from the parse tree 
		CNF myComparison;
		Record literal;
		myComparison.GrowFromParseTree (final, &lineitem, literal);
		
		// print out the comparison to the screen
		myComparison.Print ();

		db.MoveFirst ();

		Record temp;

		int counter = 0;
		while (db.GetNext (temp, myComparison, literal) == 1) {
			counter += 1;
			temp.Print (&mySchema);
			if (counter % 10000 == 0) {
				cout << counter << "\n";
			}
		}
		cout << " selected " << counter << " recs \n";
		db.Close ();
	    // db.Open(rpath);
	    // Record temp;
	    // db.MoveFirst();
	    // cout << db.GetNext(temp) << endl;
	    // db.Add(temp);
	    
	    return counter;    	
    }
    catch(exception& e){
    	cout << e.what() << '\n';
	    return 0;
    }


}

// int open_test(DBFile &db){
//     return db.Open(fname);
// }

TEST(DBFILETEST, OpenCreatetest){
	// DBFile db;
	MockDBfile db;
	char * fname;
	string filename = "lineitem";
	fname = (char *)filename.c_str();
	Schema mySchema ("catalog", "lineitem");
	// EXPECT_CALL(db,Load(_,_)).Times(AtLeast(1));
	// EXPECT_CALL(db,GetNext(_,_,_)).Times(6017999);
	EXPECT_EQ(1,load_test(db, fname, mySchema));
	
	// EXPECT_EQ(1,open_test(db));
	// EXPECT_EQ(1,close_test(db));
}