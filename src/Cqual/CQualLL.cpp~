/********************************************************************************
Copyright (c) 2013, Rodrigo Canovas
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the libCSAM nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

********************************************************************************/

#include "CQualLL.h"

/*Compress the quality vector, using gzip*/
CQualLL::CQualLL(vector<string> QUAL, vector<RPP> rpp, cds_word sample){
	string auxGz;
  qualGZ = "";
	sampleRate = sample;
	numberOfLines = rpp.size();
	if(sampleRate == 0){
		SamGZ = NULL;
		for(cds_word i = 0; i < numberOfLines; i++)
			qualGZ += QUAL[(rpp[i]).Permutation] + "\n";                      
		qualGZ = GZcompressString(qualGZ);                        
	}         
	else{               
		SamGZ = new cds_word[1 + ((numberOfLines + sampleRate - 1)/sampleRate)];
		SamGZ[0] = 0;                       
		for(cds_word i = 0; i < numberOfLines; i++){
			auxGz = "";                                     
			for(cds_word j = 0; j < sampleRate && (i + j) < numberOfLines; j++)
				auxGz += QUAL[(rpp[i+j]).Permutation] + "\n";                                           
			i += sampleRate - 1;                                                  
			qualGZ += GZcompressString(auxGz);                                                        
			SamGZ[1 + i/sampleRate] = qualGZ.length();                                                     
		}                           
	}           
}

CQualLL::~CQualLL(){
	if(sampleRate > 0)
		delete [] SamGZ;
}

cds_word CQualLL::getSize() const{
	cds_word mem = sizeof(CQualLL);
	mem += qualGZ.size();
	if(sampleRate > 0)
		mem +=  ((numberOfLines + sampleRate - 1)/sampleRate) * sizeof(cds_word);
	cout << "QualLL: " << (mem  * 1.0 )/ 1048576 << endl;
	return mem;
}
		
/*get quality lines x to y*/
vector<string> CQualLL::getInterval(cds_word x, cds_word y) const{
	vector<string> inte, auxInt;
	string auxS;
	cds_word posSam = 0, posDiff = 0;
	for(cds_word i = x; i <= y; i++){
		posSam = i / sampleRate;
		posDiff = posSam * sampleRate;
		if(posDiff < x)
			posDiff = x - posDiff;
		else
			posDiff = 0;
		/*get blocks of sampleRate size*/
		auxS = qualGZ.substr(SamGZ[posSam], SamGZ[posSam + 1] - SamGZ[posSam]);
		Tokenize(GZdecompressString(auxS), auxInt, "\n");
		for(cds_word j = posDiff; j < sampleRate && (i + j - posDiff) <= y; j++)
			inte.push_back (auxInt[j]);
		i += sampleRate - 1 -  posDiff;
		auxInt.clear();
	}
	return inte;
}

/** Stores the Compressed Quality given a file pointer. */
void CQualLL::Save(ofstream &fp) const{
	SaveValue(fp, LOSSLESS);
	cds_word Variables[2];
	Variables[0] = sampleRate;
	Variables[1] = numberOfLines;
	SaveValue(fp, Variables, 2);
	SaveValue(fp, qualGZ.length());
	SaveValue(fp, (char *)qualGZ.c_str(), qualGZ.length());
	if(sampleRate > 0)
		SaveValue(fp, SamGZ, 1 + (numberOfLines + sampleRate - 1)/sampleRate);
}

string * CQualLL::getAllQual() const{
	string *quals = new string[numberOfLines];
	cds_word posSam = 0;
	string auxS;
	vector<string>  auxInt;
	if(sampleRate == 0){
		Tokenize(GZdecompressString(qualGZ), auxInt, "\n");
		for(cds_word i = 0; i < numberOfLines; i++)
			quals[i] = auxInt[i];
		auxInt.clear();
	}
	else{
		for(cds_word i = 0; i < numberOfLines; i++){
			/*get blocks of sampleRate size*/
			posSam = i/sampleRate;
			auxS = qualGZ.substr(SamGZ[posSam], SamGZ[posSam + 1] - SamGZ[posSam]);
			Tokenize(GZdecompressString(auxS), auxInt, "\n");
			for(cds_word j = 0; j < sampleRate && (i + j) < numberOfLines; j++)
				quals[i + j] = auxInt[j];
			i += sampleRate - 1;
			auxInt.clear();
		}
	}
	return quals;
}

/** Reads a CQual file determining the type. */
CQualLL * CQualLL::Load(ifstream &fp){
	cds_word r = LoadValue<cds_word>(fp);
	cds_word sizeAux = 0;
	char *auxText;
	if (r != LOSSLESS){
		assert(false);
		return NULL;
	}
	CQualLL *cqual = new CQualLL();
	cds_word *Variables = LoadValue<cds_word>(fp, 2);
	cqual->sampleRate = Variables[0];
	cqual->numberOfLines = Variables[1];
	sizeAux = LoadValue<size_t>(fp);
	auxText = LoadValue<char>(fp, sizeAux); 
	(cqual->qualGZ).assign(auxText, sizeAux); 
	delete [] auxText;
	if(cqual->sampleRate > 0){
		sizeAux = 1 + (cqual->numberOfLines + cqual->sampleRate - 1)/cqual->sampleRate;
		cqual->SamGZ = LoadValue<cds_word>(fp, sizeAux);
	}
	delete [] Variables;
	return cqual;
}

CQualLL::CQualLL(){
	sampleRate = 0;
	qualGZ = "";
	SamGZ = NULL;
	numberOfLines = 0;
}


