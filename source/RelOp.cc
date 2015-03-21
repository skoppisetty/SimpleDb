#include "RelOp.h"

// typedef  void* (SelectFile::*sf_Ptr)(void *);
// typedef  void* (*PthreadPtr)(void*);
// PthreadPtr G_p;
sp_input G_sp_input;
sf_input G_sf_input;
p_input G_p_input;
s_input G_s_input;
d_input G_d_input;


void * sp_Runit (void * arg) {
	// cout << "Inside thread SelectFile::Runit" << endl;
	sp_input *t = (sp_input *) arg;
	ComparisonEngine ceng;
	Record rec;
	while(t->in->Remove(&rec)){
		if(ceng.Compare(&rec,t->literal,t->selop)==1){
			t->out->Insert(&rec);
		}
	}
	t->out->ShutDown();
}

void SelectPipe::Run (Pipe &inPipe, Pipe &outPipe, CNF &selOp, Record &literal) {
	cout << "SelectPipe::Run()" << endl;
	G_sp_input = {&inPipe, &outPipe, &selOp, &literal};
 	// sf_Ptr sft = &SelectFile::Runit;
	// G_p = *(PthreadPtr*)&sft;
  	pthread_create (&thread, NULL, sp_Runit ,(void *)&G_sp_input);
  
}



void SelectPipe::WaitUntilDone () {
	pthread_join (thread, NULL);
}

void SelectPipe::Use_n_Pages (int runlen) {

}


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
	cout << "Size:" << (t->num_out) << endl;
	for(int i = 0;i< t->num_out;i++){
		cout << t->keep[i] << endl;
	}
	while(true){
	Record * temp = new Record;
		if(t->in->Remove(temp)){
			temp->Project(t->keep,t->num_out,t->num_in);
			t->out->Insert(temp);
		}	
		else{
			break;
		}	
	}
	t->out->ShutDown();
}
void Project::Run (Pipe &inPipe, Pipe &outPipe, int *keepMe, int numAttsInput, int numAttsOutput) {
	cout << "Project::Run()" << endl;
	G_p_input = {&inPipe, &outPipe, keepMe, numAttsInput, numAttsOutput};
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
	// char buffer[32];
  	// sprintf(buffer, "%1.2f", sum);
	// string str = to_string(sum) + "|";
	// cout << str << endl;

	// ostringstream outs;
    // outs << buffer;
    // cout << out.str() + "|" << endl;
    // string str = outs.str() + "|";
	// char *val = str.c_str();
	char * cls = "10.20|\n";
	rec.ComposeRecord(&out_sch,cls);
	// rec.ComposeRecord(&out_sch,str.c_str());
	rec.Print(&out_sch);
	t->out->Insert(&rec);
	t->out->ShutDown();
	cout << "thread done" << endl;
}
void Sum::Run (Pipe &inPipe, Pipe &outPipe, Function &computeMe) {
	cout << "Sum::Run()" << endl;
	G_s_input = {&inPipe, &outPipe, &computeMe};
 	// sf_Ptr sft = &SelectFile::Runit;
	// G_p = *(PthreadPtr*)&sft;
  	pthread_create (&thread, NULL, s_Runit ,(void *)&G_s_input);
  
}



void Sum::WaitUntilDone () {
	cout << "twaiting" << endl;
	pthread_join (thread, NULL);
	cout << "tjoined" << endl;
}

void Sum::Use_n_Pages (int runlen) {

}

void * d_Runit (void * arg) {
	cout << "Inside thread Sum::Runit" << endl;
	d_input *t = (d_input *) arg;
	OrderMaker * ord  = new OrderMaker(t->sch);
	int runlen = 10;
	DBFile dbfile;
	struct {OrderMaker *o; int l;} startup = {ord, runlen};
	char * path = "./temp.bin";
	dbfile.Create (path, sorted, &startup);
	dbfile.MoveFirst ();
	Record * temp = new Record;
	while(t->in->Remove(temp)){
		dbfile.Add(*temp);
	}
	dbfile.MoveFirst ();
	Record recs[2];
	ComparisonEngine comp;
	Record *lasts = NULL, *prevs = NULL;
	int j = 0;
	while (dbfile.GetNext(recs[j%2])) {
		prevs = lasts;
		lasts = &recs[j%2];
		if (prevs && lasts) {
			if (comp.Compare (prevs, lasts, ord) == 0) {
			}
			else{
				t->out->Insert(prevs);
			}
		}
		j++;
	}
	t->out->Insert(lasts);
	t->out->ShutDown();
	cout << "thread done" << endl;
}
void DuplicateRemoval::Run (Pipe &inPipe, Pipe &outPipe, Schema &mySchema) {
	cout << "DuplicateRemoval::Run()" << endl;
	G_d_input = {&inPipe, &outPipe, &mySchema};
  	pthread_create (&thread, NULL, d_Runit ,(void *)&G_d_input);
  
}



void DuplicateRemoval::WaitUntilDone () {
	pthread_join (thread, NULL);
}

void DuplicateRemoval::Use_n_Pages (int runlen) {

}