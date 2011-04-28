// IMD5.h: interface for the IMD5 class.
//
//////////////////////////////////////////////////////////////////////
/* Copyright © 1991-2, RSA Data Security, Inc. Created 1991. All

rights reserved.

License to copy and use this software is granted provided that it
is identified as the "RSA Data Security, Inc. MD5 Message-Digest
Algorithm" in all material mentioning or referencing this software
or this function.

License is also granted to make and use derivative works provided
that such works are identified as "derived from the RSA Data
Security, Inc. MD5 Message-Digest Algorithm" in all material
mentioning or referencing the derived work.

RSA Data Security, Inc. makes no representations concerning either
the merchantability of this software or the suitability of this
software for any particular purpose. It is provided "as is"
without express or implied warranty of any kind.


These notices must be retained in any copies of any part of this
documentation and/or software.
 */


#ifndef __IMD5_H__
#define __IMD5_H__


// Constants for MD5Transform routine.

#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21


#ifndef PROTOTYPES
#define PROTOTYPES 0
#endif

// POINTER defines a generic pointer type
typedef unsigned char *POINTER;

// UINT2 defines a two byte word
typedef unsigned short int UINT2;

// UINT4 defines a four byte word 
typedef unsigned long int UINT4;

/* PROTO_LIST is defined depending on how PROTOTYPES is defined above.

If using PROTOTYPES, then PROTO_LIST returns the list, otherwise it
returns an empty list. 
*/

#if PROTOTYPES
#define PROTO_LIST(list) list
#else
#define PROTO_LIST(list) ()
#endif


// F, G, H and I are basic MD5 functions.
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

// ROTATE_LEFT rotates x left n bits.

#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.

Rotation is separate from addition to prevent recomputation.
 */

#define FF(a, b, c, d, x, s, ac) { (a) += F ((b), (c), (d)) + (x) + (UINT4)(ac); (a) = ROTATE_LEFT ((a), (s)); (a) += (b); } 
#define GG(a, b, c, d, x, s, ac) { (a) += G ((b), (c), (d)) + (x) + (UINT4)(ac); (a) = ROTATE_LEFT ((a), (s)); (a) += (b); }
#define HH(a, b, c, d, x, s, ac) { (a) += H ((b), (c), (d)) + (x) + (UINT4)(ac); (a) = ROTATE_LEFT ((a), (s)); (a) += (b); }
#define II(a, b, c, d, x, s, ac) { (a) += I ((b), (c), (d)) + (x) + (UINT4)(ac); (a) = ROTATE_LEFT ((a), (s)); (a) += (b); }


// MD5 context.
typedef struct 
{
	UINT4 state[4];                                   /* state (ABCD) */
	UINT4 count[2];        /* number of bits, modulo 2^64 (lsb first) */
	unsigned char buffer[64];                         /* input buffer */
} MD5_CTX; 

static unsigned char PADDING[64] = { 0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; 

class IMD5  
{
public:
	IMD5();
	virtual ~IMD5();

	void MD5Init(MD5_CTX *context);
	void MD5Update(MD5_CTX *context, unsigned char *input, unsigned int inputLen); 
	void MD5Final(unsigned char digest[16], MD5_CTX *context);

	void Encode(unsigned char *output, UINT4 *input, unsigned int len); 
	void Decode(UINT4 *output, unsigned char *input, unsigned int len); 

	void MD5_memcpy(POINTER output, POINTER input, unsigned int len);
	void MD5_memset(POINTER output, int value, unsigned int len);
	void MD5Transform(UINT4 state[4], unsigned char block[64]);

};

#endif // ifndef __IMD5_H__

