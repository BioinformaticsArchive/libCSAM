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

#include "CQualNStore.h"

CQualNStore::CQualNStore(){
}

CQualNStore::CQualNStore(vector<string> QUAL, vector<RPP> rpp, int m){
	sizeQualLine = QUAL[0].size();
	vector<RPP>::iterator it;
	numberOfLines = rpp.size();
	switch(m){
			case 0: QualValue = getPQualValue(QUAL, rpp);  break;		
			case 1: QualValue = getRQualValue(QUAL, rpp);  break;
			default: throw CDSException("Unknown mode type");
	}
}

cds_word CQualNStore::getPQualValue(vector<string> QUAL, vector<RPP> rpp){
	vector<RPP>::iterator it;
	string qual;
	cds_word max, min, valueL;
	cds_word res = 0, cont = 0;
	for(it = rpp.begin(); it != rpp.end(); ++it){
		qual = QUAL[(*it).Permutation];
		max = 33;
		min = 126;
		for(cds_word w = 0; w < qual.length(); w++){
			valueL = (cds_word)(qual[w]);
			if(valueL < min)
				min = valueL;
			if(valueL > max)
				max = valueL;
		}
		cont ++;
		res += (max + min) / 2;
	}
	return res / cont;
}

cds_word CQualNStore::getRQualValue(vector<string> QUAL, vector<RPP> rpp){
	vector<RPP>::iterator it;  
	string qual;
	cds_word max, min, valueL;
	cds_word res = 0, cont = 0;
	for(it = rpp.begin(); it != rpp.end(); ++it){       
		qual = QUAL[(*it).Permutation];           
		max = 33;
		min = 126;
		for(cds_word w = 0; w < qual.length(); w++){                    
			valueL = (cds_word)(qual[w]);                         
			if(valueL < min)
				min = valueL;
			if(valueL > max)
				max = valueL;
		}             
		cont ++;
		res += (cds_word)( sqrt((min - 33) * (max - 33)) + 33);
	}
	return res / cont;
}



string * CQualNStore::getAllQual() const{
	string qual = "";
	string *quals = new string[numberOfLines];
	cout << qual << endl;
	qual.append(sizeQualLine, (char)QualValue);
	for(cds_word i = 0; i < numberOfLines; i++)
		quals[i] = qual;
	return quals;
}


CQualNStore::~CQualNStore(){
}

cds_word CQualNStore::getSize() const{
	cds_word mem = sizeof(CQualNStore); 
	return mem;
}

vector<string> CQualNStore::getInterval(cds_word x, cds_word y) const{
	vector<string> block;
	string qual = "";
	qual.append(sizeQualLine, (char)QualValue);
	for(cds_word i = x; i <= y; i++)
		block.push_back(qual);
	return block;
}

void CQualNStore::Save(ofstream &fp) const{
	SaveValue(fp, NSTORE);
	cds_word Variables[3];
	Variables[0] = sizeQualLine;
	Variables[1] = numberOfLines;
	Variables[2] = QualValue;
	SaveValue(fp, Variables, 3);
}


CQualNStore * CQualNStore::Load(ifstream &fp){
	cds_word r = LoadValue<cds_word>(fp);
	if (r != NSTORE) {
		assert(false);
		return NULL;
	}
	CQualNStore *cqual = new CQualNStore();
	cds_word *Variables = LoadValue<cds_word>(fp, 3);
	cqual->sizeQualLine = Variables[0];
	cqual->numberOfLines = Variables[1];
	cqual->QualValue = Variables[2];
	delete [] Variables;
	return cqual;
}

