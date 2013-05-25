#ifndef INeuroNetwork_h__
#define INeuroNetwork_h__

#include <vector>

#include <boost/shared_ptr.hpp>

namespace neuro
{

template<typename T>
class INetwork
{
public:
	typedef boost::shared_ptr<INetwork<T>> Ptr;
	virtual ~INetwork() {}

	virtual void Process(const std::vector<T>& input, std::vector<T>& output) = 0;
};

} // namespace neuro

#endif // INeuroNetwork_h__
