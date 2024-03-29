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

#ifndef _CQUAL_H
#define _CQUAL_H

#include <cmath>
#include <algorithm>

#include "./../coders/Coder.h"
#include "./../coders/Huffman.h"
#include "./../SAM.h"

const cds_word LOSSLESS = 1;
const cds_word PBLOCK = 2;
const cds_word RBLOCK = 3;
const cds_word BINS = 4;
const cds_word NSTORE = 5;

class CQual{

	public:
		virtual ~CQual();
		virtual cds_word getSize() const = 0;
		
		/*A sample must be stored*/
		virtual vector<string> getInterval(cds_word x, cds_word y) const = 0;

		virtual string * getAllQual() const = 0;

		/** Stores the Compressed Quality given a file pointer. */
		virtual void Save(ofstream &fp) const = 0;

		/** Reads a CQual file determining the type. */
		static CQual *Load(ifstream &fp);

};

#include "CQualLL.h"
#include "CQualPBlock.h"
#include "CQualRBlock.h"
#include "CQualBin.h"
#include "CQualNStore.h"

#endif
