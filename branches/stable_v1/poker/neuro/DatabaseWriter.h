#ifndef DatabaseWriter_h__
#define DatabaseWriter_h__

#include "INeuroNetwork.h"
#include "ILog.h"

#include <string>

namespace sql
{
    class IDatabase;
}

namespace neuro
{

class DatabaseWriter : public INetwork<float>
{
public:

	DatabaseWriter(ILog& logger, const std::string& file);
	~DatabaseWriter();

	virtual void Process(const std::vector<float>& input, std::vector<float>& output) override;

private:

    sql::IDatabase* m_Db;
};

} // namespace neuro
#endif // DatabaseWriter_h__
