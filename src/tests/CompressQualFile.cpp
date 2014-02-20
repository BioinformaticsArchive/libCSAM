

#include <iostream>
#include <fstream>
#include <cctype>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <algorithm>
#include "./../SAM.h" 
#include "./../Cqual/CQual.h"

int main (int argc, char *argv[]){
	CQual *cqual;
	int lossyparameter = 0;
	cds_word sample = 0;
	cds_word qualmode = 0;
	cds_word mode = 1;
	bool reorder = false;

	if(argc ==1){
		cout << "Use: ./CompressQualFie <arch>.qual <opt>" << endl;
		cout << "opt: " << endl;
		cout << "-q qm: How the Quality values are stored. qm=0 gzip, qm=1 pblock, qm=2 rblock, qm=3 bins, qm=4 no store. Default: qm 0" << endl;
		cout << "-m mode: If qm is 1 or 2, give the mode use to store the Representative Array. mode=0 ASCII, mode=1 Binary Global, mode=2 Binary Local. Default mode=1" << endl;
		cout << "-s sample:  size of the sample rate that will be use. Default: no sample" << endl;
		cout << "-l lossy: lossy parameter use to compress the quality score depending on the mode use. Default: 0" << endl; 
		return 0;
	}

	string filename = argv[1];

	int c;
	while((c = getopt (argc, argv, "q:l:s:m:")) != -1){
		switch (c){
			case 'q':	qualmode = atoi(optarg); 	break;
			case 's':	sample = atoi(optarg);	break;
			case 'm': mode = atoi(optarg); break;
			case 'l': lossyparameter = atoi(optarg); break;
			case '?': if(optopt == 's') fprintf (stderr, "Option -%c requires an argument.\n", optopt);
									else if(optopt == 'q') fprintf (stderr, "Option -%c requires an argument.\n", optopt);
									else if(optopt == 'm') fprintf (stderr, "Option -%c requires an argument.\n", optopt);
									else if(optopt == 'l') fprintf (stderr, "Option -%c requires an argument.\n", optopt);
									else if(isprint (optopt)) fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				        else fprintf(stderr,"Unknown option character `\\x%x'.\n",	optopt);
				        return 1;
			default:	abort ();
		}
	}

	/*get the Quality fields*/
	string qual;
	cds_word permutation = 0;
	vector<string> QUAL;
	vector<RPP> rpp;
	ifstream QualFile(filename.c_str());
	if (!QualFile.is_open()){
		cout << "Unable to open file" << endl;
		return 1;
	}
	getline(QualFile, qual);
	while (QualFile.good() ){
		QUAL.push_back(qual);
		pushRPP(rpp, "", 0, permutation);
		permutation ++;
		getline (QualFile, qual);
	}
	QualFile.close();
	/*create CQual data structure*/	
	switch(qualmode){
		case 0: cqual = new CQualLL(QUAL, rpp, sample); break;
		case 1: cqual = new CQualPBlock(QUAL, rpp, lossyparameter, mode, sample); break;
		case 2: cqual = new CQualRBlock(QUAL, rpp, lossyparameter, mode, sample); break;
		case 3: cqual = new CQualBin(QUAL, rpp, lossyparameter, 0, sample); break;
		case 4: cqual = new CQualNStore(QUAL, rpp, lossyparameter); break;
		default: cout << "Error: Qual mode selected do not exist" << endl; exit(0);
	}
	QUAL.clear();
	cout << "Size CQual in MB: " <<  ((cqual->getSize()*1.0)/1048576) << endl;

	ofstream fileCQual;
	string file = ".cqual";
	file = filename + file;
	fileCQual.open(file.c_str());
	/*save Cqual file*/
	SaveValue(fileCQual, permutation);
	cqual->Save(fileCQual);
	SaveValue(fileCQual, reorder);
	if(reorder){
		/*save Permutation*/
		cds_word bitsPer =  (cds_word)(ceil(log2(permutation)));
		cds_word lengthPer = (bitsPer * permutation + kWordSize - 1) / kWordSize;
		cds_word *Permutation = new cds_word[lengthPer];
		for(cds_word i = 0; i < lengthPer; i++)
			Permutation[i] = 0;
		for(cds_word i = 0; i < permutation; i++)
			SetField(Permutation, bitsPer, i, (rpp[i]).Permutation);

		delete [] Permutation;
		SaveValue(fileCQual, Permutation, lengthPer);
		cout << "Size Permutation in MB: " <<  ((lengthPer*sizeof(cds_word)*1.0)/1048576) << endl;
	}
	fileCQual.close();
	
	delete cqual;
	return 0;
}

