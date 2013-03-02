#ifndef DecisionMaker_h__
#define DecisionMaker_h__

#include "Logic.h"
#include "ILog.h"

namespace pcmn
{

class Decisionmaker : public IDecisionCallback
{
public:
	Decisionmaker(ILog& logger);

private:

	virtual void MakeDecision() override;

private:

	ILog& m_Log;
};

}

#endif // DecisionMaker_h__