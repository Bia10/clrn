#ifndef INeuroNetwork_h__
#define INeuroNetwork_h__

#include <vector>

#include <boost/shared_ptr.hpp>

namespace neuro
{

const std::size_t LAYERS_COUNT = 3;
const std::size_t HIDDEN_NEIRONS_COUNT = 3;
const std::size_t INPUT_COUNT = 9;
const std::size_t OUTPUT_COUNT = 3;

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
