#pragma once

#include "VectorClock.hpp"
#include <time.h>

class Command
{
public:
	enum CommandType
	{
		ADD,
		SUB,
		MUL,
		DIV,
		POW,
		MOD,
		CommandTypeNum
	};

public:
	Command():
		m_type(ADD),
		m_operand(0),
		m_currentState(0)
	{

	}

	~Command()
	{

	}

	int execute(int state)
	{
		switch (m_type)
		{
		case ADD:
			state += m_operand;
			break;
		case SUB:
			state -= m_operand;
			break;
		case MUL:
			state *= m_operand;
			break;
		case DIV:
			if (m_operand != 0)
			{
				state /= m_operand;
			}
			break;
		case POW:
			state = (int)::pow(state, m_operand);
			break;
		case MOD:
			if (m_operand != 0)
			{
				state %= m_operand;
			}
			break;
		}

		return state;
	}

	CommandType m_type;
	int m_operand;
	Clock m_id;
	VectorClock m_vectorClock;
	int m_currentState;		//0: received, >0: responsed count, -1: committed, -2: replaying, -3: replayed
	clock_t m_clock;
};

