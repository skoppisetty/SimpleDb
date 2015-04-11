#ifndef STATISTICS_
#define STATISTICS_
#include "ParseTree.h"
#include <map>
#include <string>

using namespace std;

class RelInfo
{
private:
	// typedef long long int tupletype;
public:
	int numTuples;
	map<string,int> AttInfo;
	RelInfo(int num){
		numTuples = num;
	}
	void AddAtt(string att, int num){
		AttInfo[att]=num;
	}

};


class Statistics
{
private:
	// long long int numTuples;
	

public:
	map<string,RelInfo> rels;
	Statistics();
	Statistics(Statistics &copyMe);	 // Performs deep copy
	~Statistics();


	void AddRel(char *relName, int numTuples);
	void AddAtt(char *relName, char *attName, int numDistincts);
	void CopyRel(char *oldName, char *newName);
	
	void Read(char *fromWhere);
	void Write(char *fromWhere);

	void  Apply(struct AndList *parseTree, char *relNames[], int numToJoin);
	double Estimate(struct AndList *parseTree, char **relNames, int numToJoin);

};


#endif
