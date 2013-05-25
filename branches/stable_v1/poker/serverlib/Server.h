#ifndef Server_h__
#define Server_h__

#include <memory>

namespace srv
{

class Server
{
public:
	Server();
	~Server();
	void Run();

private:
	class Impl;
	Impl* m_Impl;

};

} // namespace srv


#endif // Server_h__
