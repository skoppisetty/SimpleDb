
#include <iostream>
#include "ParseTree.h"
#include "Statistics.h"
#include "ParseNode.h"
#include <string.h>

extern struct FuncOperator *finalFunction; // the aggregate function (NULL if no agg)
extern struct TableList *tables; // the list of tables and aliases in the query
extern struct AndList *boolean; // the predicate in the WHERE clause
extern struct NameList *groupingAtts; // grouping atts (NULL if no grouping)
extern struct NameList *attsToSelect; // the set of attributes in the SELECT (NULL if no such atts)
extern int distinctAtts; // 1 if there is a DISTINCT in a non-aggregate query 
extern int distinctFunc; 

using namespace std;

void PrintOperand(struct Operand *pOperand)
{
        if(pOperand!=NULL)
        {
                cout<<pOperand->value<<" ";
        }
        else
                return;
}

void PrintComparisonOp(struct ComparisonOp *pCom)
{
        if(pCom!=NULL)
        {
                PrintOperand(pCom->left);
                switch(pCom->code)
                {
                        case 1:
                                cout<<" < "; break;
                        case 2:
                                cout<<" > "; break;
                        case 3:
                                cout<<" = ";
                }
                PrintOperand(pCom->right);

        }
        else
        {
                return;
        }
}
void PrintOrList(struct OrList *pOr)
{
        if(pOr !=NULL)
        {
                struct ComparisonOp *pCom = pOr->left;
                PrintComparisonOp(pCom);

                if(pOr->rightOr)
                {
                        cout<<" OR ";
                        PrintOrList(pOr->rightOr);
                }
        }
        else
        {
                return;
        }
}

void PrintAndList(struct AndList *pAnd)
{
        if(pAnd !=NULL)
        {
                struct OrList *pOr = pAnd->left;
                PrintOrList(pOr);
                if(pAnd->rightAnd)
                {
                        cout<<" AND ";
                        PrintAndList(pAnd->rightAnd);
                }
        }
        else
        {
                return;
        }
}

extern "C" {
	int yyparse(void);   // defined in y.tab.c
}

void parse_query(vector<AndList> &joins, vector<AndList> &selects, vector<AndList> &joinDepSel, Statistics stats){

  struct OrList *current_Or;

  	while (boolean != 0){
		current_Or = boolean->left;
		if (current_Or && current_Or->left->code == EQUALS && current_Or->left->left->code == NAME
			&& current_Or->left->right->code == NAME){
			// Join case - both or->left->right and or->left->right have the value 
				// of NAME for code variable.
				// ex: "a.x = b.x"
			AndList join_andlist = *boolean;
			join_andlist.rightAnd = NULL;
			joins.push_back(join_andlist);
		}
    	else {
			current_Or = boolean->left;
			if(current_Or->left == NULL){ 
				// single or list , so push to select
				AndList select_andlist = *boolean;
				select_andlist.rightAnd = NULL;
				selects.push_back(select_andlist);
			}
			else{
				// mutiple or lists
				vector<string> involvedTables;
				while(current_Or != NULL){
					Operand *op = current_Or->left->left;
					if(op->code != NAME){
						op = current_Or->left->right;
					}
					// get relation name
					string rel;
					stats.GetRelation(op, rel);
					cout << "table name" << rel << endl;
					if(involvedTables.size() == 0){
						involvedTables.push_back(rel);
					}
					else if(rel.compare(involvedTables[0]) != 0){
						involvedTables.push_back(rel);
					}
					current_Or = current_Or->rightOr;
				}

				if(involvedTables.size() > 1){
					// dependent query
					AndList select_dep = *boolean;
					select_dep.rightAnd = 0;
					joinDepSel.push_back(select_dep);
				}
				else{
					// independent query
					AndList select_indep = *boolean;
					select_indep.rightAnd = 0;
					selects.push_back(select_indep);
				}		
			}	
   	}
    	boolean = boolean->rightAnd;
  	} 
} 


void fetch_relations(vector<string> &relations){
  	// get all the relations names
  	TableList *list = tables;
  	while (list){
		if (list->aliasAs){
			relations.push_back(list->aliasAs);
		}
		else {
			relations.push_back(list->tableName);
		}
		list = list->next;
	}
}

