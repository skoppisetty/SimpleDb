#ifndef PARSE_NODE_H
#define PARSE_NODE_H

#define P_SIZE 100

#include "Schema.h"
#include "Pipe.h"
#include "Function.h"
#include <vector>
#include <string>

enum ParseNodeType {SELECTF, SELECTP, PROJECT, JOIN, SUM, GROUP_BY, DISTINCT, WRITE};

class ParseNode {

public:

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

	Schema *schema;
	string path;

	ParseNode();
	~ParseNode();

	void SetType(ParseNodeType setter);
	void GenerateSchema();
	
	void GenerateFunction();
	void GenerateOM(int numAtts, vector<int> whichAtts, vector<int> whichTypes);
	
	void PrintInOrder();
	void PrintNode();
	string GetTypeName ();
	void PrintCNF();
	void PrintFunction();

	AndList *cnf;
	FuncOperator *funcOp;
	Function *func;
	OrderMaker *order;
};

#endif