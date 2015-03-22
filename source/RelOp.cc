#include "RelOp.h"

// typedef  void* (SelectFile::*sf_Ptr)(void *);
// typedef  void* (*PthreadPtr)(void*);
// PthreadPtr G_p;
sp_input G_sp_input;
sf_input G_sf_input;
p_input G_p_input;
s_input G_s_input;
d_input G_d_input;
j_input G_j_input;
w_input G_w_input;
g_input G_g_input;

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
		// cout << "Num of atts in record " << rec.GetNumAtts() << endl;
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
	char buffer[32];
  	sprintf(buffer, "%1.2f", sum);
	// string str = to_string(sum) + "|";
	// cout << str << endl;

	ostringstream out;
    out << buffer;
    string str = out.str() + "|";
	// char *val = str.c_str();
	// char * cls = "10.20|\n";
	// rec.ComposeRecord(&out_sch,cls);
	rec.ComposeRecord(&out_sch,str.c_str());
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
	dbfile.Close();
	dbfile.Open(path);
	dbfile.MoveFirst ();
	Record temp;
	while(t->in->Remove(&temp)){
		dbfile.Add(temp);
	}
	cout << "Created the file" << endl;
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
				// prevs->Print(t->sch);
				t->out->Insert(prevs);
			}
		}
		j++;
	}
	// lasts->Print(t->sch);
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


void * w_Runit (void * arg) {
	cout << "Inside thread write::Runit" << endl;
	w_input *t = (w_input *) arg;
	Record temp;
	while(t->in->Remove(&temp)){
		int n = t->sch->GetNumAtts();
		Attribute *atts = t->sch->GetAtts();
		// loop through all of the attributes
		for (int i = 0; i < n; i++) {
			// print the attribute name
			fprintf(t->f, "%s: ", atts[i].name);
		

			// use the i^th slot at the head of the record to get the
			// offset to the correct attribute in the record
			int pointer = ((int *) temp.bits)[i + 1];

			// here we determine the type, which given in the schema;
			// depending on the type we then print out the contents
			fprintf(t->f, "[");
			// first is integer
			if (atts[i].myType == Int) {
				int *myInt = (int *) &(temp.bits[pointer]);
				// cout << *myInt;	
				fprintf(t->f, "%d", *myInt);

			// then is a double
			} else if (atts[i].myType == Double) {
				double *myDouble = (double *) &(temp.bits[pointer]);
				fprintf(t->f, "%f", *myDouble);	

			// then is a character string
			} else if (atts[i].myType == String) {
				char *myString = (char *) &(temp.bits[pointer]);
				fprintf(t->f, "%s", myString);	
			} 

			fprintf(t->f, "]");

			// print out a comma as needed to make things pretty
			if (i != n - 1) {
				fprintf(t->f, ",");
			}
		}

		fprintf(t->f, "\n");
	}
	cout << "thread done" << endl;
}
void WriteOut::Run (Pipe &inPipe, FILE *outFile, Schema &mySchema) {
	cout << "WriteOut::Run()" << endl;
	G_w_input = {&inPipe, outFile, &mySchema};
  	pthread_create (&thread, NULL, w_Runit ,(void *)&G_w_input);
  
}



void WriteOut::WaitUntilDone () {
	pthread_join (thread, NULL);
}

void WriteOut::Use_n_Pages (int runlen) {

}

void * g_Runit (void * arg) {
	cout << "Inside thread write::Runit" << endl;
	g_input *t = (g_input *) arg;
	int runlen = 10;
	DBFile dbfile;
	struct {OrderMaker *o; int l;} startup = {t->ord, runlen};
	char * path = "./group_temp.bin";
	dbfile.Create (path, sorted, &startup);
	dbfile.Close();
	dbfile.Open(path);
	dbfile.MoveFirst ();
	Record temp;
	while(t->in->Remove(&temp)){
		dbfile.Add(temp);
	}
	dbfile.MoveFirst ();

	Record recs[2];
	ComparisonEngine comp;
	Record *lasts = NULL, *prevs = NULL;
	int j = 0;
	double sum = 0.0;
	Attribute DA = {"double", Double};
	Schema out_sch ("out_sch", 1, &DA);
	cout << "shit is about to het serious " << endl;
	while (dbfile.GetNext(recs[j%2])) {
		prevs = lasts;
		lasts = &recs[j%2];
		if (prevs && lasts) {
			int ival = 0; 
			double dval = 0.0;
			cout << "error" << endl;
			t->func->Apply(*prevs, ival, dval);
			sum += (ival + dval);
			if (comp.Compare (prevs, lasts, t->ord) == 0) {
				
			}
			else{
				// diff records - generate new record for prevs group
				char buffer[32];
  				sprintf(buffer, "%1.2f", sum);
				ostringstream out;
    			out << buffer;
    			string str = out.str() + "|";
    			cout << str << endl;
				prevs->ComposeRecord(&out_sch,str.c_str());
				prevs->Print(&out_sch);
				sum = 0.0;
				t->out->Insert(prevs);
				// t->out->Insert(prevs);
			}
		}
		j++;
	}
	// generating for the last record
	int ival = 0; 
	double dval = 0.0;
	t->func->Apply(*lasts, ival, dval);
	sum += (ival + dval);
	char buffer[32];
	sprintf(buffer, "%1.2f", sum);
	ostringstream out;
	out << buffer;
	string str = out.str() + "|";
	cout << "Holy shit" << endl;
	lasts->ComposeRecord(&out_sch,str.c_str());
	// lasts->Print(&out_sch);
	cout << str << endl;
	// lasts->Print(t->sch);
	t->out->Insert(lasts);
	t->out->ShutDown();
	cout << "thread done" << endl;

}
void GroupBy::Run (Pipe &inPipe, Pipe &outPipe, OrderMaker &groupAtts, Function &computeMe) {
	cout << "WriteOut::Run()" << endl;
	G_g_input = {&inPipe, &outPipe, &groupAtts, &computeMe};
  	pthread_create (&thread, NULL, g_Runit ,(void *)&G_g_input);
  
}