vector<AndList> optimize_joins(vector<AndList> joins, Statistics *s){
	vector<AndList> newjoins;
	AndList join;
	double smallest = -1.0;
	double guess = 0.0;
	string rel1;
	string rel2;
	int i = 0;
	int smallestIndex = 0;
	int count = 0;
	vector<string> joinedRels;

	while(joins.size() > 1){
		while(i < joins.size()){
			join = joins[i];

			s->GetRelation(join.left->left->left, rel1);
			s->GetRelation(join.left->left->right, rel2);

			if(smallest == -1.0){
				char* rels[] = {(char*)rel1.c_str(), (char*)rel2.c_str()};
				smallest = s->Estimate(&join, rels, 2);
				smallestIndex = i;
			} else {
				char* rels[] = {(char*)rel1.c_str(), (char*)rel2.c_str()};
				guess = s->Estimate(&join, rels, 2);
				if(guess < smallest){
					smallest = guess;
					smallestIndex = i;
				}
			}
			i++;
		}
		joinedRels.push_back(rel1);
		joinedRels.push_back(rel2);
		newjoins.push_back(joins[smallestIndex]);
		count++;
		smallest = -1.0;
		i = 0;
		joins.erase(joins.begin()+smallestIndex);
	}
	newjoins.insert(newjoins.begin()+count, joins[0]);
	return newjoins;
}


void generate_stats(Statistics *stats){
	char *relName[] = {"region", "nation", "part", "supplier", "partsupp", "customer", "orders", "lineitem"};
	
	//region block
	stats->AddRel(relName[0],5);
	stats->AddAtt(relName[0], "r_regionkey",5);
	stats->AddAtt(relName[0], "r_name",5);
	stats->AddAtt(relName[0], "r_comment",5);
	
	//nation block
	stats->AddRel(relName[1], 25);
	stats->AddAtt(relName[1], "n_nationkey",25);
	stats->AddAtt(relName[1], "n_name",25);
	stats->AddAtt(relName[1], "n_regionkey",5);
	stats->AddAtt(relName[1], "n_comment",25);
	//part block
	stats->AddRel(relName[2], 200000);
	stats->AddAtt(relName[2], "p_partkey",200000);
	stats->AddAtt(relName[2], "p_name",199996);
	stats->AddAtt(relName[2], "p_mfgr",5);
	stats->AddAtt(relName[2], "p_brand",25);
	stats->AddAtt(relName[2], "p_type",150);
	stats->AddAtt(relName[2], "p_size",50);
	stats->AddAtt(relName[2], "p_container",40);
	stats->AddAtt(relName[2], "p_retailprice",20899);
	stats->AddAtt(relName[2], "p_comment",127459);
	
	//supplier block
	stats->AddRel(relName[3], 10000);
	stats->AddAtt(relName[3], "s_suppkey",10000);
	stats->AddAtt(relName[3], "s_name",10000);
	stats->AddAtt(relName[3], "s_address",10000);
	stats->AddAtt(relName[3], "s_nationkey",25);
	stats->AddAtt(relName[3], "s_phone",10000);
	stats->AddAtt(relName[3], "s_acctbal",9955);
	stats->AddAtt(relName[3], "s_comment",10000);
	
	//partsupp block
	stats->AddRel(relName[4], 800000);
	stats->AddAtt(relName[4], "ps_partkey",200000);
	stats->AddAtt(relName[4], "ps_suppkey",10000);
	stats->AddAtt(relName[4], "ps_availqty",9999);
	stats->AddAtt(relName[4], "ps_supplycost",99865);
	stats->AddAtt(relName[4], "ps_comment",799123);

	//customer block 150000
	stats->AddRel(relName[5], 150000);
	stats->AddAtt(relName[5], "c_custkey",150000);
	stats->AddAtt(relName[5], "c_name",150000);
	stats->AddAtt(relName[5], "c_address",150000);
	stats->AddAtt(relName[5], "c_nationkey",25);
	stats->AddAtt(relName[5], "c_phone",150000);
	stats->AddAtt(relName[5], "c_acctbal",140187);
	stats->AddAtt(relName[5], "c_mktsegment",5);
	stats->AddAtt(relName[5], "c_comment",149965);
	
	//orders block 	1500000
	stats->AddRel(relName[6], 1500000);
	stats->AddAtt(relName[6], "o_orderkey",1500000);
	stats->AddAtt(relName[6], "o_custkey",99996);
	stats->AddAtt(relName[6], "o_orderstatus",3);
	stats->AddAtt(relName[6], "o_totalprice",1464556);
	stats->AddAtt(relName[6], "o_orderdate",2406);
	stats->AddAtt(relName[6], "o_orderpriority",5);
	stats->AddAtt(relName[6], "o_clerk",1000);
	stats->AddAtt(relName[6], "o_shippriority",1);
	stats->AddAtt(relName[6], "o_comment",1478684);
	
	//lineitem block 6001215
	stats->AddRel(relName[7], 6001215);
	stats->AddAtt(relName[7], "l_orderkey",1500000);
	stats->AddAtt(relName[7], "l_partkey",200000);
	stats->AddAtt(relName[7], "l_suppkey",10000);
	stats->AddAtt(relName[7], "l_linenumber",7);
	stats->AddAtt(relName[7], "l_quantity",50);
	stats->AddAtt(relName[7], "l_extendedprice",933900);
	stats->AddAtt(relName[7], "l_discount",11);
	stats->AddAtt(relName[7], "l_tax",9);
	stats->AddAtt(relName[7], "l_returnflag",3);
	stats->AddAtt(relName[7], "l_linestatus",2);
	stats->AddAtt(relName[7], "l_shipdate",2526);
	stats->AddAtt(relName[7], "l_commitdate",2466);
	stats->AddAtt(relName[7], "l_receiptdate",2554);
	stats->AddAtt(relName[7], "l_shipinstruct",4);
	stats->AddAtt(relName[7], "l_shipmode",7);
	stats->AddAtt(relName[7], "l_comment",4501941);
}

