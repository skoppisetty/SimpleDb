#include "BigQ.h"

OrderMaker G_sortorder;
ComparisonEngine G_comp;

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

int BigQ::Create () {
    try{
    	char tbl_path[100]; // construct path of the tpch flat text file
		sprintf (tbl_path, "bigq.sbin"); 
	    f.Open(0,tbl_path);
	    curpage = 0;
	    totalpages = 1;
	    return 1;
    }
    catch(...){
    	return 0;
    }
}

int BigQ::GetNext (Record &fetchme) {
	if(p.GetFirst(&fetchme)){
		return 1;
	}
	else{
		//cout << "pagechange" << curpage << " " << totalpages << endl;
		if(curpage < totalpages - 2){
			curpage++;
			f.GetPage(&p,curpage);
			if(GetNext(fetchme)){
				return 1;
			}
		}
		return 0;
	}
	
}


int BigQ::savelist (vector<Record *> v) {
	sort(v.begin(),v.end(),mysorter);
	int count = 1;
	for(int i = 0;i < v.size();i++){
		Record * rec = v[i];
		if(!p.Append(rec)){
			if((count)%(runlength+1) != 0){
				f.AddPage(&p, curpage);
				p.EmptyItOut();
				curpage++;totalpages++;
				count++;
				p.Append(rec);
			}
			else{
				cout << "Error" << endl;
				return 0;
			}
		}
	}
	
	return 1;
}

BigQ :: BigQ (Pipe &in, Pipe &out, OrderMaker &sortorder, int runlen) {
	// read data from in pipe sort them into runlen pages

    // construct priority queue over sorted runs and dump sorted data 
 	// into the out pipe

    // finally shut down the out pipe
	G_sortorder = sortorder;
	runlength = runlen;
	cout << Create() << endl;
	vector<Record *> v;
	int curSizeInBytes = 0;
	// priority_queue <Record*, vector<Record*>, Compare> pq;
	while (true) {
		Record * curr = new Record();
		if(in.Remove(curr)){
			// out.Insert(curr);
			char *b = curr->GetBits();
			int rec_size = ((int *) b)[0];
			if (curSizeInBytes + rec_size < (PAGE_SIZE)*runlen) {
				curSizeInBytes += rec_size;
				// pq.push(curr);
				v.push_back(curr);
			}
			else{
				if(!savelist(v)){
					cout << "overflowww";
				}
				v.clear();
				v.push_back(curr);
				curSizeInBytes = rec_size;
				
			}
		}
		else{
			break;
		}
		
	}
	if(!savelist(v)){
		cout << "overflowww";
	}
	v.clear();

	v.clear();
	f.AddPage(&p, curpage);
	totalpages++;


	curpage = 0;
	p.EmptyItOut();

	f.GetPage(&p,curpage);
	Record temp;
	while(GetNext(temp) == 1){
		out.Insert(&temp);
	}


	
	// while (!pq.empty())
	//   {	 
	  	 
	//   	 temp = pq.top();
	//   	 pq.pop();
	//      out.Insert(temp);
	//   }

	out.ShutDown ();
}

BigQ::~BigQ () {
}





// BigQ :: BigQ (Pipe &in, Pipe &out, OrderMaker &sortorder, int runlen) {
// 	// read data from in pipe sort them into runlen pages

//     // construct priority queue over sorted runs and dump sorted data 
//  	// into the out pipe

//     // finally shut down the out pipe
// 	sortorder.Print();
// 	G_sortorder = sortorder;
// 	Record curr;
// 	Record temp;
// 	priority_queue <Record, vector<Record>, Compare> pq;
// 	while (in.Remove(&curr)) {
// 		temp = curr;
// 		//char *b = curr.GetBits();
// 		//int rec_size = ((int *) b)[0];
// 		// temp = curr;
// 		//if (curSizeInBytes + rec_size < (PAGE_SIZE)*runlen) {
// 		pq.push(temp);
// 		//	curSizeInBytes += rec_size;
// 		//}
// 		//else{

// 		//}

// 	}
// 	// 	if(p.numRecs == 0){
// 	// 		p.Append(&curr);
// 	// 	}
// 	// 	else{
// 	// 		char *b = curr.GetBits();
// 	// 		int rec_size = ((int *) b)[0];
// 	// 		// first see if we can fit the record
// 	// 		if (p.curSizeInBytes + rec_size > PAGE_SIZE) {
// 	// 			// code to add new page
// 	// 			f.AddPage(&p,curpage);
// 	// 			p.EmptyItOut();
// 	// 			curpage++;
// 	// 		}
// 	// 		else{
// 	// 			Sorted_insert(curr,0,p.numRecs-1,sortorder,rec_size);
// 	// 		}
// 	// 	}
// 	// }
// 	// Record temp;
// 	// int i = 0;
// 	// while (i<3)
// 	//   {	 
	  	 
// 	//   	 temp = pq.top();
// 	//   	 //pq.pop();
// 	//      out.Insert(&temp);
// 	//      i++;
// 	//   }
// 	// p.myRecs->MoveToStart();
// 	// while(p.GetFirst(&temp) == 1){
// 	// 	out.Insert(&temp);
// 	// }
// 	out.ShutDown ();
// }

// BigQ::~BigQ () {
// }