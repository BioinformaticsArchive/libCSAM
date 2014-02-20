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

#include "CQualPBlock.h"

CQualPBlock::CQualPBlock(){
	RunLength = NULL;
	ByteSeq = NULL;
	sampleRate = 0;
	SamRL = SamBS = NULL;
}

CQualPBlock::CQualPBlock(vector<string> QUAL, vector<RPP> rpp, int lossyparam, int m, cds_word sample){
	string OccArray = "", QualityArray = "";
	cds_word posSam = 1, lengthSample = 0;
	sizeQualLine = QUAL[0].size();
	LocalMin = "";
	LocalMax = "";
	numberOfValues = 0;
	globalMax = 33; globalMin = 126;
	sampleRate = sample;
	getArrays(QUAL, rpp, OccArray, QualityArray, lossyparam);
	/*case numberOfValues == numberOfLines
	 * Do not store Occ array, local min, local max */
	if(numberOfValues == numberOfLines){
		storeQualityASCII(QualityArray);
		lengthRunLength = 0;
		RunLength = NULL;
		LocalMin.clear();
		LocalMax.clear();
		if(sampleRate > 0){   
			delete [] SamRL;    
			delete [] SamBS;
		}
	}
	else{		
		lengthRunLength = (numberOfValues * 16 + kWordSize -1) / kWordSize; //worse case: each value use 16 bits
		RunLength = new cds_word[lengthRunLength];
		for(cds_word w = 0; w < lengthRunLength; w++)
			RunLength[w] = 0;
		/*compute occ*/
		if(sampleRate > 0)
			lengthSample =  (numberOfLines + sampleRate - 1)/sampleRate;
		lengthRunLength = 0;
		for(cds_word w = 0; w < OccArray.size(); w++){

			lengthRunLength =	encodeGamma((cds_word)(OccArray[w]), RunLength, lengthRunLength);
			if(posSam < lengthSample){
				if((w + 1) == SamRL[posSam]){
					SamRL[posSam] = lengthRunLength;
					posSam ++;
				}
			}
		}
		mode = m;
		if(m != 2){   //should be add inside getArrays
			LocalMin.clear();
			LocalMax.clear();
		}
		switch(mode){
			case 0: storeQualityASCII(QualityArray); break;		
			case 1: storeQualityGlobalmm(QualityArray); break;
			case 2: storeQualityLocalmm(QualityArray, OccArray); break;
			default: throw CDSException("Unknown QualPBLock mode type");
		}
	}
}

void CQualPBlock::getArrays(vector<string> QUAL, vector<RPP> rpp, string& OccArray, string& QualityArray, int lossyparam){
	string qual;
	cds_word valueL = 0, storequal = 0 , line = 0, posSam = 1;
	int occ = 0;
	cds_word min = 0, max = 0, localmin = 0, localmax = 0;
	numberOfLines = rpp.size();
	vector<RPP>::iterator it;
	if(sampleRate > 0){
		SamRL = new cds_word[(numberOfLines + sampleRate - 1)/sampleRate];
		SamBS = new cds_word[(numberOfLines + sampleRate - 1)/sampleRate];
		SamRL[0] = SamBS[0] = 0;
	}
	for(it = rpp.begin(); it != rpp.end(); ++it){
		qual = QUAL[(*it).Permutation];
		localmax = 33;
		localmin = 126;
		for(cds_word w = 0; w < qual.length(); w++){
			valueL = (cds_word)(qual[w]);
			if(min == 0){
				min = max = valueL;
				occ = 1;
			}
			else{
				if(valueL >= min && valueL <= max)
					occ++;
				else{
					if(valueL < min  &&  (max - valueL) <= (cds_word)(2*lossyparam)){
						occ++;
						min = valueL;
					}
					else if(valueL > max  &&  (valueL - min) <= (cds_word)(2*lossyparam)){
						occ++;
						max = valueL;
					}
					else{
						numberOfValues++;
						OccArray += (char)occ;
						storequal = (max + min) / 2;
						QualityArray += (char)(storequal);
						if(storequal < localmin)
							localmin = storequal;
						if(storequal > localmax)
							localmax = storequal;
						if(storequal < globalMin)
							globalMin = storequal;
						if(storequal > globalMax)
							globalMax = storequal;
						min = max = valueL;
						occ = 1;
					}
				}
			}
		}
		numberOfValues++;
		OccArray += (char)occ;
		storequal = (max + min) / 2;
		QualityArray += (char)(storequal);
		if(storequal < localmin)
			localmin = storequal;
		if(storequal > localmax)
			localmax = storequal;
		if(storequal < globalMin)
			globalMin = storequal;
		if(storequal > globalMax)
			globalMax = storequal;
		min = 0;
		LocalMin += (char)localmin;
		LocalMax += (char)localmax;
		if(sampleRate > 0){
			if(((line + 1) % sampleRate) == 0){
				SamRL[posSam] = OccArray.length();
				SamBS[posSam] = QualityArray.length();
				posSam ++;
			}
		}
		line ++;
	}
}


