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

#include "CQualBin.h"

CQualBin::CQualBin(vector<string> QUAL, vector<RPP> rpp, int lossyparam, int m, cds_word sample){
	string qualBinArray = "";
	cds_word lengthSample = 0, posSam = 1;
	sampleRate = sample;
	mode = m;
	numberOfBins = lossyparam;
	sizeQualLine = QUAL[0].size(); //all lines must have same length
	getArrays(QUAL, rpp, qualBinArray);
	/*apply huffman over qualBinArray and store*/
	huffBin = new Huffman(qualBinArray);
	lengthBitSeq = (qualBinArray.size() * 8 + kWordSize -1) / kWordSize; //worse case: each value use 8 bits
	BitSeq = new cds_word[lengthBitSeq];
	for(cds_word w = 0; w < lengthBitSeq; w++)
		BitSeq[w] = 0;
	if(sampleRate > 0)
		lengthSample =  (numberOfLines + sampleRate - 1)/sampleRate;
	lengthBitSeq = 0;
	for(cds_word w = 0; w < qualBinArray.size(); w++){
		lengthBitSeq = huffBin->encode((cds_uint)(qualBinArray[w]), BitSeq, lengthBitSeq);
		if(posSam < lengthSample){
			if((w + 1) == SamBin[posSam]){
				SamBin[posSam] = lengthBitSeq;
				posSam ++;
			}
		}
	}
}

CQualBin::~CQualBin(){
	delete [] Bins;
	delete [] BitSeq;
	if(sampleRate > 0)
		delete [] SamBin;
}

cds_word CQualBin::getSize() const{
	cds_word mem = sizeof(CQualBin);
	mem += ((lengthBitSeq + kWordSize -1) / kWordSize) * sizeof(cds_word);
	mem += huffBin->getSize();
	mem += numberOfBins * sizeof(int);
	if(sampleRate > 0)
		mem += ((numberOfLines + sampleRate -1)/sampleRate) * sizeof(cds_word);
	return mem;
}
		
/*get quality lines x to y*/
vector<string> CQualBin::getInterval(cds_word x, cds_word y) const{
	vector<string> inte;
	string qual;
	cds_word posBS, qlen;
	cds_uint val;
	cds_word line = x / sampleRate;
	if(sampleRate == 0)
		return inte; //empty blockreturn inte;
	posBS = SamBin[line];
	line = line * sampleRate;
	/*move to the x-th line*/
	while(line < x){
		qlen = 0;
		while(qlen <  sizeQualLine){
			posBS = huffBin->decode(&val, BitSeq, posBS);	
			qlen ++;
		}		
		line ++;
	}
	/*Now get the inteval, we will assume that the interval always exist*/
	while(line <= y){
		qlen = 0;
		qual = "";
		while(qlen <  sizeQualLine){
			posBS = huffBin->decode(&val, BitSeq, posBS);
			qual += (char)(Bins[val] + 33);
			qlen ++;
		}
		inte.push_back(qual);
		line ++;
	}
	return inte;
}

string * CQualBin::getAllQual() const{
	cds_word posByte = 0;
	cds_uint val = 0;
	string *quals = new string[numberOfLines];
	for(cds_word i = 0; i < numberOfLines; i++){
		quals[i] = "";
		for(cds_word j = 0; j < sizeQualLine; j++){
			posByte = huffBin->decode(&val, BitSeq, posByte);
			quals[i] += (char)(Bins[val] + 33);
		}
	}
	return quals;
}

/** Stores the Compressed Quality given a file pointer. */
void CQualBin::Save(ofstream &fp) const{
	SaveValue(fp, BINS);
	SaveValue(fp, mode);
	SaveValue(fp, numberOfBins);
	SaveValue(fp, Bins, numberOfBins);
	SaveValue(fp, sizeQualLine);
	SaveValue(fp, lengthBitSeq);
	SaveValue(fp, numberOfLines);
	SaveValue(fp, BitSeq, ((lengthBitSeq + kWordSize -1) / kWordSize));
	huffBin->Save(fp);
	SaveValue(fp, sampleRate);
	if(sampleRate > 0)
		SaveValue(fp, SamBin, 1 + ((numberOfLines + sampleRate - 1)/sampleRate));
}


/** Reads a CQual file determining the type. */
CQualBin * CQualBin::Load(ifstream &fp){
	cds_word r = LoadValue<cds_word>(fp);
	cds_word sizeAux = 0;
	if (r != BINS) {
		assert(false);
		return NULL;
	}
	CQualBin *cqual = new CQualBin();
	cqual->mode = LoadValue<int>(fp);
	cqual->numberOfBins = LoadValue<int>(fp);
	cqual->Bins = LoadValue<int>(fp,  cqual->numberOfBins);
	cqual->sizeQualLine = LoadValue<cds_word>(fp);
	cqual->lengthBitSeq = LoadValue<cds_word>(fp);
	cqual->numberOfLines  = LoadValue<cds_word>(fp);
	cqual->BitSeq = LoadValue<cds_word>(fp, ((cqual->lengthBitSeq + kWordSize -1) / kWordSize));
	cqual->huffBin = Huffman::Load(fp);
	cqual->sampleRate = LoadValue<cds_word>(fp);
	if(cqual->sampleRate > 0){
		sizeAux = 1 + ((cqual->numberOfLines + cqual->sampleRate - 1)/cqual->sampleRate);
		cqual->SamBin = LoadValue<cds_word>(fp, sizeAux);
	}
	return cqual;
}

void CQualBin::getArrays(vector<string> QUAL, vector<RPP> rpp, string& qualBinArray){	
	cds_word valueQ;
	double div = 1.0;
	cds_word bin = 0;
	double *table = createTable();
	string qual;
	cds_word line = 0, posSam = 1;
	vector<RPP>::iterator it;
	numberOfLines = rpp.size();
	if(sampleRate > 0){
		SamBin = new cds_word[(numberOfLines + sampleRate - 1)/sampleRate];
		SamBin[0] = 0;
	}
	//create bin array
	if(mode == 0){
		if(numberOfBins > 94)
			numberOfBins = 94;
		div = 94.0 / numberOfBins; //94 possible Log values
	}
	else      
		div = 1.0 / numberOfBins; //percentage value
	cout << "div: " << div << endl;
	Bins = new int[numberOfBins];
	for(int i = 0; i < numberOfBins; i ++)
		Bins[i] = 126;  //higher possible quality value
	for(it = rpp.begin(); it != rpp.end(); ++it){
		qual = QUAL[(*it).Permutation];
		/*check the qual line*/
		for(cds_word w = 0; w < qual.length(); w++){
			valueQ = (cds_word)(qual[w]) - 33;
			//get bin of the quality value
			if(mode == 0)
				bin = (int)(valueQ / div);
			else 
				bin = (int)(table[valueQ] / div);
			qualBinArray += (char)bin;
			if(Bins[bin] > (int)valueQ)
				Bins[bin] = valueQ;
		}
		if(sampleRate > 0){
			if(((line + 1) % sampleRate) == 0){
				SamBin[posSam] = qualBinArray.length();
				posSam ++;
			}
		}
		line ++;
	}
	delete [] table;
}

double * CQualBin::createTable(){
	double *table = new double[94];
	for(int i = 0; i < 94; i++)
		table[i] = pow (10.0,(( - i * 1.0)/10.0));
	return table;
}

CQualBin::CQualBin(){
	Bins = NULL;
	SamBin= NULL;
	BitSeq = NULL;
}

