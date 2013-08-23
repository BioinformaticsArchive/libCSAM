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

#include "SAM.h"

void pushTableStFr(TableStFr *stfr, string s){
	vector<string>::iterator it = find((stfr->stringV).begin(), (stfr->stringV).end(), s);
	int pos;
	if(it != (stfr->stringV).end()){
		pos = it - (stfr->stringV).begin();
		(stfr->frecV)[pos] ++;
	}
	else{
		(stfr->stringV).push_back(s);
		(stfr->frecV).push_back(1);
	}
}

void pushRPP(vector<RPP>& rpp, string RN, int PO, cds_word Pe){
	RPP aux = {RN, PO, Pe};
	rpp.push_back(aux);
}

void pushPSR(PSR *psr, string RN, int PO, cds_word Pe, string SE){
	pushRPP(psr->rpp, RN, PO, Pe);
	(psr->SEQ).push_back(SE);
}

void pushREFI(REFI *refi, int FL, int MA){
	(refi->FLAG).push_back(FL);
	(refi->MAPQ).push_back(MA);
}

void clearPSR(PSR *psr){
	FreeAll(psr->SEQ);
	FreeAll(psr->rpp);
}


void clearREFI(REFI *refi){
	(refi->FLAG).clear();
	(refi->MAPQ).clear();
}

void Tokenize(const string& str, vector<string>& tokens, const string& delimiters){
	string::size_type last_pos = 0;
	string::size_type pos = str.find_first_of(delimiters);
	while(pos!=string::npos) {
		tokens.push_back(str.substr(last_pos,pos-last_pos));
		last_pos = pos+1;
		if(last_pos >= str.length())
			break;
		pos = str.find_first_of(delimiters,pos+1);
	}
	if(last_pos<str.length())
		tokens.push_back(str.substr(last_pos));
}

bool compareRPP(RPP first, RPP second){
	cds_word i=0;
	string f = first.RNAME;
	string s = second.RNAME;
	/*should force the RNAME == "*" be at the beggining*/
	while( (i < f.length()) && (i < s.length()) ){
		if (f[i] < s[i])
			return true;
		else if(f[i] > s[i])
			return false;
		++i;
	}
	if(f.length() < s.length())
		return true;
	else if(f.length() > s.length())
		return false;
	else{
		if(first.POS < second.POS)
			return true;
		else
			return false;
	}
}

