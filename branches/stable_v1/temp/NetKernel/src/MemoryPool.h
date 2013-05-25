/*
 * MemoryPool.h
 *
 *  Created on: Feb 4, 2012
 *      Author: clrn
 */

#ifndef MEMORYPOOL_H_
#define MEMORYPOOL_H_

#include <memory>
#include <list>

namespace net
{

#pragma pack (push, 1)
	//! Transaction Id
	struct CTransactionId
	{
		unsigned char	GUID[16];
		std::size_t		nId;

		const bool operator < (const CTransactionId& that) const;
		void Generate();
	};
#pragma pack (pop)


//! Class for memory management
class CMemoryPool
{
public:

	//! Buffer type
	struct Buffer
	{
		void*		pData;
		std::size_t nSize;
	};

	CMemoryPool();
	~CMemoryPool();

	//! Allocate buffer
	const Buffer Allocate(const std::size_t nSize);

	//! Free transaction buffer
	void Free(const CTransactionId& Id);

	//! Read handler
	void ReadHandler(const std::size_t nReaded, std::list< CTransactionId >& Completed);

private:

	//! Implementation
	class CImpl;
	const std::auto_ptr< CImpl > m_pImpl;

};

} /* namespace net */
#endif /* MEMORYPOOL_H_ */
