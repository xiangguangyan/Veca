#pragma once

#include <vector>
#include <algorithm>

class Clock
{
public:
	Clock() :
		m_replica(0),
		m_version(0)
	{

	}

	Clock(unsigned int replica, unsigned int version) :
		m_replica(replica),
		m_version(version)
	{

	}

public:
	unsigned int m_replica;
	unsigned int m_version;
};

bool operator==(const Clock& clock1, const Clock& clock2)
{
	return clock1.m_replica == clock2.m_replica &&
		clock1.m_version == clock2.m_version;
}

bool operator!=(const Clock& clock1, const Clock& clock2)
{
	return clock1.m_replica != clock2.m_replica ||
		clock1.m_version != clock2.m_version;
}

bool operator<(const Clock& clock1, const Clock& clock2)
{
	return clock1.m_replica < clock2.m_replica ||
		!(clock2.m_replica < clock1.m_replica) && clock1.m_version < clock2.m_version;
}

class VectorClock
{
public:
	VectorClock()
	{

	}

	~VectorClock()
	{

	}

	bool addClock(const Clock& clock)
	{
		for (std::vector<Clock>::iterator it = m_vectorClock.begin(); it != m_vectorClock.end(); ++it)
		{
			if (clock.m_replica == it->m_replica)
			{
				return false;
			}
			else if (clock.m_replica < it->m_replica)
			{
				m_vectorClock.insert(it, clock);
				return true;
			}
		}

		m_vectorClock.push_back(clock);
		return true;
	}

	unsigned int increase(unsigned int replica)
	{
		for (std::vector<Clock>::iterator it = m_vectorClock.begin(); it != m_vectorClock.end(); ++it)
		{
			if (it->m_replica == replica)
			{
				++it->m_version;
				return it->m_version;
			}
		}

		return 0;
	}

	bool update(const VectorClock& vectorClock)
	{
		if (vectorClock.m_vectorClock.size() != m_vectorClock.size())
		{
			return false;
		}

		for (std::vector<Clock>::size_type i = 0; i < m_vectorClock.size(); ++i)
		{
			if (vectorClock.m_vectorClock[i].m_replica != m_vectorClock[i].m_replica)
			{
				return false;
			}

			if (vectorClock.m_vectorClock[i].m_version > m_vectorClock[i].m_version)
			{
				m_vectorClock[i].m_version = vectorClock.m_vectorClock[i].m_version;
			}
		}

		return true;
	}

	bool operator==(const VectorClock& vectorClock)
	{
		if (vectorClock.m_vectorClock.size() != m_vectorClock.size())
		{
			return false;
		}

		for (std::vector<Clock>::size_type i = 0; i < m_vectorClock.size(); ++i)
		{
			if (vectorClock.m_vectorClock[i].m_replica != m_vectorClock[i].m_replica)
			{
				return false;
			}
		}

		return true;
	}

	std::vector<Clock>& getVector()
	{
		return m_vectorClock;
	}

private:
	std::vector<Clock> m_vectorClock;
};

