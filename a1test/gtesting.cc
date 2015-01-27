#include "gtest/gtest.h"
#include "DBFile.h"

class DBFiletest{
private:
	DBFile result;
	char *fname;
public:
	DBFiletest(){

	}
	int create_test();
	int open_test();
};

int DBFiletest::create_test(){
	string filename = "./results.txt";
	fname = (char *)filename.c_str();
    return result.Create(fname,heap,NULL);
}

int DBFiletest::open_test(){
    return result.Open(fname);
}

TEST(DBFILETEST, OpenCreatetest){
	DBFiletest db;
	EXPECT_EQ(1,db.create_test());
	EXPECT_EQ(1,db.open_test());
}