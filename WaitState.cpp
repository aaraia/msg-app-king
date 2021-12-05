#include "WaitState.h"


// stl
#include <iostream>
#include <string>

//  app
#include "StateMachine.h"

namespace MessageService {

namespace {
	const uint32_t TIMEOUT = 10;
}

bool WaitState::update()
{
	if (m_firstPass)
	{
		m_start = std::chrono::steady_clock::now();
		m_firstPass = false;
	}

	auto now = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed_seconds = now - m_start;
	if (elapsed_seconds.count() > TIMEOUT)
	{
		m_firstPass = true;

		//  dont wait endlessly for the backend
		// timeout and return to menu
		m_statemachine.changeState(STATE_ID::eMENU_STATE_ID);
	}

	return true;
}

}