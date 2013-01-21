#include "Cards.h"

#include <exception>
#include <iostream>

#include <boost/regex.hpp>

using namespace clnt;

void Process(const std::string& input, std::ostream& output)
{
	output << "m_Players = boost::assign::list_of" << std::endl;

	static const boost::regex line("\\]\\:(.*?)/\\*");

	boost::sregex_iterator itLine(input.begin(), input.end(), line);
	boost::sregex_iterator end;

	bool ended = false;
	while (itLine != end)
	{
		const std::string& lineText = (*itLine)[1];

		// info
		{
			static const boost::regex info(".*'(\\S+)'.*stack.*'(\\d+)'");
			boost::sregex_iterator it(lineText.begin(), lineText.end(), info);

			while (it != end)
			{
				output << "\t(Player(\"" << (*it)[1] << "\", " << (*it)[2] << "))" << std::endl;
				++it;
			}
		}

		// actions
		{
			static const boost::regex action(".*'(\\S+)'.*action.*'(\\S+)'.*'(\\d+)'");
			boost::sregex_iterator it(lineText.begin(), lineText.end(), action);		

			while (it != end)
			{
				if (!ended)
				{
					output << "\t;" << std::endl;
					output << "m_Table->PlayersInfo(m_Players); " << std::endl << std::endl;
					ended = true;
				}

				output << "m_Table->PlayerAction(\"" << (*it)[1] << "\", Action::" << (*it)[2] << ", " << (*it)[3] << ");" << std::endl;
				++it;
			}
		}		
		
		// flop cards
		{
			static const boost::regex cards("\\('(\\S+?)'of'(\\S+?)'\\)");
			boost::sregex_iterator it(lineText.begin(), lineText.end(), cards);		

			if (it != end)
			{
				output << "{const Card::List flopCards = boost::assign::list_of" << std::endl;

				while (it != end)
				{
					output << "\t(Card(Card::" << (*it)[1] << ", Suit::" << (*it)[2] << "))" << std::endl;
					++it;
				}
				output << "\t;" << std::endl;
				output << "m_Table->FlopCards(flopCards);}" << std::endl << std::endl;
			}
		}

		// player cards
// 		{
// 			static const boost::regex action(".*'(\\S+)'.*action:'ShowCards'.*'(\\S+)'");
// 			boost::sregex_iterator it(lineText.begin(), lineText.end(), action);		
// 
// 			while (it != end)
// 			{
// 				const std::string& name = (*it)[1];
// 				const std::string& cards = (*it)[2];
// 
// 				Card::List cardsList;
// 
// 				Card tmp(Card::FromString(cards[0]), static_cast<Suit::Value>(cards[1]));
// 				cardsList.push_back(tmp);
// 
// 				tmp.m_Value = Card::FromString(cards[2]);
// 				tmp.m_Suit = static_cast<Suit::Value>(cards[3]);
// 				cardsList.push_back(tmp);
// 
// 
// 				output << "m_Table->PlayerAction(\"" << (*it)[1] << "\", Action::" << (*it)[2] << ", " << (*it)[3] << ");" << std::endl;
// 				++it;
// 			}
// 		}	

		++itLine;
	}
}

int main(int argc, char* argv[])
{
	try 
	{
		std::string text;
		std::string temp;
		while (temp != "END")
		{
			std::cin >> temp;
			text += temp;
		}

		system("cls");
		Process(text, std::cout);
		std::cin >> temp;
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}

