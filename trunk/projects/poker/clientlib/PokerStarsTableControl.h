#ifndef PokerStarsTableControl_h__
#define PokerStarsTableControl_h__

#include "ILog.h"
#include "ITableControl.h"
#include "IConfig.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#include <windows.h>

#include <boost/shared_ptr.hpp>
#include <boost/system/error_code.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/function/function_fwd.hpp>

namespace clnt
{
namespace ps
{

class TableControl : public ITableControl
{
    typedef boost::shared_ptr<boost::asio::deadline_timer> TimerPtr;
public:
    TableControl(ILog& logger, boost::asio::io_service& io, IConfig& cfg, HWND window);

    virtual void Fold() override;
    virtual void CheckCall() override;
    virtual void BetRaise(unsigned amount) override;
private:
    virtual void TimerCallback(const boost::system::error_code& e, HWND window, unsigned key, TimerPtr, const boost::function<void()>& work);
    void SetBetAmount(unsigned amount);
private:
    ILog& m_Log;
    boost::asio::io_service& m_Io;
    IConfig::Settings m_Settings;
    const HWND m_Window;
};

} // namespace ps
} // namespace clnt

#endif // PokerStarsTableControl_h__
