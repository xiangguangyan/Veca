#pragma once

#include "Command.hpp"
#include <thread>
#include <mutex>
#include <map>
#include <iostream>

class Replica
{
public:
	Replica(unsigned int id):
		m_id(id),
		m_isRunning(false),
		m_state(100)
	{
		m_vectorClock.addClock(Clock(m_id, 0));
	}

	~Replica()
	{
		m_isRunning = false;
		m_runCommandThread.join();
	}

	void start()
	{
		m_isRunning = true;
		m_runCommandThread.thread::thread(&Replica::run, this);
	}

	int getState()
	{
		return m_state;
	}

	bool addOtherReplica(Replica* replica)
	{
		if (m_otherReplicas.insert(std::pair<unsigned int, Replica*>(replica->m_id, replica)).second)
		{
			return m_vectorClock.addClock(Clock(replica->m_id, 0));
		}
		return false;
	}

	void onReceiveRequest(Command& command)
	{
		m_mutex.lock();
		command.m_currentState = 0;
		command.m_id.m_replica = m_id;
		command.m_id.m_version = m_vectorClock.increase(m_id);
		command.m_vectorClock = m_vectorClock;
		
		m_mutex.unlock();

		//std::cout << "Receive command (" << command.m_id.m_replica << ',' << command.m_id.m_version << ")\n";

		onReceivePropose(command);
		for (auto it = ++m_otherReplicas.begin(); it != m_otherReplicas.end(); ++it)
		{
			it->second->onReceivePropose(command);
		}
	}

	bool runCommand(Command& command)
	{
		if (command.m_currentState != -1)
		{
			return false;
		}

		command.m_currentState = -2;

		Clock clock = command.m_id;
		while (clock.m_version > 1)
		{
			--clock.m_version;
			auto it = m_commands.find(clock);
			if (it == m_commands.end() || it->second.m_currentState < -1)
			{
				break;
			}
			if (it->second.m_currentState > -1 || !runCommand(it->second))
			{
				command.m_currentState = -1;
				return false;
			}
		}

		std::vector<Clock>& vectorClock = command.m_vectorClock.getVector();
		for (const Clock& c : vectorClock)
		{
			if (c.m_version == 0 || c.m_replica == clock.m_replica)
			{
				continue;
			}

			auto it = m_commands.find(c);
			if (it == m_commands.end() || it->second.m_currentState < -1)
			{
				continue;
			}
			if (it->second.m_currentState > -1 || !runCommand(it->second))
			{
				command.m_currentState = -1;
				return false;
			}
		}

		m_state = command.execute(m_state);

		clock_t duration = ::clock() - command.m_clock;
		std::cout << "Replica " << m_id << " run command (" << command.m_id.m_replica << ',' << command.m_id.m_version << "), state:" << m_state << ", replay time:" << duration << std::endl;

		command.m_currentState = -3;
	
		return true;
	}

	void run()
	{
		while (m_isRunning)
		{
			m_mutex.lock();
			for (std::map<Clock, Command>::iterator it = m_commands.begin(); it != m_commands.end();)
			{
				if (it->second.m_currentState == -3)
				{
					it = m_commands.erase(it);
				}
				else
				{
					break;
				}
			}

			for (std::map<Clock, Command>::iterator it = m_commands.begin(); it != m_commands.end(); ++it)
			{
				if (it->second.m_currentState == -1)
				{
					if (!runCommand(it->second))
					{
						break;
					}
				}
			}
			m_mutex.unlock();
			//std::this_thread::sleep_for(std::chrono::microseconds(10));
		}
	}

private:
	void onReceivePropose(Command& command)
	{
		m_mutex.lock();
		m_vectorClock.update(command.m_vectorClock);

		command.m_vectorClock = m_vectorClock;
		m_commands.insert(std::pair<Clock, Command>(command.m_id, command));

		m_mutex.unlock();

		if (command.m_id.m_replica == m_id)
		{
			onReceiveAccept(command);
		}
		else
		{
			auto it = m_otherReplicas.find(command.m_id.m_replica);
			it->second->onReceiveAccept(command);
		}
	}

	void onReceiveAccept(const Command& command)
	{
		Command* c = nullptr;
		m_mutex.lock();

		auto it = m_commands.find(command.m_id);

		if (it == m_commands.end() || it->second.m_currentState < 0)
		{
			m_mutex.unlock();
			return;
		}

		m_vectorClock.update(command.m_vectorClock);
			
		it->second.m_vectorClock = m_vectorClock;

		if (++it->second.m_currentState > (m_otherReplicas.size() + 1) / 2)
		{
			c = &it->second;	
		}
	
		m_mutex.unlock();

		if (c != nullptr)
		{
			Command tmp = *c;
			onReceiveCommit(tmp);
			for (auto it = m_otherReplicas.begin(); it != m_otherReplicas.end(); ++it)
			{
				it->second->onReceiveCommit(tmp);
			}
		}
	}

	void onReceiveCommit(const Command& command)
	{
		m_mutex.lock();
		
		auto pair = m_commands.insert(std::pair<Clock, Command>(command.m_id, command));
		pair.first->second.m_currentState = -1;
		if (!pair.second)
		{
			pair.first->second.m_vectorClock = command.m_vectorClock;
		}
		pair.first->second.m_clock = ::clock();
		m_mutex.unlock();
	}
	
private:
	unsigned int m_id;
	std::thread m_runCommandThread;
	bool m_isRunning;
	std::map<Clock, Command> m_commands;
	std::mutex m_mutex;

	int m_state;
	VectorClock m_vectorClock;
	std::map<unsigned int, Replica*> m_otherReplicas;
};