int main () {

	Statistics *stats = new Statistics();

	// Read some statistics file
	// stats->Read("tcp-h_stats.txt");

	generate_stats(stats);


	map<string, double> joinCosts;
	vector<string> relations;
	vector<AndList> joins;
	vector<AndList> selects;
	vector<AndList> joinDepSels;

	yyparse();

	// Fetch the relations
	fetch_relations(relations);

	// split the andlist to seperate andlists
	parse_query(joins, selects, joinDepSels, *stats);

	cout << "Number of selects: " << selects.size() << endl;
	cout << "Number of joins: " << joins.size() << endl;
	cout << "Number of join dependent selects: " << joinDepSels.size() << endl;

	// cout << whichOne << endl;
	map<string, ParseNode*> leafs; // used to store the slelect file nodes
	ParseNode *insert = NULL; //holder variable for when we need to insert stuff.
	ParseNode *traverse;
	ParseNode *topNode = NULL;
	int pipeID = 1; // Initialise pipeID to 1
	string projectStart;
	
	TableList *iterTable = tables;	// parser populates the TableList structure
	while(iterTable != NULL){		// Generating Select file nodes
		if(iterTable->aliasAs != NULL){
			leafs.insert(pair<string,ParseNode*>(iterTable->aliasAs, new ParseNode()));
			stats->CopyRel(iterTable->tableName, iterTable->aliasAs);
			insert = leafs[iterTable->aliasAs];
		}
		else{
			leafs.insert(pair<string,ParseNode*>(iterTable->tableName, new ParseNode()));
			insert = leafs[iterTable->tableName];
		}
		// Values to set in a node
		// ParseNodeType
		// outPipeID
		// path
		// schema

		// Create scheme for each table
		insert->schema = new Schema ("catalog", iterTable->tableName);
		if(iterTable->aliasAs != 0){ // update the table name if it is an alias
			insert->schema->updateName(string(iterTable->aliasAs));
		}
		topNode = insert;

		insert->outPipeID = pipeID++;
		string base (iterTable->tableName);
		string path ("bin/"+base+".bin");
		insert->path = strdup(path.c_str());
		insert->SetType(SELECTF);
		iterTable = iterTable->next;	
	}

	// Adding select nodes to the tree
	AndList selectIter;
	string table;
	string attribute;

	for(int i = 0; i < selects.size(); i++){
		selectIter = selects[i];
		if(selectIter.left->left->left->code == NAME){
			stats->GetRelation(selectIter.left->left->left, table);
		}
		else{
			stats->GetRelation(selectIter.left->left->right, table);
		}

		traverse = leafs[table]; //Get the root node (Select File)
		projectStart = table;
		while(traverse->parent != NULL){
			traverse = traverse->parent;
		}
		// Values to set for select node
		// ParseNodeType
		// outPipeID
		// lChildPipeID
		// left pointer
		// schema
		// cnf
		insert = new ParseNode();
		traverse->parent = insert; // link select node to selectfile node
		insert->left = traverse;
		insert->schema = traverse->schema; //Schemas are the same throughout selects, only rows change
		insert->type = SELECTP;
		insert->cnf = &selects[i]; //Need to implement CreateCNF in QueryTreeNode
		insert->lChildPipeID = traverse->outPipeID;
		insert->outPipeID = pipeID++;
		char *statApply = strdup(table.c_str());
		stats->Apply(&selectIter, &statApply,1);
		topNode = insert;
	}

	// Add join nodes to the tree
	// Before adding , we need to optimize the order in which we need to add
	if(joins.size() > 1){
		joins = optimize_joins(joins, stats);
	}

	ParseNode *lTableNode;
	ParseNode *rTableNode;
	AndList curJoin;
	string rel1;
	string rel2;
	for(int i = 0; i < joins.size(); i++){
		curJoin = joins[i];
		rel1 = "";//curJoin.left->left->left->value;
		stats->GetRelation(curJoin.left->left->left, rel1);
		rel2 = "";//curJoin.left->left->right->value;
		stats->GetRelation(curJoin.left->left->right, rel2);
		table = rel1; //done for testing purposes. will remove later
		//So, now we can get the top nodes for each of these
		lTableNode = leafs[rel1];
		rTableNode = leafs[rel2];
		while(lTableNode->parent != NULL) {
			lTableNode = lTableNode->parent;
		}
		while(rTableNode->parent != NULL) {
			rTableNode = rTableNode->parent;
		}
		//At this point, we have the top node for the left, and for the right
		//Now we join! MWAHAHAHA
		insert = new ParseNode();
		insert->type = JOIN;
		insert->lChildPipeID = lTableNode->outPipeID;
		insert->rChildPipeID = rTableNode->outPipeID;
		insert->outPipeID = pipeID++;
		insert->cnf = &joins[i];
		insert->left = lTableNode;
		insert->right = rTableNode;
		lTableNode->parent = insert;
		rTableNode->parent = insert;

		insert->GenerateSchema();
		topNode = insert;
	}

	for(unsigned i = 0; i < joinDepSels.size(); i++){
		traverse = topNode;
		insert = new ParseNode();
		traverse->parent = insert;
		insert->left = traverse;
		insert->schema = traverse->schema; //Schemas are the same throughout selects, only rows change
		insert->type = SELECTP;
		insert->cnf = &joinDepSels[i]; //Need to implement CreateCNF in QueryTreeNode
		insert->lChildPipeID = traverse->outPipeID;
		insert->outPipeID = pipeID++;
		topNode = insert;
	}


	if(finalFunction != 0) { 
		if(distinctFunc != 0){
			insert = new ParseNode();
			insert->type = DISTINCT;
			insert->left = topNode;
			insert->lChildPipeID = topNode->outPipeID;
			insert->outPipeID = pipeID++;
			insert->schema = topNode->schema;
			topNode->parent = insert;
			topNode = insert;		
		}
		if(groupingAtts != NULL){
			insert = new ParseNode();
			insert->type = SUM;
			insert->left = topNode;
			topNode->parent = insert;
			insert->lChildPipeID = topNode->outPipeID;
			insert->outPipeID = pipeID++;
			insert->funcOp = finalFunction;
			insert->schema = topNode->schema;
			insert->GenerateFunction();
		}
		else{		
			insert = new ParseNode();
			insert->type = GROUP_BY;
			insert->left = topNode;
			topNode->parent = insert;
			insert-> lChildPipeID = topNode->outPipeID;
			insert->outPipeID = pipeID++;
			insert->schema = topNode->schema;	
			insert->order = new OrderMaker();	
			int numAttsToGroup = 0;
			vector<int> attsToGroup;
			vector<int> whichType;
			NameList *groupTraverse = groupingAtts;
			while(groupTraverse){
				numAttsToGroup++;
				attsToGroup.push_back(insert->schema->Find(groupTraverse->name));
				whichType.push_back(insert->schema->FindType(groupTraverse->name));
				groupTraverse = groupTraverse->next;
			}

			insert->GenerateOM(numAttsToGroup, attsToGroup, whichType);
			insert->funcOp = finalFunction;
			insert->GenerateFunction();
		}
		topNode = insert;
	}

	if(distinctAtts != 0){
		insert = new ParseNode();
		insert->type = DISTINCT;
		insert->left = topNode;
		topNode->parent = insert;
		insert->lChildPipeID = topNode->outPipeID;
		insert->outPipeID = pipeID++;
		insert->schema = topNode->schema;
		topNode = insert;
	}

	if(attsToSelect != 0){ 
		traverse = topNode;
		insert = new ParseNode();
		insert->type = PROJECT;
		insert->left = traverse;
		traverse->parent = insert;
		insert->lChildPipeID = traverse->outPipeID;
		insert->outPipeID = pipeID++;

		vector<int> indexOfAttsToKeep;
		Schema *oldSchema = traverse->schema;
		NameList *attsTraverse = attsToSelect;
		string attribute;

		while(attsTraverse != 0){
			attribute = attsTraverse-> name;
			indexOfAttsToKeep.push_back(oldSchema->Find(const_cast<char*>(attribute.c_str())));
			attsTraverse = attsTraverse->next;
		}
		Schema *newSchema = new Schema(oldSchema, indexOfAttsToKeep);
		insert->schema = newSchema;
		insert->schema->Print();
	}
	
	cout << "PRINTING PARSE TREE IN ORDER: " << endl;
	if(insert != NULL) insert->PrintInOrder();

	// PrintAndList(boolean);
	// cout << endl;


}


