#include "./../Cqual/CQual.h"


int main (int argc, char *argv[]){
	CQual *cqual;
	cds_word *Permutation = NULL;
	cds_word *inv_per = NULL;
	cds_word lengthPer = 0, NumberofLines = 0, bitsPer = 0; 
	bool reorder = false;
	string filename, file = ".qual";
	string  *strCqual;
	string auxQual = "";
	if(argc!=2){
		cout << "Use: ./DecompressQual <arch>" << endl;
		return 0;
	}
	/*load info*/
	ifstream fileQual;
	filename = argv[1];
	fileQual.open(argv[1]);
	NumberofLines = LoadValue<cds_word>(fileQual);
	cqual = CQual::Load(fileQual);
	cout << "Size in MB: " <<  ((cqual->getSize()*1.0)/1048576) << endl;
	reorder = LoadValue<bool>(fileQual);
	if(reorder){
		bitsPer =  (cds_word)(ceil(log2(NumberofLines)));
		lengthPer = (bitsPer * NumberofLines + kWordSize - 1) / kWordSize;
		Permutation = LoadValue<cds_word>(fileQual, lengthPer);
		inv_per = new cds_word[NumberofLines];
		for(cds_word i = 0; i < NumberofLines; i++)
			inv_per[GetField(Permutation, bitsPer, i)] = i;
	}
	fileQual.close();
	
	/*generate .qual file*/
	ofstream fileDecomQual;
	file = filename + file;
	fileDecomQual.open(file.c_str());
	strCqual = cqual->getAllQual();
	if(reorder){
		for(cds_word i = 0; i < NumberofLines; i++)
			auxQual += strCqual[inv_per[i]] + "\n";
	}
	else{
		for(cds_word i = 0; i < NumberofLines; i++)   
			auxQual += strCqual[i] + "\n";
	}
	SaveValue(fileDecomQual, (char *)auxQual.c_str(), auxQual.length());
	
	fileDecomQual.close();
	
	delete cqual;
	delete [] strCqual;
	if(reorder){
		delete [] Permutation;
		delete [] inv_per;
	}
	return 0;
}

