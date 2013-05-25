#ifndef IConfig_h__
#define IConfig_h__

namespace clnt
{

//! Client configuration abstraction
class IConfig
{
public:

    struct Settings
    {
        Settings() : m_FoldButton(), m_CheckCallButton(), m_RaiseButton() {}
        unsigned m_FoldButton;
        unsigned m_CheckCallButton;
        unsigned m_RaiseButton;
    };

    virtual ~IConfig() {}

    virtual void Read() = 0;
    virtual void Write() const = 0;

    virtual const Settings& Get() const = 0;
    virtual void Set(const Settings& data) = 0;
};

}

#endif // IConfig_h__
