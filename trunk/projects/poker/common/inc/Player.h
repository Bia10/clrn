#ifndef Player_h__
#define Player_h__

#include <string>
#include <vector>

namespace clnt
{
class Player
{
public:

	typedef std::vector<Player> List;

	std::string Name() const { return m_Name; }
	void Name(const std::string& val) { m_Name = val; }
	std::string Country() const { return m_Country; }
	void Country(const std::string& val) { m_Country = val; }
	std::size_t Stack() const { return m_Stack; }
	void Stack(std::size_t val) { m_Stack = val; }

private:
	std::string m_Name;
	std::string m_Country;
	std::size_t m_Stack;
};
}

#endif // Player_h__
