#include "Statistics.h"

using namespace std;

Statistics::Statistics()
{
}
Statistics::Statistics(Statistics &copyMe)
{
}
Statistics::~Statistics()
{
}

void Statistics::AddRel(char *relName, int numTuples)
{
	string relname(relName);
	RelInfo newrel(numTuples);
	rels[relname]=newrel;
}

void Statistics::AddAtt(char *relName, char *attName, int numDistincts)
{
	string relname(relName);
	string attname(attName);

	if(numDistincts==-1){
		rels[relname].AddAtt(attname,rels[relname].numTuples);
	} else {
		rels[relname].AddAtt(attname,numDistincts);
	}
}

void Statistics::CopyRel(char *oldName, char *newName)
{	
	string oldname(oldName);
	string newname(newName);

	// map<string,int> oldAttInfo = rels[oldname].AttInfo;
	// RelInfo newrel(rels[oldname].numTuples);

	// for(auto i:oldAttInfo){
	// 	newrel.AddAtt(i.first,i.second);
	// }

	// rels[newname]=newrel;
}
	
void Statistics::Read(char *fromWhere)
{
}
void Statistics::Write(char *fromWhere)
{
}

void  Statistics::Apply(struct AndList *parseTree, char *relNames[], int numToJoin)
{
}
double Statistics::Estimate(struct AndList *parseTree, char **relNames, int numToJoin)
{
}

