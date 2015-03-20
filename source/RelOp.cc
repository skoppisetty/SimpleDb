#include "RelOp.h"

// typedef  void* (SelectFile::*sf_Ptr)(void *);
// typedef  void* (*PthreadPtr)(void*);
// PthreadPtr G_p;

sf_input G_sf_input;
p_input G_p_input;
s_input G_s_input;

void * sf_Runit (void * arg) {
	// cout << "Inside thread SelectFile::Runit" << endl;
	sf_input *t = (sf_input *) arg;
	ComparisonEngine ceng;
	Record rec;
	while(t->inFile->GetNext(rec)){
		if(ceng.Compare(&rec,t->literal,t->selop)==1){
			t->out->Insert(&rec);
		}
	}
	t->out->ShutDown();
}

void SelectFile::Run (DBFile &inFile, Pipe &outPipe, CNF &selOp, Record &literal) {
	cout << "SelectFile::Run()" << endl;
	G_sf_input = {&inFile, &outPipe, &selOp, &literal};
 	// sf_Ptr sft = &SelectFile::Runit;
	// G_p = *(PthreadPtr*)&sft;
  	pthread_create (&thread, NULL, sf_Runit ,(void *)&G_sf_input);
  
}



void SelectFile::WaitUntilDone () {
	pthread_join (thread, NULL);
}

void SelectFile::Use_n_Pages (int runlen) {

}

void * p_Runit (void * arg) {
	cout << "Inside thread Project::Runit" << endl;
	p_input *t = (p_input *) arg;
	Record temp;
	while(t->in->Remove(&temp)){
		// temp.Print(&part_schema);
		// cout << "inside" << endl;
		temp.Project(*t->keep,*t->num_out,*t->num_in);
		// temp.Print(&part_schema);
		// cout << "sending" << endl;
		t->out->Insert(&temp);
	}
	t->out->ShutDown();
}
void Project::Run (Pipe &inPipe, Pipe &outPipe, int *keepMe, int numAttsInput, int numAttsOutput) {
	cout << "Project::Run()" << endl;
	G_p_input = {&inPipe, &outPipe, &keepMe, &numAttsInput, &numAttsOutput};
 	// sf_Ptr sft = &SelectFile::Runit;
	// G_p = *(PthreadPtr*)&sft;
  	pthread_create (&thread, NULL, p_Runit ,(void *)&G_p_input);
  
}



void Project::WaitUntilDone () {
	pthread_join (thread, NULL);
}

void Project::Use_n_Pages (int runlen) {

}


void * s_Runit (void * arg) {
	cout << "Inside thread Sum::Runit" << endl;
	s_input *t = (s_input *) arg;
	Record rec;
	double sum = 0.0;
	while (t->in->Remove (&rec)){
		int ival = 0; 
		double dval = 0.0;
		t->func->Apply(rec, ival, dval);
		sum += (ival + dval);
	}
	cout << "Sum: " << sum << endl;
	Attribute DA = {"double", Double};
	Schema out_sch ("out_sch", 1, &DA);
	char buffer[32];
  	sprintf(buffer, "%1.2f", sum);
	// string str = to_string(sum) + "|";
	// cout << str << endl;

	ostringstream out;
    out << buffer;
    // cout << out.str() + "|" << endl;
    string str = out.str() + "|";
	// char *val = str.c_str();
	rec.ComposeRecord(&out_sch,str.c_str());
	rec.Print(&out_sch);
	t->out->Insert(&rec);
	t->out->ShutDown();
}
void Sum::Run (Pipe &inPipe, Pipe &outPipe, Function &computeMe) {
	cout << "Sum::Run()" << endl;
	G_s_input = {&inPipe, &outPipe, &computeMe};
 	// sf_Ptr sft = &SelectFile::Runit;
	// G_p = *(PthreadPtr*)&sft;
  	pthread_create (&thread, NULL, s_Runit ,(void *)&G_s_input);
  
}



void Sum::WaitUntilDone () {
	pthread_join (thread, NULL);
}

void Sum::Use_n_Pages (int runlen) {

}