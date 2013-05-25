/*
 * MemoryPool.cpp
 *
 *  Created on: Feb 4, 2012
 *      Author: clrn
 */

#include "MemoryPool.h"

#include <cassert>
#include <iostream>
#include <string.h>
#include <vector>
#include <map>
#include <stdexcept>
#include <algorithm>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/thread/mutex.hpp>

namespace net
{

	/*

	// transaction packet
	std::size_t nTransactionId;
	std::size_t nDataSize;
	std::size_t nTotalChunks;
	// data packet below

	// Data packet
	std::size_t nTransactionId;
	std::size_t nChunkIndex;
	std::size_t nChunkSize;
	// Data below

	*/
const bool CTransactionId::operator < (const CTransactionId& that) const
{
	const int nResult = memcmp(GUID, that.GUID, sizeof(GUID));
	if (0 == nResult)
		return nId < that.nId;
	return nResult < 0;
}

void CTransactionId::Generate()
{
	static bool HasMac = false;
	static boost::uuids::uuid UID = {0};
	static std::size_t nCounter = 0;
	if (!HasMac)
	{
		UID = boost::uuids::random_generator()();
		HasMac = true;
	}
	std::copy(UID.begin(), UID.end(), GUID);
	nId = nCounter++;
}

//! Pool implementation
class CMemoryPool::CImpl
{
	enum 
	{
		BLOCK_SIZE		= 4096,
		TOTAL_BLOCKS	= 1000
	};

	//! Type of the memory pool
	typedef std::vector< char > TMemory;

	//! Info about chunk location
	struct CChunkInfo
	{
		std::size_t		nLocation;			//!< Chunk location in pool
		std::size_t		nSize;				//!< Data size in bytes
		bool			bSwapped;			//!< Data chunk is swapped on hard disk
		CChunkInfo()
			: nLocation(0)
			, nSize(0)
			, bSwapped(false)
		{

		}

		const bool operator < (const CChunkInfo& that) const
		{
			return nLocation < that.nLocation;
		}
	};

	//! Info about transaction
	struct CTransactionInfo
	{
		std::size_t					nDataSize;
		std::size_t					nTotalChunks;  
		std::vector< CChunkInfo >	vecChunks;		//!< Chunks locations
		std::size_t					nHeaderLocation; //!< Location of the transaction header

		CTransactionInfo(const std::size_t nSize, const std::size_t nChunks, const std::size_t nHeaderPosition) 
			: nDataSize(nSize)
			, nTotalChunks(nChunks)
			, nHeaderLocation(nHeaderPosition)
		{
			vecChunks.reserve(nTotalChunks);
		}
	};

	//! Transactions map type
	typedef std::map< CTransactionId, CTransactionInfo >	TTransactions;

	//! Allocated data
	typedef std::vector< bool >								TAllocatedData;

public:
	CImpl()
		: m_nNextLocation(0)
	{
		m_vecMemory.resize(BLOCK_SIZE * TOTAL_BLOCKS);
		m_vecAllocatedData.resize(TOTAL_BLOCKS);
	}

	const Buffer Allocate(const std::size_t nSize)
	{
		Buffer Result;
		boost::mutex::scoped_lock Lock(m_mxRead);

		// Checking free space
		std::size_t nFreeSpace = m_vecMemory.size() - m_nNextLocation;
		if (nFreeSpace >= nSize)
		{
			Result.pData = &m_vecMemory[m_nNextLocation];
			Result.nSize = nFreeSpace;
			return Result;
		}

		// Need to reallocate
		// Swap all data from beginning
		// remember what we swapped and where(correcting m_mapTransactions)
		Result.pData = &m_vecMemory.front();
		Result.nSize = m_vecMemory.size();
		return Result;
	}

	void Free(const CTransactionId& Id)
	{
		boost::mutex::scoped_lock Lock(m_mxTransactions);

		TTransactions::iterator itTrans = m_mapTransactions.find(Id);
		if (m_mapTransactions.end() == itTrans)
			throw std::logic_error("Transaction Id not found.");

		CTransactionInfo& Transaction = itTrans->second;

		// Sorting chunks by location in buffer
		//std::sort(Transaction.vecChunks.begin(), Transaction.vecChunks.end());

		// Space for headers
		static const std::size_t nHeadersSize = sizeof(CTransactionId) * 2 + sizeof(std::size_t) * 4;

		std::size_t nIndex = 0;
		std::vector< CChunkInfo >::reverse_iterator it = Transaction.vecChunks.rbegin();
		const std::vector< CChunkInfo >::const_reverse_iterator itBegin = it;
		const std::vector< CChunkInfo >::const_reverse_iterator itEnd = Transaction.vecChunks.rend();
		for (; it != itEnd; ++it)
		{
			const CChunkInfo& Info = *it;
			const std::size_t nBlock = Info.nLocation / BLOCK_SIZE;

			if (itBegin == it)
			{
				// First loop
				nIndex = nBlock;
			}
			else if (nBlock < nIndex || Info.nLocation == nHeadersSize)
			{
				// Buffer with previuos index
				// That mean that buffers after it completely freed
				for (std::size_t n = nIndex; n > nBlock; --n)
					m_vecAllocatedData[n] = false;

				// Block completely freed
				if (Info.nLocation == nHeadersSize)
					m_vecAllocatedData[nBlock] = false;

				nIndex = nBlock;
			}
		}

		// Detecting last free index, starting from m_nNextLocation
		std::size_t nCurrent = m_nNextLocation / BLOCK_SIZE;
		for (;;)
		{
			if (m_vecAllocatedData[nCurrent])
			{
				++nCurrent;
				break;
			}

			if (!nCurrent)
				break;	

			--nCurrent;
		}

		m_nNextLocation = nCurrent * BLOCK_SIZE;

		const char* szTest = &m_vecMemory[m_nNextLocation];
		
		m_mapTransactions.erase(itTrans);
	}

