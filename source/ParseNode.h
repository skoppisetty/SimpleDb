#ifndef PARSE_NODE_H
#define PARSE_NODE_H

#define P_SIZE 100


#include <vector>


enum ParseNodeType {SELECTF, SELECTP, PROJECT, JOIN, SUM, GROUP_BY, DISTINCT, WRITE};

class ParseNode {

 public:

	      //REL OP SECTION:
		SelectFile *sf;
		SelectPipe *sp;
		Join *j;
		GroupBy *gb;
		Project *p;
		Sum *s;
		DuplicateRemoval *d;
		WriteOut *h;
	
		DBFile *db;

      ParseNodeType type;
      ParseNode *parent;
      ParseNode *left;
      ParseNode *right;

	      //Pipe identifiers ( can be replaced by actual pipes later, or something)
		int lChildPipeID;
		Pipe *lInputPipe;
		int rChildPipeID;
		Pipe *rInputPipe;
		int outPipeID;
		Pipe *outPipe;

	   ParseNode();
	 	~ParseNode();

		void PrintInOrder();
   	void PrintNode ();
   	void PrintCNF();
		void PrintFunction();

		void SetType(ParseNodeType setter);
      string GetTypeName ();
      ParseNodeType GetType ();
		

		//Used for JOIN
		void GenerateSchema();
		//Used for SUM
		void GenerateFunction();
		//Used for GROUP_BY
		void GenerateOM(int numAtts, vector<int> whichAtts, vector<int> whichTypes);
		

// private:

		// For a PROJECT
		int numAttsIn;
		int numAttsOut;
		vector<int> aTK;
	  	int *attsToKeep;
	  	int numAttsToKeep;
		//NameList *projectAtts;
		// For various operations
      AndList *cnf;
      CNF *opCNF;
      Schema *schema;
		// For GROUP BY
      OrderMaker *order;
		// For aggregate
	   FuncOperator *funcOp;
		Function *func;
		// For Write and SelectFile (identifies where files are located)
		string path;

		string lookupKey(string path);

	   vector<string> relations;

};

#endif