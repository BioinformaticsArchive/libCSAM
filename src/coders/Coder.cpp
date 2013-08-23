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

#include "Coder.h"


//symb > 0 always!!
cds_word encodeUnary(cds_word symb, cds_word *stream, cds_word pos){
	cds_word zeros = 0;  //set all bits in 0
	cds_word newPos = 0;
	if(symb == 1){
		BitOne(stream, pos);
		newPos = pos + 1;
	}
	else{
		SetVarField(stream, pos, (pos + symb - 2), zeros);
		BitOne(stream, pos + symb - 1);
		newPos = pos + symb;
	}
	return newPos;
}

cds_word decodeUnary(cds_word *symb, cds_word *stream, cds_word pos){
	//this should be improve using pop-count or just implementing a 
	//method to fast look for the next 1
	cds_word newPos = pos;
	cds_word x = 1;
	while(!BitGet(stream, newPos)){
		x++;
		newPos++;
	}
	*symb = x;
	return newPos + 1;
}

//symb > 0 always!!
cds_word encodeGamma(cds_word symb, cds_word *stream, cds_word pos){
	cds_word lenbit, x;
	cds_word newPos = pos;
	if(symb == 1){
		BitOne(stream, pos);
		newPos++;
	}
	else{
		lenbit = (cds_word)(floor(log2(symb)));
		newPos = encodeUnary(lenbit + 1, stream, pos);
		x = GetVarField(&symb, 0, lenbit-1);
		SetVarField(stream, newPos, (newPos + lenbit-1), x);
		newPos += lenbit;
	}
	return newPos;
}

cds_word decodeGamma(cds_word *symb, cds_word *stream, cds_word pos){
	cds_word lenbit = 0;
	cds_word newPos = 0;
	cds_word x;
	if(BitGet(stream, pos)){
		*symb = 1;
		newPos = pos + 1;
	}
	else{
		newPos = decodeUnary(&lenbit, stream, pos); 
		x =  GetVarField(stream, newPos, newPos + lenbit - 2);
		*symb = (1<<(lenbit-1)) + x ;
		newPos +=  (lenbit - 1);
	}
	return newPos;
}

//we will assume that m is 2^k for k > 0 STILL NOT TESTED
cds_word encodeGolomb(cds_word symb, cds_word *stream, cds_word pos, cds_word m){
	cds_word newPos = 0;
	cds_word q = (cds_word)(floor(symb / m));
	cds_word r = symb - q * m;
	cds_word b = (cds_word)(log2(m));
	newPos = encodeUnary(q + 1, stream, pos);
	SetVarField(stream, newPos, (newPos + b - 1), r);
	newPos += b;
	return newPos;
}


cds_word decodeGolomb(cds_word *symb, cds_word *stream, cds_word pos, cds_word m){
	cds_word newPos = 0;
	cds_word q = 0, r = 0;
	cds_word b = (cds_word)(log2(m));
	newPos = decodeUnary(&q, stream, pos);
	q = q - 1;
	r = GetVarField(stream, newPos, newPos + b - 1);
	*symb = q * m + r;
	newPos += b;
	return newPos;
}

void computeEntropy(string array, cds_word size){
	cds_word alphabet[256];
	double entropy =0.0;
	for(int w = 0; w < 256; w++)
		alphabet[w]=0;
	for(cds_word i = 0; i < size; i++)
		alphabet[(int)(array[i])] += 1;
	for(int w = 0; w < 256; w++){
		if(alphabet[w] > 0)
			entropy += (alphabet[w]*1.0/size)*log2(size*1.0/alphabet[w]);
	}
	cout << "Length: " << size << "  Entropy: " <<  entropy <<  "  Theoretical Space use in MB: "  <<  (entropy * (size) / 8388608) << endl;
}