void CQualPBlock::storeQualityASCII(string QualityArray){
	lengthByteSeq = (QualityArray.length() * 8 + kWordSize -1) / kWordSize;
	ByteSeq = new cds_word[lengthByteSeq];
	for(cds_word w = 0; w < lengthByteSeq; w++)
		ByteSeq[w] = 0;
	for(cds_word w = 0; w < QualityArray.length(); w++)
		SetField(ByteSeq, 8, w, (cds_word)(QualityArray[w]));
	lengthByteSeq = QualityArray.length() * 8;
}

void CQualPBlock::storeQualityGlobalmm(string QualityArray){
	cds_word bitsPerValue = (cds_word)ceil(log2(1.0 * (globalMax - globalMin + 1)));
	lengthByteSeq = (QualityArray.length() * bitsPerValue + kWordSize -1) / kWordSize;
	ByteSeq = new cds_word[lengthByteSeq];
	for(cds_word w = 0; w < lengthByteSeq; w++)
		ByteSeq[w] = 0;
	for(cds_word w = 0; w < QualityArray.length(); w++)
		SetField(ByteSeq, bitsPerValue, w, ((cds_word)(QualityArray[w]) - globalMin));
	lengthByteSeq = QualityArray.length() * bitsPerValue;
}

void CQualPBlock::storeQualityLocalmm(string QualityArray, string OccArray){
	/*need to recompute sample*/
	cds_word posSam = 1, lengthSample = 0;
	cds_word qlen = 0, line = 0, min = 0;
	cds_word localbits = (cds_word)ceil(log2(1.0 * (globalMax - globalMin + 1))); //worse case
	lengthByteSeq = (QualityArray.length() * localbits + kWordSize -1) / kWordSize;
	ByteSeq = new cds_word[lengthByteSeq];
	for(cds_word w = 0; w < lengthByteSeq; w++)
		ByteSeq[w] = 0;
	lengthByteSeq = 0;
	if(sampleRate > 0)
		lengthSample =  (numberOfLines + sampleRate - 1)/sampleRate;
	for(cds_word w = 0; w < QualityArray.length(); w++){
		if(qlen  == 0){
			min = (cds_word)(LocalMin[line]);
			localbits = (cds_word)ceil(log2(1.0 * ((cds_word)(LocalMax[line]) - min + 1)));
			line++;
		}
		SetVarField(ByteSeq, lengthByteSeq, lengthByteSeq + localbits - 1, ((cds_word)(QualityArray[w]) - min));
		lengthByteSeq += localbits;
		qlen += (cds_word)(OccArray[w]);
		if(qlen == sizeQualLine)
			qlen = 0;
		if(posSam < lengthSample){
			if((w + 1) == SamBS[posSam]){
				SamBS[posSam] = lengthByteSeq;
				posSam ++;
			}
		}
	}
}

