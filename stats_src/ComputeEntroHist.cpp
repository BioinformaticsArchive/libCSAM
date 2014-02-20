#include <iostream>
#include <fstream>
#include <cctype>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <algorithm>

using namespace std;

int main (int argc, char *argv[]){
	if(argc ==1){
		cout << "Use: ./ComputeEntroHist <arch> <out_arch>" << endl;
		return 0;
	}

	string filename = argv[1];
	string out_filename = argv[2];


	/*get the Quality fields*/
	string line, qual;
	unsigned long cont = 0;
	unsigned long letters[256];

	for(unsigned int i = 0; i < 256; i++)
		letters[i] = 0;
	
	double entropy = 0;

	ifstream RefFile(filename.c_str());
	if (!RefFile.is_open()){
		cout << "Unable to open file" << endl;
		return 1;
	}
	
	getline(RefFile, line);
	while (RefFile.good() ){
		for(unsigned int i = 0; i < line.length(); i++){
			letters[(int)(line[i])]++;
			cont ++;
		}
		getline(RefFile, line);
	}
	
	RefFile.close();
	
	for(unsigned int i = 0; i < 256; i++)
		if(letters[i] > 0)		
			entropy += ((letters[i]*1.0)/(cont*1.0))*log2((cont*1.0)/(letters[i]*1.0));
	cout << "Entropy: " << entropy << endl;

	ofstream myfile;
	myfile.open (out_filename.c_str());
	for(unsigned int i = 0; i < 256; i++){
		if(letters[i] > 0)
			myfile << i << "\t" << letters[i] << endl;
	}
	myfile.close();
	return 0;
}

