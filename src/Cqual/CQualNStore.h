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

#ifndef _CQUALNS_H
#define _CQUALNS_H

#include <cmath>
#include <algorithm>
#include "CQual.h"


class CQualNStore : public CQual{

	public:
		/*Compress the quality vector, given the fidelity param lossyparam
		 * m: 0- Using a P value
		 * 		1- Using a R value  
		 **/
		CQualNStore(vector<string> QUAL, vector<RPP> rpp, int m);
		virtual ~CQualNStore();
		virtual cds_word getSize() const;
		
		/*get quality lines x to y*/
		virtual vector<string> getInterval(cds_word x, cds_word y) const;

		/** Stores the Compressed Quality given a file pointer. */
		virtual void Save(ofstream &fp) const;

		virtual string * getAllQual() const;

		/** Reads a CQual file determining the type. */
		static CQualNStore *Load(ifstream &fp);


	protected:
		cds_word sizeQualLine;
		cds_word QualValue;
		cds_word numberOfLines; //total number of quality lines stored
	
	private:
		CQualNStore();

		cds_word getPQualValue(vector<string> QUAL, vector<RPP> rpp);
		cds_word getRQualValue(vector<string> QUAL, vector<RPP> rpp);

};


#endif
