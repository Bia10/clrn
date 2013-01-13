#ifndef Injector_h__
#define Injector_h__

#include <string>

namespace dll
{

class Injector
{
public:
	Injector(std::size_t id);
	Injector(const std::string& name);
	~Injector();
	void Inject(const std::string& path);
private:
	std::size_t m_ProcessId;
};

} // namespace dll

#endif // Injector_h__