void GroupBy::WaitUntilDone () {
	pthread_join (thread, NULL);
}

void GroupBy::Use_n_Pages (int runlen) {

}

void * j_Runit (void * arg) {
	cout << "Join::Runit" << endl;
	j_input *t = (j_input *) arg;
	// Create ordermaker
	OrderMaker so1,so2;
	t->cnf->GetSortOrders(so1,so2);

	so1.Print();
	so2.Print();

	DBFile db1,db2;
	int len = 10;
	struct {OrderMaker *o; int l;} startup1 = {&so1, len};
	struct {OrderMaker *o; int l;} startup2 = {&so2, len};

	char *path1 = "./join_left_temp.bin";
	char *path2 = "./join_right_temp.bin";
	remove(path1);
	remove(path2);
	db1.Create (path1, sorted, &startup1);
	db2.Create (path2, sorted, &startup2);
	db1.Close();
	db1.Open(path1);
	db2.Close();
	db2.Open(path2);
	Record temp;
	int l, r;
	cout << " starting merging" << endl;
	// if(t->inL->Remove(&temp)){
	// 	l = temp.GetNumAtts();
	// 	db1.Add(temp);
	// }
	// if(t->inR->Remove(&temp)){
	// 	r = temp.GetNumAtts();
	// 	db2.Add(temp);
	// }
	while(t->inL->Remove(&temp)){
		l = temp.GetNumAtts();
		cout << ".";
		db1.Add(temp);
		cout << "*" ;
	}
	cout << "One file records done" << endl;
	while(t->inR->Remove(&temp)){
		r = temp.GetNumAtts();
		cout << ".";
		db2.Add(temp);
		cout << "*" ;
	}
	cout << "records added" << endl;
	int A[l+r];
	cout << "Size :" <<  l+r << endl;
	int i = 0;
	for(i = 0;i<l;i++){
		A[i] = i;
	}
	for(int k=0;k<r;k++){
		A[i]=k;
		i++;
	}
	for(int k = 0;k<i;k++){
		cout << A[k] << "\t" ;
	}
	cout << "Adding records" << endl;
	cout << "Add Done" << endl;
	db1.MoveFirst();
	
	cout << "first mode" << endl;
	db2.MoveFirst();
	cout << "second" << endl;
	ComparisonEngine comp;
	Record rec1,rec2;
	cout << "starting comparison merger" << endl;
	int flag1 = 0;
	int flag2 = 0;
	if(db1.GetNext(rec1) && db2.GetNext(rec2)){
		flag1 = 1;
		flag2= 1;
	}
	while(true){
		if(flag1 && flag2){
			int res = comp.Compare(&rec1,&so1,&rec2,&so2);
			if(res == 0){
				// merge
				Record *temp = new Record();
				temp->MergeRecords (&rec1,&rec2,l,r,A,l+r-(so2.numAtts),l);		
				// push to output
				cout << "pushing" << endl;
				t->out->Insert(temp);
				if(!(db1.GetNext(rec1) && db2.GetNext(rec2))){
					flag1 = 0;
					flag2= 0;
				}
			}
			else if(res == -1){
				if(!db1.GetNext(rec1)) {flag1 = 0; break;}
			}
			else{
				if(!db2.GetNext(rec2)) {flag2 = 0; break;}
			}
		}
		else{
			break;
		}
	}
	cout << "done" << endl;
	t->out->ShutDown();
	cout << "thread done" << endl;
}

void Join::Run (Pipe &inPipeL, Pipe &inPipeR, Pipe &outPipe, CNF &selOp, Record &literal) {
	cout << "Join::Run()" << endl;
	G_j_input = {&inPipeL, &inPipeR, &outPipe, &selOp, &literal};
  	pthread_create (&thread, NULL, j_Runit ,(void *)&G_j_input);
  
}

void Join::WaitUntilDone () {
	pthread_join (thread, NULL);
}

void Join::Use_n_Pages (int runlen) {

}