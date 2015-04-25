#ifndef PARSE_NODE_H
#define PARSE_NODE_H

#define P_SIZE 100

#include "Schema.h"
#include "Pipe.h"

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

	AndList *cnf;

};

#endif