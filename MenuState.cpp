#include "MenuState.h"


// stl
#include <iostream>
#include <string>

//  app
#include "StateMachine.h"
#include "Message.h"
#include "FrontEnd.h"

namespace MessageService {

bool MenuState::update()
{
	// clear screen
	for (int i = 0; i < 80; ++i) std::cout << std::endl;
	// show options
	std::cout << "Please select an option:" << std::endl;
	std::cout << "1. Create User" << std::endl;
	std::cout << "2. Send Message" << std::endl;
	std::cout << "3. Receive All Messages For User" << std::endl;
	std::cout << "4. Report" << std::endl;
	std::cout << "5. Quit" << std::endl;
	std::string in;
	std::getline(std::cin, in);
	std::cout << std::endl;

	if (in == "1")
	{
		m_statemachine.changeState(STATE_ID::eADD_USER_STATE_ID);
	}
	else if (in == "2")
	{
		m_statemachine.changeState(STATE_ID::eSEND_MSG_STATE_ID);
	}
	else if (in == "3")
	{
		m_statemachine.changeState(STATE_ID::eGET_ALL_MESSAGES_STATE_ID);
	}
	else if (in == "4")
	{
		//  create the message for the front end to pass on
		auto msg = GetReport::create(m_frontEnd.getBackEndID(), m_frontEnd.getID());
		m_frontEnd.push(msg);

		std::cout << "Waiting..." << std::endl;
		std::cout << std::endl;
		m_statemachine.changeState(STATE_ID::eWAIT_STATE_ID);
	}
	else if (in == "5")
	{
		std::cout << "Quitting!" << std::endl;
		return false;
	}
	else
	{
		std::cout << "Invalid Option Selected" << std::endl;
		std::cout << std::endl << "Enter any key and press return to continue.....";
		std::string str;
		std::getline(std::cin, str);
	}

	return true;
}
}
