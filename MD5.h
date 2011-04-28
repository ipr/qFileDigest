// MD5.h: interface for the MD5 class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __MD5_H__
#define __MD5_H__


/* Copyright © 1990-2, RSA Data Security, Inc. Created 1990. All
rights reserved.

RSA Data Security, Inc. makes no representations concerning either
the merchantability of this software or the suitability of this
software for any particular purpose. It is provided "as is"
without express or implied warranty of any kind.

These notices must be retained in any copies of any part of this
documentation and/or software.
 */

#include "IMD5.h"

#include <memory>


class CMD5 : protected IMD5  
{
protected:
	MD5_CTX m_context;

	const int m_iDigestLen; // = 16 // depends on algorithm used
	unsigned char *m_pDigestResult;
	
public:
	CMD5(void)
		: IMD5()
		, m_iDigestLen(16)
		, m_pDigestResult(nullptr)
	{
		MD5Init(&m_context);
		m_pDigestResult = new unsigned char[m_iDigestLen];
	};
	virtual ~CMD5(void)
	{};

	// prepare for MD5 digesting,
	// caller should not need this when using new instance
	void Reset(void)
	{
		MD5Init(&m_context);
		::memset(m_pDigestResult, 0, m_iDigestLen);
	}

	// note: code may expect size in multiples of 4
	void AddInput(unsigned char *input, unsigned int inputLen)
	{
		if (inputLen != 0)
		{
			MD5Update(&m_context, input, inputLen);
		}
	}

	// to access final result after
	// digesting all data
	unsigned char *GetResultDigest()
	{
		MD5Final(m_pDigestResult, &m_context);
		return m_pDigestResult;
	}
	
	// for reserving buffer of suitable size
	// for the digest
	int GetDigestLength() const
	{
		return m_iDigestLen;
	}

};

#endif // ifndef __MD5_H__
