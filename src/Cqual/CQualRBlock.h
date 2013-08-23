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

#ifndef _CQUALRBLOCK_H
#define _CQUALRBLOCK_H

#include <cmath>
#include <algorithm>
#include "CQual.h"


class CQualRBlock : public CQual{

	public:
		/*Compress the quality vector, given the fidelity param lossyparam 
		 * QUAL: vector of quality lines
		 * rpp: permutation container
		 * lossyparam: percentange of different accepted (ex: 10%)
		 * m: 0- Quality values stored as plain ASCII bytes
		 * 		1- Quality values stored as binary value using range between min and 
		 * 		   max values over the whole file.
		 * 		2- Quality values stored as binary value using range between local min 
		 *			 and max values over each quality line*/
		CQualRBlock(vector<string> QUAL, vector<RPP> rpp, int lossyparam, int m, cds_word sample);
		virtual ~CQualRBlock();
		virtual cds_word getSize() const;
		
		/*get quality lines x to y*/
		virtual vector<string> getInterval(cds_word x, cds_word y) const;

		/** Stores the Compressed Quality given a file pointer. */
		virtual void Save(ofstream &fp) const;

		virtual string * getAllQual() const;

		/** Reads a CQual file determining the type. */
		static CQualRBlock *Load(ifstream &fp);


	protected:
		void getArrays(vector<string> qual, vector<RPP> rpp, string& Occarray, string& QualityArray, int lossyparam);
		void storeQualityASCII(string QualityArray);
		void storeQualityGlobalmm(string QualityArray);
		void storeQualityLocalmm(string QualityArray, string OccArray);
		bool isPossible(cds_word min, cds_word max, double r);
		cds_word computeMidPoint(cds_word min, cds_word max, double r);

		int mode;
		cds_word sampleRate;
		cds_word *SamRL, *SamBS;
		cds_word sizeQualLine;
		cds_word lengthRunLength;
		cds_word *RunLength;
		cds_word lengthByteSeq;
		cds_word *ByteSeq;
		cds_word globalMax, globalMin;
		string LocalMin;
		string LocalMax;		
		cds_word numberOfValues; //total numer of quality values stored
		cds_word numberOfLines; //total number of quality lines stored
	
	private:
		CQualRBlock();

};


#endif
