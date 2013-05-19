#ifndef PokerStarsConfig_h__
#define PokerStarsConfig_h__

#include "IConfig.h"
#include "ILog.h"

namespace clnt
{
namespace ps
{

class Config : public IConfig
{
public:

    Config(ILog& logger);

    virtual void Read() override;
    virtual void Write() const override;

    virtual const Settings& Get() const override;
    virtual void Set(const Settings& data) override;

private:
    ILog& m_Log;
    Settings m_Data;
};
}
}
#endif // PokerStarsConfig_h__