	void ReadHandler(const std::size_t nReaded, std::list< CTransactionId >& Completed)
	{
		boost::mutex::scoped_lock Lock(m_mxTransactions);

		// m_NextLocation contains last allocated buffer
		// we need to analyze data from m_NextLocation to m_NextLocation + nReaded

		// Vector contains plain data, so simply getting pointers to data begin and end
		const char* pBegin = &m_vecMemory[m_nNextLocation];
		const char* pEnd = pBegin + nReaded;
		
		// Setting up m_NextLocation to next byte after readed data
		m_nNextLocation += nReaded;

		// Process recieved data
		for (const char* pCurrent = pBegin; pCurrent < pEnd; )
		{
			const CTransactionId& Id = *reinterpret_cast< const CTransactionId* >(pCurrent);
			pCurrent += sizeof(CTransactionId);

			TTransactions::iterator it = m_mapTransactions.find(Id);
			if (m_mapTransactions.end() == it)
			{
				// New transaction, getting transaction info
				const std::size_t nDataSize = *reinterpret_cast< const std::size_t* >(pCurrent);

				pCurrent += sizeof(std::size_t);
				const std::size_t nTotalChunks = *reinterpret_cast< const std::size_t* >(pCurrent);
				pCurrent += sizeof(std::size_t);
				
				CTransactionInfo Info(nDataSize, nTotalChunks, reinterpret_cast< const char * >(&Id) - &m_vecMemory.front());
				m_mapTransactions.insert(std::make_pair(Id, Info));

				m_vecAllocatedData[(reinterpret_cast< const char * >(&Id) - &m_vecMemory.front()) / BLOCK_SIZE] = true;

				continue;
			}

			// Reading block info
			const std::size_t nChunkIndex = *reinterpret_cast< const std::size_t* >(pCurrent);

			pCurrent += sizeof(std::size_t);
			const std::size_t nChunkSize = *reinterpret_cast< const std::size_t* >(pCurrent);
			pCurrent += sizeof(std::size_t);		

			CChunkInfo ChunkInfo;
			ChunkInfo.nLocation = pCurrent - &m_vecMemory.front();
			ChunkInfo.nSize = nChunkSize;

			it->second.vecChunks.push_back(ChunkInfo);
			if (it->second.vecChunks.size() == it->second.nTotalChunks)
				Completed.push_back(Id);

			pCurrent += nChunkSize;	

			m_vecAllocatedData[(pCurrent - &m_vecMemory.front()) / BLOCK_SIZE] = true;

			std::size_t nIndex = (pCurrent - &m_vecMemory.front()) / BLOCK_SIZE;
			std::size_t nFirstFree = nIndex * BLOCK_SIZE;
			const char* szFirst = &m_vecMemory[nFirstFree];

			std::size_t nSecondFree = (nIndex + 1) * BLOCK_SIZE;
			const char* szSecond = &m_vecMemory[nSecondFree];
		}
	}

private:

	//! Memory blocks
	TMemory						m_vecMemory;

	//! Allocated data
	TAllocatedData				m_vecAllocatedData;

	//! Transactions
	TTransactions				m_mapTransactions;
	
	//! Next allocation pointer
	std::size_t					m_nNextLocation;

	//! Read lock
	boost::mutex				m_mxRead;

	//! Free lock
	boost::mutex				m_mxTransactions;

};

CMemoryPool::CMemoryPool() :
	m_pImpl(new CImpl())
{


}

CMemoryPool::~CMemoryPool()
{
}

const CMemoryPool::Buffer CMemoryPool::Allocate(const std::size_t nSize)
{
	return m_pImpl->Allocate(nSize);
}

void CMemoryPool::Free(const CTransactionId& Id)
{
	m_pImpl->Free(Id);
}

void CMemoryPool::ReadHandler(const std::size_t nReaded, std::list< CTransactionId >& Completed)
{
	m_pImpl->ReadHandler(nReaded, Completed);
}

} /* namespace net */