string * CQualPBlock::getAllQual() const{
	cds_word qlen = 0, posRun = 0, run = 0, val = 0, posByte = 0;
	cds_word bitsPerValue = 0;
	if(mode == 1)
		bitsPerValue = (cds_word)ceil(log2(1.0 * (globalMax - globalMin + 1)));
	string *quals = new string[numberOfLines];
	if(numberOfValues == numberOfLines){
		for(cds_word i = 0; i < numberOfLines; i++){
			quals[i] = "";
			val = GetField(ByteSeq, 8, posByte);
			posByte ++;
			quals[i].append(sizeQualLine, (char)val);
		}
	}
	else{
		for(cds_word i = 0; i < numberOfLines; i++){
			quals[i] = "";
			qlen = 0;
			if(mode != 2){
				while(qlen <  sizeQualLine){
					posRun = decodeGamma(&run, RunLength, posRun);
					if(mode == 0)
						val = GetField(ByteSeq, 8, posByte);
					else //mode == 1
						val = globalMin + GetField(ByteSeq, bitsPerValue, posByte);					
					posByte ++;				
					quals[i].append(run, (char)val);
					qlen += run;
				}
			}
			else{ //mode == 2
				bitsPerValue = (cds_word)ceil(log2(1.0 * ((cds_word)(LocalMax[i])  - (cds_word)(LocalMin[i]) +1)));
				while(qlen < sizeQualLine){
					posRun = decodeGamma(&run, RunLength, posRun);
					val = (cds_word)(LocalMin[i]) + GetVarField(ByteSeq, posByte, posByte + bitsPerValue - 1);
					posByte += bitsPerValue;
					quals[i].append(run, (char)val);
					qlen += run;
				}
			}		
		}
	}
	return quals;
}


CQualPBlock::~CQualPBlock(){
	delete [] ByteSeq;
	if(numberOfValues > numberOfLines){
		delete [] RunLength;
		if(sampleRate > 0){
			delete [] SamRL;
			delete [] SamBS;
		}
	}
}

cds_word CQualPBlock::getSize() const{
	cds_word mem = sizeof(CQualPBlock), mem_run = 0, mem_byse = 0, mem_sample = 0;
	/*RunLength*/
	mem_run = ((lengthRunLength + kWordSize -1) / kWordSize) * sizeof(cds_word);
	mem_byse = ((lengthByteSeq + kWordSize -1) / kWordSize) * sizeof(cds_word);
	if(numberOfValues > numberOfLines){
		if(mode == 2)
			mem_byse += 2 * LocalMin.length();
		if(sampleRate > 0)
			mem_sample = 2 * ((numberOfLines + sampleRate -1)/sampleRate) * sizeof(cds_word);
	}
	cout << "Runlength: " << ((mem_run * 1.0) / 1048576) << endl;
	cout << "Representative:" << ((mem_byse * 1.0) / 1048576) << endl;
	return mem + mem_run + mem_byse + mem_sample;
}

vector<string> CQualPBlock::getInterval(cds_word x, cds_word y) const{
	vector<string> block;
	string qual;
	cds_word posRL, posBS, qlen, run, val, line, bitsPerValue = 0;
	/*special case*/
	if(numberOfValues == numberOfLines){
		for(cds_word i = x; i <= y; i++){
			qual = "";
			val = GetField(ByteSeq, 8, i);
			qual.append(sizeQualLine, (char)val);
			block.push_back(qual);
		}
	}
	else{
		/*normal case*/		
		if(sampleRate == 0)
			return block; //empty block
		line = x / sampleRate;
		if(mode == 1)
			bitsPerValue = (cds_word)ceil(log2(1.0 * (globalMax - globalMin + 1)));
		posRL = SamRL[line];
		posBS = SamBS[line];
		line = line * sampleRate;
		while(line < x){
			qlen = 0;         
			if(mode != 2){                  
				while(qlen <  sizeQualLine){
					posRL = decodeGamma(&run, RunLength, posRL);                                    
					posBS ++;
					qlen += run;                                                                  
				}                     
			}
			else{
				bitsPerValue = (cds_word)ceil(log2(1.0 * ((cds_word)(LocalMax[line])  - (cds_word)(LocalMin[line]) +1)));
				while(qlen <  sizeQualLine){
					posRL = decodeGamma(&run, RunLength, posRL);
					posBS += bitsPerValue;
					qlen += run;
				}
			}
			line ++;
		}
		/*Now get the inteval, we will assume that the interval always exist*/
		while(line <= y){
			qlen = 0;
			qual = "";
			if(mode != 2){
				while(qlen <  sizeQualLine){
					posRL = decodeGamma(&run, RunLength, posRL);
					if(mode == 0)
						val = GetField(ByteSeq, 8, posBS);
					else //mode == 1
						val = globalMin + GetField(ByteSeq, bitsPerValue, posBS);
					posBS ++;
					qual.append(run, (char)val);
					qlen += run;
				}
			}
			else{
				bitsPerValue = (cds_word)ceil(log2(1.0 * ((cds_word)(LocalMax[line])  - (cds_word)(LocalMin[line]) +1)));
				while(qlen < sizeQualLine){
					posRL = decodeGamma(&run, RunLength, posRL);
					val = (cds_word)(LocalMin[line]) + GetVarField(ByteSeq, posBS, posBS + bitsPerValue - 1);
					posBS += bitsPerValue;
					qual.append(run, (char)val);
					qlen += run;
				}
			}
			block.push_back(qual);
			line ++;
		}
	}
	return block;
}

