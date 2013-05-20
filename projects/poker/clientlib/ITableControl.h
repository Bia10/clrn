#ifndef ITableControl_h__
#define ITableControl_h__

namespace clnt
{

//! Table control mechanism
class ITableControl
{
public:

    virtual ~ITableControl() {}

    virtual void Fold() = 0;
    virtual void CheckCall() = 0;
    virtual void BetRaise() = 0;
};

}
#endif // ITableControl_h__