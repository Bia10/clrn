/*
 * Crypto.cpp
 *
 *  Created on: Dec 15, 2011
 *      Author: root
 */

#include "Crypto.h"
#include "Exception.h"

#include <cassert>
#include <vector>
#include <stdlib.h>
#include <time.h>

#include <openssl/evp.h>
#include <openssl/md5.h>

namespace cmn{
CCrypto::CCrypto(const _tstring& sKey)
{
	Init(sKey);
}

CCrypto::~CCrypto()
{
}

void CCrypto::Encrypt(const _tstring & sInput, std::vector< unsigned char >& vecResult)
{
	TRY_EXCEPTIONS
	{
		int c_len = sInput.size() + 128, f_len = 0;

		vecResult.resize(c_len);

		EVP_EncryptInit_ex(m_pctxEncypt.get(), NULL, NULL, NULL, NULL);

		if (EVP_EncryptUpdate(m_pctxEncypt.get(), &vecResult.front(), &c_len, reinterpret_cast< const unsigned char * > (sInput.c_str()), sInput.size()) != 1)
			THROW_EXCEPTION("EVP_EncryptUpdate failed");

		if (EVP_EncryptFinal_ex(m_pctxEncypt.get(), &vecResult.front() + c_len, &f_len) != 1)
			THROW_EXCEPTION("EVP_EncryptFinal_ex failed");

		vecResult.resize(c_len + f_len);
	}
	CATCH_PASS_EXCEPTIONS
}



void CCrypto::Decrypt(const _tstring & sInput, std::vector< unsigned char >& vecResult)
{
	TRY_EXCEPTIONS
	{
		int nInputLength = sInput.size();
		int nOutputLength = 0;

		vecResult.resize(nInputLength);

		EVP_DecryptInit_ex(m_pctxDecypt.get(), NULL, NULL, NULL, NULL);

		if (EVP_DecryptUpdate(m_pctxDecypt.get(), &vecResult.front(), &nInputLength, reinterpret_cast< const unsigned char * > (sInput.c_str()), sInput.size()) != 1)
			THROW_EXCEPTION("EVP_DecryptUpdate failed");

		if (EVP_DecryptFinal_ex(m_pctxDecypt.get(), &vecResult.front() + nInputLength, &nOutputLength) != 1)
			THROW_EXCEPTION("EVP_DecryptFinal_ex failed");

		vecResult.resize(nInputLength + nOutputLength);
	}
	CATCH_PASS_EXCEPTIONS
}



const _tstring CCrypto::CalcMD5(const unsigned char *pData, const std::size_t & nLength)
{
	_tostringstream ossResult;

	unsigned char pMD5[MD5_DIGEST_LENGTH];
	MD5(pData, nLength, pMD5);

	for (unsigned int i = 0; i < MD5_DIGEST_LENGTH; ++i)
	{
		ossResult << std::hex << static_cast< int > (pMD5[i]);
	}

	return ossResult.str();
}

void CCrypto::Init(const _tstring & sKey)
{
	TRY_EXCEPTIONS
	{
		assert(!sKey.empty());

		m_pctxEncypt.reset(new EVP_CIPHER_CTX());
		m_pctxDecypt.reset(new EVP_CIPHER_CTX());

		unsigned char pKey[32] = {0};
		unsigned char pIv[32] = {0};

		const int nRounds = 5;

		if (EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha1(), 0, reinterpret_cast< const unsigned char * > (sKey.c_str()), sKey.size(), nRounds, pKey, pIv) != 32)
			THROW_EXCEPTION("Key should be 256 bits")

		EVP_CIPHER_CTX_init(m_pctxEncypt.get());
		EVP_EncryptInit_ex(m_pctxEncypt.get(), EVP_aes_256_cbc(), NULL, pKey, pIv);
		EVP_CIPHER_CTX_init(m_pctxDecypt.get());
		EVP_DecryptInit_ex(m_pctxDecypt.get(), EVP_aes_256_cbc(), NULL, pKey, pIv);

	}
	CATCH_PASS_EXCEPTIONS
}

const _tstring CCrypto::GenerateRndString()
{
	static _tstring s_sChars(
		  "abcdefghijklmnopqrstuvwxyz"
		  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		  "1234567890");

	static bool s_bIsInited = false;
	if (!s_bIsInited)
	{
		srand(time(NULL));
		s_bIsInited = true;
	}

	std::vector< unsigned char > vecRnd(50);

	for(unsigned int i = 0; i < vecRnd.size(); ++i)
	{
		vecRnd[i] = s_sChars[rand() % s_sChars.size() + 1];
	}

	return cmn::CCrypto::CalcMD5(&vecRnd.front(), vecRnd.size());

}

}//namespace cmn




