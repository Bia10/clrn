#ifndef NeuroNetwork_h__
#define NeuroNetwork_h__

#include "INeuroNetwork.h"

#include <string>

struct fann;

namespace neuro
{

class Network : public INetwork<float>
{
public:

	Network(const std::string& file);
	~Network();

	virtual void Process(const std::vector<float>& input, std::vector<float>& output) override;

private:
	fann* m_Net;
};

} // namespace neuro

#endif // NeuroNetwork_h__
