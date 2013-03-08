#ifndef Messages_h__
#define Messages_h__

#include "IMessage.h"
#include "ILog.h"


namespace clnt
{
namespace ps
{
namespace msg
{

class PlayerAction : public IMessage
{
public:
	PlayerAction(ILog& logger) : m_Log(logger) {}
private:
	virtual std::size_t GetId() const override;
	virtual void Process(const dasm::WindowMessage& message, ITable& table) const override;
private:
	ILog& m_Log;
};

class FlopCards : public IMessage
{
public:
	FlopCards(ILog& logger) : m_Log(logger) {}
private:
	virtual std::size_t GetId() const override;
	virtual void Process(const dasm::WindowMessage& message, ITable& table) const override;
private:
	ILog& m_Log;
};

class TurnAndRiverCards : public FlopCards
{
public:
	TurnAndRiverCards(ILog& logger) : FlopCards(logger) {}
private:
	virtual std::size_t GetId() const override;
};

class PlayerCards : public IMessage
{
public:
	PlayerCards(ILog& logger) : m_Log(logger) {}
private:
	virtual std::size_t GetId() const override;
	virtual void Process(const dasm::WindowMessage& message, ITable& table) const override;
private:
	ILog& m_Log;
};

class PlayersInfo : public IMessage
{
public:
	PlayersInfo(ILog& logger) : m_Log(logger) {}
private:
	virtual std::size_t GetId() const override;
	virtual void Process(const dasm::WindowMessage& message, ITable& table) const override;
private:
	ILog& m_Log;
};

class PlayerInfo : public IMessage
{
public:
	PlayerInfo(ILog& logger) : m_Log(logger) {}
private:
	virtual std::size_t GetId() const override;
	virtual void Process(const dasm::WindowMessage& message, ITable& table) const override;
private:
	ILog& m_Log;
};

class TableInfo : public IMessage
{
public:
	TableInfo(ILog& logger) : m_Log(logger) {}
private:
	virtual std::size_t GetId() const override;
	virtual void Process(const dasm::WindowMessage& message, ITable& table) const override;
	void ParseActions(const dasm::WindowMessage& message, ITable& table) const;
private:
	ILog& m_Log;
};

} // namespace msg
} // namespace ps
} // namespace clnt

#endif // Messages_h__
