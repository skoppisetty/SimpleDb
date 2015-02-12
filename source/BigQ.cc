#include "BigQ.h"

BigQ :: BigQ (Pipe &in, Pipe &out, OrderMaker &sortorder, int runlen) {
	// read data from in pipe sort them into runlen pages

    // construct priority queue over sorted runs and dump sorted data 
 	// into the out pipe

    // finally shut down the out pipe

	Record rec[2];
	Record *last = NULL, *prev = NULL;
	ComparisonEngine ceng;
	int err = 0;
	int i = 0;

	while (in.Remove (&rec[i%2])) {
		prev = last;
		last = &rec[i%2];

		if (prev && last) {
			// if (ceng.Compare (prev, last, t->sortorder) == 1) {
			// 	err++;
			// }
			out.Insert (prev);
		}

		i++;
	}
	out.Insert (last);

	cout << " BigQ: removed " << i << " recs from the pipe\n";
	cerr << " BigQ: " << (i - err) << " recs out of " << i << " recs in sorted order \n";
	if (err) {
		cerr << " BigQ: " <<  err << " recs failed sorted order test \n" << endl;
	}



	out.ShutDown ();
}

BigQ::~BigQ () {
}
