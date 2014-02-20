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
		cout << "Use: ./ComputeAvg <arch> <out_arch>" << endl;
		return 0;
	}

	string filename = argv[1];
	string out_filename = argv[2];


	/*get the Quality fields*/
	string line, qual;
	unsigned long cont = 0;
	double letters[256];
	double stdart[256]; 
	double aux = 0;


	for(unsigned int i = 0; i < 256; i++){
		letters[i] = 0;
		stdart[i] = 0;
	}
	
	ifstream RefFile(filename.c_str());
	if (!RefFile.is_open()){
		cout << "Unable to open file" << endl;
		return 1;
	}
	
	getline(RefFile, line);
	while (RefFile.good() ){
		for(unsigned int i = 0; i < line.length(); i++)
			letters[i] += (int)(line[i]);
		cont ++;
		getline(RefFile, line);
	}
	
	/*compute mean*/
	for(unsigned int i = 0; i < 256; i++)
		letters[i] = letters[i] * 1.0 / cont;

	/*compute std*/
	RefFile.clear() ;
	RefFile.seekg(0, ios::beg) ;
	cont = 0;
	getline(RefFile, line);
	while (RefFile.good() ){
		for(unsigned int i = 0; i < line.length(); i++){
			aux = pow((int)(line[i]) - letters[i], 2.0);  
			stdart[i] += aux;
		}
		cont ++;
		getline(RefFile, line);
	}
	cout << "cont: " << cont << endl;

	RefFile.close();


	ofstream myfile;
	myfile.open (out_filename.c_str());
	for(unsigned int i = 0; i < 256; i++){
		if(letters[i] > 0){
			aux = sqrt(stdart[i] / cont);
			myfile << i << "\t" <<  (letters[i] - 2 * aux) << "\t" << letters[i] << "\t" <<  (letters[i] + 2 * aux)  << endl;
		}
	}
	myfile.close();
	return 0;
}

