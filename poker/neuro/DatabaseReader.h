#ifndef DatabaseReader_h__
#define DatabaseReader_h__

#include "INeuroNetwork.h"
#include "ILog.h"

#include <string>

namespace sql
{
    class IDatabase;
}

namespace neuro
{

class DatabaseReader : public INetwork<float>
{
public:

	DatabaseReader(ILog& logger, const std::string& file);
	~DatabaseReader();

	virtual void Process(const std::vector<float>& input, std::vector<float>& output) override;

private:

    sql::IDatabase* m_Db;
};

} // namespace neuro
#endif // DatabaseReader_h__
