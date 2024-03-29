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
#ifndef CODER_H
#define CODER_H


#include "./../basics/libcds.h"
#include "./../basics/io.h"
#include <cmath>
#include <algorithm>

using namespace std;
using namespace cds::basic;


/** Unary encode symb into stream at bit-position pos,
 *  returns the ending position (bits) */
cds_word encodeUnary(cds_word symb, cds_word *stream, cds_word pos);

/** Decode into symb from stream at bit-position
 * pos, returns the new position. Each symbol was Unary encoded*/
cds_word decodeUnary(cds_word *symb, cds_word *stream, cds_word pos);


/** Elias Gamma encode symb into stream at bit-position pos,
 *  returns the ending position (bits) */
cds_word encodeGamma(cds_word symb, cds_word *stream, cds_word pos);

/** Decode into symb from stream at bit-position
 * pos, returns the new position. Each symbol was Elias Gamma encoded*/
cds_word decodeGamma(cds_word *symb, cds_word *stream, cds_word pos);


/** Golomb encode symb into stream at bit-position pos,
 *  returns the ending position (bits). Golomb is parameterized by m */       
cds_word encodeGolomb(cds_word symb, cds_word *stream, cds_word pos, cds_word m);
								

/** Decode into symb from stream at bit-position
 * pos, returns the new position. Each symbol was Golomb encode*/     
cds_word decodeGolomb(cds_word *symb, cds_word *stream, cds_word pos, cds_word m);

void computeEntropy(string array, cds_word size);


#endif
