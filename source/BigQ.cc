#include <cmath>
#include "BigQ.h"

OrderMaker G_sortorder;
ComparisonEngine G_comp;
static int G_runlen=0;
Schema mySchema ("../source/catalog", "region");

class block
{	
public:
	block(){
		count = 0;
	}
	Record rec;
	Page p;
	int curpage;
	int count;
	int getRecord(){
		// cout << "getting records" << endl;
		if(!p.GetFirst(&rec)){
			return 0;
		}
		// cout << "printing record" << endl;
		// rec.Print(&mySchema);
		return 1;
	}
};

typedef enum {source, destination} merger_type;

typedef enum {initial, merge_done, done, finish,  first_done , second_done , compare, read_first, read_second} merger_state;


merger_state block_state = initial;

class merger
{	
public:
	merger(){
		curpage = 0;
		totalpages = 1;
	}
	File f;
	Page p;
	int curpage;
	int totalpages;
	block *first;
	block *second;
	merger_type mtype;
	int loadpage(block *node){
		if(node->curpage < (totalpages-1) && node->count > 0){
			f.GetPage(&node->p,node->curpage);
			node->count--;
			return 1;
		}
		return 0;
	}
	int AddRec(Record &rec){
		// cout << "add" << endl;
		// rec.Print(&mySchema);
		if(!p.Append(&rec)){
			// cout << "new page" << "\n";
			f.AddPage(&p, curpage);
			p.EmptyItOut();
			curpage++;totalpages++;
			p.Append(&rec);
		}
	}
};

class Compare{
	public:
	bool operator()(Record * R1, Record *R2)
	{
	    if((G_comp).Compare(R1,R2,&(G_sortorder)) == -1){
	    	return true;
	    }
	    else{
	    	return false;
	    }
	}
}mysorter;

int Create (char* s, merger *merge_file) {
    try{
	    merge_file->f.Open(0,s);
	    merge_file->curpage = 0;
	    merge_file->totalpages = 1;
	    return 1;
    }
    catch(...){
    	return 0;
    }
}

int GetNext (Record &fetchme, merger *merge_file) {
	if(merge_file->p.GetFirst(&fetchme)){
		return 1;
	}
	else{
		if(merge_file->curpage < merge_file->totalpages - 2){
			merge_file->curpage++;
			merge_file->f.GetPage(&merge_file->p,merge_file->curpage);
			if(GetNext(fetchme,merge_file)){
				return 1;
			}
		}
		return 0;
	}
	
}

int SendAll(merger *merge_file, BigQ_input *t){
	merge_file->curpage = 0;
	merge_file->p.EmptyItOut();
	merge_file->f.GetPage(&(merge_file->p),merge_file->curpage);
	Record temp;
	while(GetNext(temp,merge_file) == 1){
		t->out->Insert(&temp);
	}

}

int savelist (vector<Record *> v, merger *merge_file) {
	sort(v.begin(),v.end(),mysorter);
	int count = 1;
	for(int i = 0;i < v.size();i++){
		Record * rec = v[i];
		if(!merge_file->p.Append(rec)){
			if((count)%(G_runlen+1) != 0){
				merge_file->f.AddPage(&(merge_file->p), merge_file->curpage);
				merge_file->p.EmptyItOut();
				merge_file->curpage++;merge_file->totalpages++;
				count++;
				merge_file->p.Append(rec);
			}
			else{
				cout << "Error" << endl;
				return 0;
			}
		}
	}
	
	return 1;
}


