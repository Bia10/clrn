#ifndef Server_h__
#define Server_h__

#include <memory>

namespace srv
{

class Server
{
public:
	Server();
	void Run();

private:
	class Impl;
	std::auto_ptr<Impl> m_Impl;

};

} // namespace srv


#endif // Server_h__
