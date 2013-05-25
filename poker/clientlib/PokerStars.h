#ifndef PokerStars_h__
#define PokerStars_h__

#include <wtypes.h>
#include <string>
#include <assert.h>

namespace dasm
{

#pragma pack(push, 1)
	struct StringData
	{
		char *m_StringPtr;
		DWORD m_Size1;
		DWORD m_Size2;
	};
#pragma pack(pop)

#pragma pack(push, 1)
	struct DataBlock
	{
		char *m_StringBody;
		StringData m_StringData;
		DWORD m_Offset;
		int* m_SomePointer;
		char* m_Data;
		DWORD m_Size;
		DWORD m_UnknownDword;
	};
#pragma pack(pop)

	struct WindowMessage
	{
		DWORD Int1;
		DWORD Int2;
		DWORD Int3;
		DWORD* Int4;
		DWORD* Int5;
		DWORD* Int6;
		DWORD Int7;
		DWORD Int8;
		DataBlock m_Block;
	};

#pragma pack(push, 1)
	class MessageRecord
	{
	public:
		MessageRecord(const WindowMessage& message)
			: m_Block(&message.m_Block)
			, m_BytesParsed(0)
			, m_FieldType(0)
			, m_UnknownDword1C(0)
			, m_UnknownDword10(0)
			, m_UnknownPointer(0)
			, m_StringData(0)
			, m_UnknownDwordC(0)
			, m_UnknownBlock(0)
		{

		}

		MessageRecord()
			: m_Block(0)
			, m_BytesParsed(0)
			, m_FieldType(0)
			, m_UnknownDword1C(0)
			, m_UnknownDword10(0)
			, m_UnknownPointer(0)
			, m_StringData(0)
			, m_UnknownDwordC(0)
			, m_UnknownBlock(0)
		{

		}

		template<typename T>
		void Extract(T& value)
		{
			assert(!m_FieldType);

			const BYTE* values = reinterpret_cast<const BYTE*>(m_Block->m_Data);
			values += m_Block->m_Offset;
			values += m_BytesParsed;

			value = *reinterpret_cast<const T*>(values);
			m_BytesParsed += sizeof(T);
		}

		template<>
		void Extract<std::string>(std::string& value)
		{
			assert(!m_FieldType);

			const char* values = reinterpret_cast<const char*>(m_Block->m_Data);
			values += m_Block->m_Offset;
			values += m_BytesParsed;

			value.assign(values);
			m_BytesParsed += (value.size() + 1);
		}

		void SetParsedBytes(DWORD parsed)
		{
			m_BytesParsed = parsed;
		}

	private:
		const DataBlock *m_Block;
		DWORD m_BytesParsed;
		DWORD m_FieldType;
		DWORD m_UnknownDwordC;
		DWORD m_UnknownDword10;
		DWORD *m_UnknownPointer;
		StringData *m_StringData;
		DWORD m_UnknownDword1C;
		DataBlock *m_UnknownBlock;

	};

#pragma pack(pop)
}


#endif // PokerStars_h__
