/*
 *  sha1.h
 *
 *  Description:
 *      This is the header file for code which implements the Secure
 *      Hashing Algorithm 1 as defined in FIPS PUB 180-1 published
 *      April 17, 1995.
 *
 *      Many of the variable names in this code, especially the
 *      single character names, were used because those were the names
 *      used in the publication.
 *
 *      Please read the file sha1.c for more information.
 *
 */

#ifndef _SHA1_H_
#define _SHA1_H_

/*
 * If you do not have the ISO standard stdint.h header file, then you
 * must typdef the following:
 *    name              meaning
 *  uint32_t         unsigned 32 bit integer
 *  uint8_t          unsigned 8 bit integer (i.e., unsigned char)
 *  int_least16_t    integer of >= 16 bits
 *
 */
#include <stdint.h>


#ifndef _SHA_enum_
#define _SHA_enum_
enum
{
    shaSuccess = 0,
    shaNull,            /* Null pointer parameter */
    shaInputTooLong,    /* input data too long */
    shaStateError       /* called Input after Result */
};
#endif

#define SHA1HashSize 20

class CSHA1
{
protected:
	/*
	 *  This structure will hold context information for the SHA-1
	 *  hashing operation
	 */
	typedef struct SHA1Context
	{
		uint32_t Intermediate_Hash[SHA1HashSize/4]; /* Message Digest  */

		uint32_t Length_Low;            /* Message length in bits      */
		uint32_t Length_High;           /* Message length in bits      */

								   /* Index into message block array   */
		int_least16_t Message_Block_Index;
		uint8_t Message_Block[64];      /* 512-bit message blocks      */

		int Computed;               /* Is the digest computed?         */
		int Corrupted;             /* Is the message digest corrupted? */
	} SHA1Context;

	// context where digest is calculated while
	// parts of data is given
	SHA1Context m_SHA1Context;
	
	// final result of digest
	uint8_t *m_pDigestResult;
	

	void SHA1PadMessage(SHA1Context *context);
	void SHA1ProcessMessageBlock(SHA1Context *context);

	int SHA1Reset(SHA1Context *context);
	int SHA1Input(SHA1Context *context, const uint8_t *message_array, unsigned long ulLength);
	int SHA1Result(SHA1Context *context, uint8_t Message_Digest[SHA1HashSize]);

public:
	CSHA1(void)
	{
		SHA1Reset(&m_SHA1Context);
		m_pDigestResult = new uint8_t[SHA1HashSize];
	};
	virtual ~CSHA1(void)
	{};

	// prepare for SHA1 digesting,
	// caller should not need this when using new instance
	int Reset(void)
	{
		return SHA1Reset(&m_SHA1Context);
	}

	int AddInput(const uint8_t *input, unsigned long ulLength)
	{
		return SHA1Input(&m_SHA1Context, input, ulLength);
	}
	
	// get resulting digest as array,
	// caller should encode it from binary-data
	// to hex-string for display/storage etc.
	//
	uint8_t *GetResultDigest()
	{
		int iRes = SHA1Result(&m_SHA1Context, m_pDigestResult);
		if (iRes == shaSuccess)
		{
			return m_pDigestResult;
		}
		return nullptr;
	}

	// for reserving buffer of suitable size
	// for the digest
	int GetDigestLength() const
	{
		return SHA1HashSize;
	}
	
};

#endif
