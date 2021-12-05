#include "GetAllMsgState.h"


// stl
#include <iostream>
#include <string>

//  app
#include "StateMachine.h"
#include "FrontEnd.h"
#include "Message.h"

namespace MessageService {

bool GetAllMsgState::update()
{
	std::cout << "Enter name of user to receive all messages for: ";
	std::string user;
	std::getline(std::cin, user);
	std::cout << std::endl;

	//  create the message for the front end to pass on
	auto msg = GetMessages::create(m_frontEnd.getBackEndID(), m_frontEnd.getID(), user);
	m_frontEnd.push(msg);

	std::cout << "Waiting..." << std::endl;
	std::cout << std::endl;
	m_statemachine.changeState(STATE_ID::eWAIT_STATE_ID);

    return true;
}

}