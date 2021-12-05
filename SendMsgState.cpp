#include "SendMsgState.h"


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
	const uint32_t MAX_MESSAGE = 140;
}

bool SendMsgState::update()
{
	bool fromOK = false;
	bool toOK = false;
	bool messageOK = false;

	std::cout << "From: ";
	std::string from;
	std::getline(std::cin, from);
	std::cout << std::endl;

	std::cout << "To: ";
	std::string to;
	std::getline(std::cin, to);
	std::cout << std::endl;

	std::cout << "Message: ";
	std::string msg;
	std::getline(std::cin, msg);
	std::cout << std::endl;

	if (!from.empty() && from.size() <= MAX_USER_NAME)
	{
		//  strip out any unusable user names
		auto fnd = std::find_if(from.begin(), from.end(), [](const char c) {
			return std::isalnum(c) == 0;
		});

		//  didnt find a non-alpha-numeric character?
		if (fnd == from.end())
		{
			fromOK = true;
		}
	}

	if (!to.empty() && to.size() <= MAX_USER_NAME)
	{
		//  strip out any unusable user names
		auto fnd = std::find_if(to.begin(), to.end(), [](const char c) {
			return std::isalnum(c) == 0;
		});

		//  didnt find a non-alpha-numeric character?
		if (fnd == to.end())
		{
			toOK = true;
		}
	}

	if (!msg.empty() && msg.size() <= MAX_MESSAGE)
	{
		//  strip out any unusable messages
		auto fnd = std::find_if(msg.begin(), msg.end(), [](const char c) {
			if (std::isalnum(c) == 0)
			{
				if (std::isspace(c) == 0)
				{
					return true;
				}
			}

			return false;
		});

		//  didnt find a non-alpha-numeric character?
		if (fnd == msg.end())
		{
			messageOK = true;
		}
	}

	if (messageOK && toOK && fromOK)
	{
		//  create the add user message for the front end to pass on
		auto request = SendMessage::create(m_frontEnd.getBackEndID(), m_frontEnd.getID(), from, to, msg);
		m_frontEnd.push(request);

		std::cout << "Waiting..." << std::endl;
		std::cout << std::endl;
		m_statemachine.changeState(STATE_ID::eWAIT_STATE_ID);
		return true;
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