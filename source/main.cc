
#include <iostream>
#include "ParseTree.h"
#include "Statistics.h"

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
					// get table name
					string rel;
					stats.ParseRelation(op, rel);
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


	// cout << whichOne << endl;
	PrintAndList(boolean);
	cout << endl;
	// PrintOrList(myOrList);


}


