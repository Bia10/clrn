#ifndef NetworkTeacher_h__
#define NetworkTeacher_h__

#include "INeuroNetwork.h"

#include <string>

struct fann;

namespace neuro
{

class NetworkTeacher : public INetwork<float>
{
public:

	NetworkTeacher(const std::string& file);
	~NetworkTeacher();

	virtual void Process(const std::vector<float>& input, std::vector<float>& output) override;

private:
	fann* m_Net;
	std::string m_File;
};

} // namespace neuro

#endif // NetworkTeacher_h__
