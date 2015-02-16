#include "BigQ.h"
#include "TwoWayList.cc"

void BigQ:: insert_at_offset(Record * record, int offset, int rec_size){
	p.curSizeInBytes += rec_size;
	p.myRecs->MoveToStart();
	while(offset > 0){
		p.myRecs->Advance();
		offset--;
	}
	p.myRecs->Insert(record);
	p.numRecs++;
}


void  BigQ :: Sorted_insert(Record & record,int start, int end, OrderMaker &sortorder,int rec_size){
	ComparisonEngine ceng; 
	int comp =0;
	while (start < end)
	  {
	  	// cout << start << " " << p.numRecs << " " << end <<  endl;
	    int mid = start + (end-start)/2;
	    p.myRecs->MoveToStart();
	 	comp = ceng.Compare(p.myRecs->Current(mid),&record,&sortorder);
	    if(comp == 0){
	    	insert_at_offset(&record,mid,rec_size);
			return;
	    }
	    if(comp == -1) {
	    	start = mid + 1; 
	 	}
	    else {
	    	end = mid - 1;
	  	}
	}
	if(ceng.Compare(p.myRecs->Current(start),&record,&sortorder) == 1){
		insert_at_offset(&record,start,rec_size);
		// cout << "inserting at " << start << endl;
	}
	else{
		insert_at_offset(&record,start+1,rec_size);
		// cout << "inserting at " << start+1 << endl;
	}
	// cout << "Start : " << start << " End: "<< end << endl;
	
	
	
}


BigQ :: BigQ (Pipe &in, Pipe &out, OrderMaker &sortorder, int runlen) {
	// read data from in pipe sort them into runlen pages

    // construct priority queue over sorted runs and dump sorted data 
 	// into the out pipe

    // finally shut down the out pipe
	sortorder.Print();
	sortorder = sortorder;
	Record curr;
	while (in.Remove (&curr)) {
		if(p.numRecs == 0){
			p.Append(&curr);
		}
		else{
			char *b = curr.GetBits();
			int rec_size = ((int *) b)[0];
			// first see if we can fit the record
			if (p.curSizeInBytes + rec_size > PAGE_SIZE) {
				// code to add new page
				f.AddPage(&p,curpage);
				p.EmptyItOut();
				curpage++;
			}
			else{
				Sorted_insert(curr,0,p.numRecs-1,sortorder,rec_size);
			}
		}
	}
	Record temp;
	p.myRecs->MoveToStart();
	while(p.GetFirst(&temp) == 1){
		out.Insert(&temp);
	}
	out.ShutDown ();
}

BigQ::~BigQ () {
}