void CQualPBlock::Save(ofstream &fp) const{
	SaveValue(fp, PBLOCK);
	SaveValue(fp, mode);
	cds_word Variables[7];
	Variables[0] = sizeQualLine;
	Variables[1] = lengthRunLength;
	Variables[2] = lengthByteSeq;
	Variables[3] = globalMin;
	Variables[4] = globalMax;
	Variables[5] = numberOfValues;
	Variables[6] = numberOfLines;
	SaveValue(fp, Variables, 7);
	SaveValue(fp, ByteSeq, ((lengthByteSeq + kWordSize -1) / kWordSize));
	if(numberOfValues == numberOfLines)
		return;
	SaveValue(fp, RunLength, ((lengthRunLength + kWordSize -1) / kWordSize));
	if(mode == 2){
		SaveValue(fp, LocalMin.length());
		SaveValue(fp, (char *)LocalMin.c_str(), LocalMin.length());
		SaveValue(fp, (char *)LocalMax.c_str(), LocalMax.length());
	}
	SaveValue(fp, sampleRate);
	if(sampleRate > 0){
		SaveValue(fp, SamRL, 1 + ((numberOfLines + sampleRate - 1)/sampleRate));
		SaveValue(fp, SamBS, 1 + ((numberOfLines + sampleRate - 1)/sampleRate));
	}
}


CQualPBlock * CQualPBlock::Load(ifstream &fp){
	cds_word r = LoadValue<cds_word>(fp);
	cds_word locallength = 0, sizeAux = 0;
	char *auxText;
	if (r != PBLOCK) {
		assert(false);
		return NULL;
	}
	CQualPBlock *cqual = new CQualPBlock();
	cqual->mode = LoadValue<int>(fp);
	cds_word *Variables = LoadValue<cds_word>(fp, 7);
	cqual->sizeQualLine = Variables[0];
	cqual->lengthRunLength = Variables[1];
	cqual->lengthByteSeq = Variables[2];
	cqual->globalMin = Variables[3];
	cqual->globalMax = Variables[4];
	cqual->numberOfValues = Variables[5];
	cqual->numberOfLines = Variables[6];
	cqual->ByteSeq = LoadValue<cds_word>(fp, ((cqual->lengthByteSeq + kWordSize -1) / kWordSize));
	if(cqual->numberOfValues > cqual->numberOfLines){
		cqual->RunLength = LoadValue<cds_word>(fp, ((cqual->lengthRunLength + kWordSize -1) / kWordSize));
		if(cqual->mode == 2){
			locallength = LoadValue<size_t>(fp);
			auxText = LoadValue<char>(fp, locallength);
			(cqual->LocalMin).assign(auxText, locallength); delete [] auxText;
			auxText = LoadValue<char>(fp, locallength); 
			(cqual->LocalMax).assign(auxText, locallength); delete [] auxText;
		}
		cqual->sampleRate = LoadValue<cds_word>(fp);
		if(cqual->sampleRate > 0){
			sizeAux = 1 + ((cqual->numberOfLines + cqual->sampleRate - 1)/cqual->sampleRate);
			cqual->SamRL = LoadValue<cds_word>(fp, sizeAux);
			cqual->SamBS = LoadValue<cds_word>(fp, sizeAux);
		}
	}
	delete [] Variables;
	return cqual;
}