void *TPMMS (void *arg) {
	
	BigQ_input *t = (BigQ_input *) arg;
	
	merger *first_file = new merger();
	merger *second_file = new merger();
	char first_path[100]; 
	char second_path[100];// construct path of the tpch flat text file
	sprintf (first_path, "Bigq.bin"); 
	cout << Create(first_path,first_file) << endl;
	sprintf (second_path, "Bigq_temp.bin"); 
	cout << Create(second_path,second_file) << endl;


	vector<Record *> v;
	int curSizeInBytes = 0;
	int Runs=0;
	// priority_queue <Record*, vector<Record*>, Compare> pq;
	while (true) {
		Record * curr = new Record();
		if(t->in->Remove(curr)){
			// out.Insert(curr);
			char *b = curr->GetBits();
			int rec_size = ((int *) b)[0];
			if (curSizeInBytes + rec_size < (PAGE_SIZE)*G_runlen) {
				curSizeInBytes += rec_size;
				// pq.push(curr);
				v.push_back(curr);
			}
			else{
				if(!savelist(v,first_file)){
					cout << "overflowww";
				}
				Runs++;
				v.clear();
				v.push_back(curr);
				curSizeInBytes = rec_size;				
			}
		}
		else{
			break;
		}
		
	}
	if(!v.empty()){
		Runs++;
	}
	if(!savelist(v,first_file)){
		cout << "overflowww";
	} 
	v.clear();
	first_file->f.AddPage(&first_file->p,first_file->curpage);
	first_file->totalpages++;

	cout << "Initial" << first_file->totalpages << endl;
	// SendAll(first_file,t);

	// TPMMS CODE
	first_file->mtype = source;
	second_file->mtype = destination;
	block *first = new block();
	first->curpage = 0;
	block *second = new block();
	second->curpage = G_runlen;
	first_file->first = first;
	first_file->second = second;

	int cur_runlength = G_runlen;
	
	merger * source_file = first_file;
	merger * destination_file = second_file;
	cout << "Initial start" << endl;
	while(block_state != merge_done){
		switch(block_state){
			case initial:
				// cout << "Initial" << endl;
				if(source_file->mtype != source){
					cout << "error" << endl;
					exit(-1);
				}
				source_file->first->count = cur_runlength;
				source_file->second->count = cur_runlength;
				source_file->loadpage(first);
				// if(!source_file->loadpage(second)){
				// 	block_state = second_done;
				// 	break;
				// }
				if(!source_file->first->getRecord()){
					source_file->first->curpage++;
					if(!source_file->loadpage(first)){
						block_state = first_done;
						break;
					}
					else{
						if(!source_file->first->getRecord()){
							block_state = first_done;
							break;
						}
					}
				}
				// source_file->first->rec.Print(&mySchema);
				if(!source_file->second->getRecord()){
					source_file->second->curpage++;
					if(!source_file->loadpage(second)){
						block_state = second_done;
						break;
					}
					else{
						if(!source_file->second->getRecord()){
							block_state = second_done;
							break;
						}
					}
				}
				block_state = compare;
				break;
			
			case compare:
				// cout << block_state << endl;
				if((G_comp).Compare(&source_file->first->rec,&source_file->second->rec,&(G_sortorder)) == -1){
					destination_file->AddRec(source_file->first->rec);
					block_state = read_first;
				}
				else{
					destination_file->AddRec(source_file->second->rec);
					block_state = read_second;
				}
				break;

			case read_first:
				// cout << block_state << endl;
				if(!source_file->first->getRecord()){
					source_file->first->curpage++;
					if(!source_file->loadpage(first)){
						block_state = first_done;
						break;
					}
					else{
						if(!source_file->first->getRecord()){
							block_state = first_done;
							break;
						}
					}
				}
				block_state = compare;
				break;

			case read_second:
				// cout << block_state << endl;
				if(!source_file->second->getRecord()){
					source_file->second->curpage++;
					if(!source_file->loadpage(second)){
						block_state = second_done;
						break;
					}
					else{
						if(!source_file->second->getRecord()){
							block_state = second_done;
							break;
						}
					}
				}
				block_state = compare;
				break;
			
			case first_done:
				// cout << block_state << endl;
				destination_file->AddRec(source_file->second->rec);
				if(!source_file->second->getRecord()){
					source_file->second->curpage++;
					if(!source_file->loadpage(second)){
						block_state = done;
						break;
					}
					else{
						if(!source_file->second->getRecord()){
							block_state = done;
							break;
						}
					}
				}
				block_state = first_done;
				break;

			case second_done:
				// cout << "second_done" << endl;
				destination_file->AddRec(source_file->first->rec);
				if(!source_file->first->getRecord()){
					source_file->first->curpage++;
					if(!source_file->loadpage(first)){
						block_state = done;
						break;
					}
					else{
						if(!source_file->first->getRecord()){
							block_state = done;
							break;
						}
					}
				}
				// cout << "adding records" << endl;
				block_state = second_done;
				break;

			case done:
				// cout << "Done" << endl;
				// cout << source_file->first->curpage << " " << source_file->totalpages-2 << endl;
				if(source_file->first->curpage >= source_file->totalpages-2){
					block_state = finish;
					break;
				}
				source_file->first->curpage += cur_runlength;
				source_file->second->curpage += cur_runlength;
				block_state = initial;
				break;
			
			case finish:
				// cout << "Finish" << endl;
				// cout << cur_runlength << " " << source_file->first->curpage << " " << source_file->totalpages-2 << endl;
				if(cur_runlength >= source_file->totalpages-2 ){
					block_state = merge_done;
					destination_file->f.AddPage(&destination_file->p,destination_file->curpage);
					destination_file->totalpages++;
					break;
				}
				else{
					destination_file->f.AddPage(&destination_file->p,destination_file->curpage);
					destination_file->totalpages++;
					merger * temp = source_file;
					source_file = destination_file;
					destination_file = temp;
					source_file->mtype = source;
					source_file->curpage = 0;
					destination_file->curpage = 0;
					destination_file->mtype = destination;
					cur_runlength = 2 * cur_runlength;
					block_state = initial;					
				}
				break;

		}
		

	}

	cout << "Its done finally" << destination_file->totalpages << endl;
	SendAll(destination_file,t);
	t->out->ShutDown ();
}

BigQ :: BigQ (Pipe &in, Pipe &out, OrderMaker &sortorder, int runlen) {
	// read data from in pipe sort them into runlen pages

 //    construct priority queue over sorted runs and dump sorted data 
 // 	into the out pipe

 //    finally shut down the out pipe
	G_sortorder = sortorder;
	G_runlen = runlen;
	BigQ_input input = {&in, &out};

	pthread_t thread;
	pthread_create (&thread, NULL, TPMMS , (void *)&input);

	pthread_join (thread, NULL);

}


BigQ::~BigQ () {
}

