#include "AddUserState.h"


// stl
#include <iostream>
#include <string>


//  app
#include "StateMachine.h"
#include "FrontEnd.h"
#include "Message.h"

namespace MessageService {

namespace {
	const uint32_t MAX_USER_NAME = 30;
}


bool AddUserState::update()
{
	std::cout << "max: " + std::to_string(MAX_USER_NAME) + " characters";
	std::cout << "(alpha numeric only)";
	std::cout << "Please enter name: ";
	std::string userName;
	std::getline(std::cin, userName);
	std::cout << std::endl;

	if (!userName.empty() && userName.size() <= MAX_USER_NAME)
	{
		//  strip out any unusable user names
		auto fnd = std::find_if(userName.begin(), userName.end(), [](const char c) {
			return std::isalnum(c) == 0;
		});

		//  didnt find a non-alpha-numeric character?
		if (fnd == userName.end())
		{
			//  create the add user message for the front end to pass on
			auto msg = AddUserMessage::create(m_frontEnd.getBackEndID(), m_frontEnd.getID(), userName);			
			m_frontEnd.push(msg);

			std::cout << "Waiting..." << std::endl;
			std::cout << std::endl;
			m_statemachine.changeState(STATE_ID::eWAIT_STATE_ID);
			return true;
		}
	}
	else
	{
		std::cout << "Invalid Input" << std::endl;
		std::cout << std::endl << "Enter any key and press return to continue.....";
		std::string str;
		std::getline(std::cin, str);
	}

	m_statemachine.changeState(STATE_ID::eMENU_STATE_ID);
    return true;
}

}