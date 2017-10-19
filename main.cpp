#include "Replica.hpp"
#include <iostream>

const int REQUEST_COUNT = 10000;

int main()
{
	Replica r0(0);
	Replica r1(1);
	Replica r2(2);
	Replica r3(3);
	Replica r4(4);

	r0.addOtherReplica(&r1);
	r0.addOtherReplica(&r2);
	r0.addOtherReplica(&r3);
	r0.addOtherReplica(&r4);

	r1.addOtherReplica(&r0);
	r1.addOtherReplica(&r2);
	r1.addOtherReplica(&r3);
	r1.addOtherReplica(&r4);

	r2.addOtherReplica(&r0);
	r2.addOtherReplica(&r1);
	r2.addOtherReplica(&r3);
	r2.addOtherReplica(&r4);

	r3.addOtherReplica(&r0);
	r3.addOtherReplica(&r1);
	r3.addOtherReplica(&r2);
	r3.addOtherReplica(&r4);

	r4.addOtherReplica(&r0);
	r4.addOtherReplica(&r1);
	r4.addOtherReplica(&r2);
	r4.addOtherReplica(&r3);

	r0.start();
	r1.start();
	r2.start();
	r3.start();
	r4.start();

	srand(time(NULL));

	std::thread t0([&r0]()
	{
		int i = 0;
		while (++i <= REQUEST_COUNT)
		{
			Command c;
			c.m_type = (Command::CommandType)(rand() % c.CommandTypeNum);
			c.m_operand = rand() % 100;
			r0.onReceiveRequest(c);
		}
	});

	std::thread t1([&r1]()
	{
		int i = 0;
		while (++i <= REQUEST_COUNT)
		{
			Command c;
			c.m_type = (Command::CommandType)(rand() % c.CommandTypeNum);
			c.m_operand = rand() % 100;
			r1.onReceiveRequest(c);
		}
	});

	std::thread t2([&r2]()
	{
		int i = 0;
		while (++i <= REQUEST_COUNT)
		{
			Command c;
			c.m_type = (Command::CommandType)(rand() % c.CommandTypeNum);
			c.m_operand = rand() % 100;
			r2.onReceiveRequest(c);
		}
	});

	std::thread t3([&r3]()
	{
		int i = 0;
		while (++i <= REQUEST_COUNT)
		{
			Command c;
			c.m_type = (Command::CommandType)(rand() % c.CommandTypeNum);
			c.m_operand = rand() % 100;
			r3.onReceiveRequest(c);
		}
	});

	std::thread t4([&r4]()
	{
		int i = 0;
		while (++i <= REQUEST_COUNT)
		{
			Command c;
			c.m_type = (Command::CommandType)(rand() % c.CommandTypeNum);
			c.m_operand = rand() % 100;
			r4.onReceiveRequest(c);
		}
	});

	
	t0.join();
	t1.join();
	t2.join();
	t3.join();
	t4.join();

	system("pause");
	std::cout << "r0 state: " << r0.getState() << std::endl;
	std::cout << "r1 state: " << r1.getState() << std::endl;
	std::cout << "r2 state: " << r2.getState() << std::endl;
	std::cout << "r3 state: " << r3.getState() << std::endl;
	std::cout << "r4 state: " << r4.getState() << std::endl;

	system("pause");
	return 0;
}