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

#ifndef _SAM_H
#define _SAM_H

#include <string>
#include <cstring>
#include "./basics/libcds.h"
#include "./basics/io.h"
#include <cmath>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>


using namespace cds;
using namespace cds::basic;

using namespace std;

using namespace boost::archive;
using namespace boost::iostreams;


inline string NumtoString(cds_word x){
	stringstream ss;
	ss << x;
	return ss.str();
}

inline string GZcompressString(string s){
	stringstream zippedStream;
	{
		filtering_ostream filteringStream;
		filteringStream.push(gzip_compressor());
		filteringStream.push(zippedStream);
		filteringStream << s;
	}
	return  zippedStream.str();
}

inline string GZdecompressString(string s){
	stringstream ss,	unzippedStream;               
	ss << s;
	filtering_istream filteringStream;
	filteringStream.push(gzip_decompressor());	
	filteringStream.push(ss);
	copy(filteringStream, unzippedStream);
	return unzippedStream.str();
}


template <typename T>
void FreeAll( T & t ){
	    T tmp;
			t.swap( tmp );
}

typedef struct{
	vector<string> stringV;
	vector<int> frecV;
}TableStFr;

typedef struct{
	string RNAME;
	int POS;
	cds_word Permutation;
}RPP;

typedef struct{
	vector<RPP> rpp;
	vector<string> SEQ;
}PSR;

typedef struct{   
	vector<int> FLAG;
	vector<int> MAPQ;
}REFI;


void pushTableStFr(TableStFr *stfr, string s);

void pushRPP(vector<RPP>& rpp, string RN, int PO, cds_word Pe);

void pushPSR(PSR *psr, string RN, int PO, cds_word Pe, string SE);
		
void pushREFI(REFI *refi, int FL, int MA);		
		
void clearPSR(PSR *psr);

void clearREFI(REFI *refi);

void Tokenize(const string& str, vector<string>& tokens, const string& delimiters = " ");

bool compareRPP(RPP first, RPP second);


#endif

