#ifndef PacketParser_h__
#define PacketParser_h__

#include <string>

namespace clnt
{
namespace ps
{

class PacketParser
{
public:
	PacketParser(const std::string& regexp, const std::string& packet);
	~PacketParser();

	bool Eof() const;
	PacketParser& operator ++ ();

	unsigned GetInt(unsigned index);
	std::string GetString(unsigned index);
private:
	class Impl;
	Impl* m_Impl;
};

}
}

#endif // PacketParser_h__
