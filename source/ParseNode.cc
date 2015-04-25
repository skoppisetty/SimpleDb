// #include "Schema.h"
// #include "Function.h"
// #include "Comparison.h"
#include "ParseNode.h"

// #include "SelectFile.h"
// #include "SelectPipe.h"
// #include "Join.h"
// #include "GroupBy.h"
// #include "Project.h"
// #include "Sum.h"
// #include "DuplicateRemoval.h"
// #include "WriteOut.h"

#include <iostream>
#include <vector>

using namespace std;

ParseNode::ParseNode(){

	lChildPipeID = 0;
	rChildPipeID = 0;
	outPipeID = 0;

	parent = NULL;
	left = NULL;
	right = NULL;

	schema = NULL;
}

ParseNode::~ParseNode(){
}

void ParseNode::SetType(ParseNodeType setter){
	type = setter;
}

void ParseNode::GenerateSchema(){
	Schema *lChildSchema = left->schema;
	Schema *rChildSchema = right->schema;
	schema = new Schema(lChildSchema, rChildSchema);
}
