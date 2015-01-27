#include "gtest/gtest.h"
#include "DBFile.h"

class DBFiletest{
private:
	DBFile result;
	char* fname = "./results.txt";
public:
	DBFiletest(){

	}
	int create_test();
	int open_test();
};

int DBFiletest::create_test(){
	// fname = find_first_of("./results.txt")
    result.Create(fname,heap,NULL);
    return result.Open(fname);
}

int DBFiletest::open_test(){
    return result.Open(fname);
}

// int Factorial(int n){
// 	if(n==0 || n ==1){
// 		return 1;
// 	}
// 	return n * Factorial(n-1);
// }

// // Tests factorial of 0.
// TEST(FactorialTest, HandlesZeroInput) {
//   EXPECT_EQ(1, Factorial(0));
// }

// // Tests factorial of positive numbers.
// TEST(FactorialTest, HandlesPositiveInput) {
//   EXPECT_EQ(1, Factorial(1));
//   EXPECT_EQ(2, Factorial(2));
//   EXPECT_EQ(6, Factorial(3));
//   EXPECT_EQ(40320, Factorial(8));
// }

TEST(DBFILETEST, Opentest){
	DBFiletest db;
	EXPECT_EQ(1,db.create_test());
}