/*
 * Crypto.h
 *
 *  Created on: Dec 15, 2011
 *      Author: root
 */

#ifndef CRYPTO_H_
#define CRYPTO_H_

#include "Types.h"

#include <memory>
#include <vector>

//! Forward declarations
struct evp_cipher_ctx_st;
typedef evp_cipher_ctx_st EVP_CIPHER_CTX;

namespace cmn{

//! Class for data crypting
class CCrypto
{
public:
	CCrypto(const _tstring& sKey);
	virtual ~CCrypto();

	//! Data encryption
	void 								Encrypt(const _tstring& sInput, std::vector< unsigned char >& vecResult);

	//! Data decryption
	void 								Decrypt(const _tstring& sInput, std::vector< unsigned char >& vecResult);

	//! Get md5 hash
	static const _tstring 				CalcMD5(const unsigned char * pData, const std::size_t& nLength);


	static const _tstring 				GenerateRndString();

private:

	//! Crypto initialization with spicified key
	void 								Init(const _tstring& sKey);

	//! Encypting context
	std::auto_ptr< EVP_CIPHER_CTX > 	m_pctxEncypt;

	//! Decrypting context
	std::auto_ptr< EVP_CIPHER_CTX > 	m_pctxDecypt;

};

} //namespace cmn

#endif /* CRYPTO_H_ */
