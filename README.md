Readme 
=========

Assignment 1 Implementation of Heap DBFile (Spring 2015 DBI)

TEAM:
=========
Sharath Chandra Darsha 	 UFID: 45194064
Suresh Koppisetty	 UFID: 18111766	

Important Note:
=========
We are using the 10M data provided with the code. Please note that this data is different from the data provided. So some CNF queries may give different results.

Folder Structure:
=========
./
./README
./Makefile
./source/
./bin/
./DATA/10M/

Steps to compile:
=========
1. make clean				// Clean any previously compiled code
2. make all				// Compile all the files necessary  
3. cd bin
4. ./gtesting.o < 1.txt 		// if you wanna pass the CNF query automatically (tests only for lineitem table)
   .gtesting.o 				// tests the written code for creat,load,open,movefirst,getnext and getnext with cnf and various other 						test cases using googletest framework and google mock.
5. ./test.out  				// run this if you want to test the code with other tables.

Executable Files:
=========
1. gtesting.o - This loads the lineitem table - 
			1. checks the return values for all the functions. 
			2. Loads 60175 records in the start.
			3. We take the first records by using movefirst and getnext.
			4. Add the fetched record at the end of the heap using Add function.
			5. We use Modes (Read and Write Mode) and we write the page whenever there is a switch in the mode.
			6. We scan the database to check for the updated new record (total records should be 60175 + 1 (i.e 60176))
			7. We also check the cnf filter results, which should return 30 rows for lineitem table.

2. test.out - Given test script to run the code(load, scan and scan with filter(CNF should be provided)) for various database tables.
3. main - given starting version of main.o - reads the records from lineitem.tbl and prints the records matching the CNF.

Settings:
=========
The following variables control the various file locations and they are declared in test.cc and gtesting.cc (just after the #include header declarations):
	o dbfile_dir -- this is where the created heap db-files will be stored. By default, this is set to "" (thus all the heap dbfiles will be created in the "bin" directory).
	o tpch_dir -- this stores the directory path where the tpch-files can be found. The "/DATA/" directory already has the required table files generated by tpchgen and the path should be accessible by the program. We supplied only 10M data along with the code.
	o catalog_path -- this stores the catalog file path. By default this is set to "source" folder. 


Results:
=========
3. Once load of a file has been selected, you can select option 2 or 3 to scan/filter all the records from the heap DBfile.  If option 3 is selected, a CNF should be supplied. Some example CNF's are given below. They are numbered q1,q2..q12. Use the table below to identify the tpch file associated with each CNF.
     	table    |   CNF
 ---------------------------------------
        region    |  q1 q2   
        nation    |  q3   
        supplier  |  q4 q5	#Todo - test.out doesnt have this table
        part      |  q7   	#Todo - retailprice CNF error
        partsupp  |  q8 q9 	
        orders    |  q10                
        lineitem  |  q11 q12 

PLEASE GIVE THE STRINGS IN THE QUERY IN CAPITALS (CASE SENSITIVE)

Example CNF's
================

q1 
(r_name = 'EUROPE')

Result:
 Filter with CNF for : region
 Enter CNF predicate (when done press ctrl-D):
	(r_name = 'EUROPE')
r_regionkey: [3], r_name: [EUROPE], r_comment: [ly final courts cajole furiously final excuse]
 selected 1 recs 


q2 
(r_name < 'middle east') AND (r_regionkey > 1)

Result:
 Filter with CNF for : region
 Enter CNF predicate (when done press ctrl-D):
	(r_name < 'middle east') AND (r_regionkey > 1)
r_regionkey: [2], r_name: [ASIA], r_comment: [ges. thinly even pinto beans ca]
r_regionkey: [3], r_name: [EUROPE], r_comment: [ly final courts cajole furiously final excuse]
r_regionkey: [4], r_name: [MIDDLE EAST], r_comment: [uickly special accounts cajole carefully blithely close requests. carefully final asymptotes haggle furiousl]
 selected 3 recs

# with capital strings
(r_name < 'MIDDLE EAST') AND (r_regionkey > 1)


Result:
 Filter with CNF for : region
 Enter CNF predicate (when done press ctrl-D):
	(r_name < 'MIDDLE EAST') AND (r_regionkey > 1)
r_regionkey: [2], r_name: [ASIA], r_comment: [ges. thinly even pinto beans ca]
r_regionkey: [3], r_name: [EUROPE], r_comment: [ly final courts cajole furiously final excuse]
 selected 2 recs


q3 
(n_regionkey = 3) AND (n_nationkey > 10) AND (n_name > 'japan')

Result:
 Filter with CNF for : nation
 Enter CNF predicate (when done press ctrl-D):
	(n_regionkey = 3) AND (n_nationkey > 10) AND (n_name > 'japan')
 selected 0 recs

(n_regionkey = 3) AND (n_nationkey > 10) AND (n_name > 'JAPAN')

Result:
 Filter with CNF for : nation
 Enter CNF predicate (when done press ctrl-D):
	(n_regionkey = 3) AND (n_nationkey > 10) AND (n_name > 'JAPAN')
n_nationkey: [19], n_name: [ROMANIA], n_regionkey: [3], n_comment: [ular asymptotes are about the furious multipliers. express dependencies nag above the ironically ironic account]
n_nationkey: [22], n_name: [RUSSIA], n_regionkey: [3], n_comment: [ requests against the platelets use never according to the quickly regular pint]
n_nationkey: [23], n_name: [UNITED KINGDOM], n_regionkey: [3], n_comment: [eans boost carefully special requests. accounts are. carefull]
 selected 3 recs


q4 
(s_suppkey < 10)


q5
(s_nationkey = 18) AND
(s_acctbal > 1000) AND
(s_suppkey < 400)

q6
(c_nationkey = 23) AND
(c_mktsegment = 'FURNITURE') AND
(c_acctbal > 7023.99) AND
(c_acctbal < 7110.83)


q7 
(p_brand = 'Brand#13') AND (p_retailprice > 500) AND (p_retailprice < 930) AND (p_size > 28) AND (p_size < 1000000)

q8 
(ps_supplycost > 999.98)

Result:
 Filter with CNF for : partsupp
 Enter CNF predicate (when done press ctrl-D):
	(ps_supplycost > 999.98)
ps_partkey: [1213], ps_suppkey: [51], ps_availqty: [5070], ps_supplycost: [999.99], ps_comment: [ ironic, special deposits. carefully final deposits haggle fluffily. furiously final foxes use furiously furiously ironic accounts. package]
 selected 1 recs 


q9 
(ps_availqty < 10)
(ps_supplycost > 100) AND
(ps_suppkey < 300) AND

q10 
(o_orderpriority = '1-URGENT') AND
(o_orderstatus = '0') AND
(o_shippriority = 0) AND
(o_totalprice > 1015.68) AND
(o_totalprice < 1051.89)

q11
(l_shipdate > '1994-01-01') AND
(l_shipdate < '1994-01-07') AND
(l_discount > 0.05) AND
(l_discount < 0.06) AND
(l_quantity = 4) 


q12
(l_orderkey > 100) AND
(l_orderkey < 1000) AND
(l_partkey > 100) AND
(l_partkey < 5000) AND
(l_shipmode = 'AIR') AND
(l_linestatus = 'F') AND
(l_tax < 0.07)
