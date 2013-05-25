#ifndef NetworkTeacher_h__
#define NetworkTeacher_h__

#include "INeuroNetwork.h"

#include <string>

#include <boost/function/function_fwd.hpp>

struct fann;

namespace neuro
{

class NetworkTeacher : public INetwork<float>
{
public:

    //! Train callback type
    typedef boost::function<void (unsigned epoch, unsigned epochCount, float error, float desiredError)> CallbackFn;

	NetworkTeacher(const std::string& file);
	~NetworkTeacher();

	virtual void Process(const std::vector<float>& input, std::vector<float>& output) override;
    void TrainOnFile(const std::string& file, const CallbackFn& callback);

private:
	fann* m_Net;
	std::string m_File;
};

} // namespace neuro

#endif // NetworkTeacher_h__
