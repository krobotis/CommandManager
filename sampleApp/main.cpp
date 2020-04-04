#include <commandmanager/CommandManagerFactory.h>
#include <commandmanager/ICommandRegister.h>
#include <commandmanager/ICommandManager.h>
#include <logger/Logger.h>

#include <sstream>
#include <string>
#include <atomic>
#include <thread>
#include <functional>		// for std::ref
#include <chrono>

using namespace logging;
using namespace commandmanager;

void foo(const std::string& name, ICommandRegister& reg)
{
	std::atomic_bool shutdown = false;
	Logger logger(name);

	logger << LogLevel::Info << "Starting\n";

	std::shared_ptr<IRegistrationToken> token = reg.add("exit", [&shutdown, &logger](const std::vector<std::string>& /*args*/)
		{
			logger << LogLevel::Info << "Initiating shutdown\n";

			shutdown = true;
		});

	std::shared_ptr<IRegistrationToken> token2 = reg.add("print", [&shutdown, &logger](const std::vector<std::string>& args)
		{
			std::string str;
			if (args.size() > 1)
			{
				str = args[1];
			}

			logger << LogLevel::Info << "Print: " << str << "\n";
		});

	while (!shutdown)
	{
	}

	token->unregister();
	token2->unregister();

	logger << LogLevel::Info << "Shutdown completed\n";
}


int main()
{
	std::string filename = "commands";
	auto fileReadRetryTimeout = std::chrono::milliseconds(100);

	auto commandManager = CommandManagerFactory::newInstance(filename, fileReadRetryTimeout);

	std::thread t1(&foo, "thread1", std::ref(commandManager->getCommandRegister()));
	std::thread t2(&foo, "thread2", std::ref(commandManager->getCommandRegister()));
	std::thread t3(&foo, "thread3", std::ref(commandManager->getCommandRegister()));
	std::thread t4(&foo, "thread4", std::ref(commandManager->getCommandRegister()));
	std::thread t5(&foo, "thread5", std::ref(commandManager->getCommandRegister()));

	foo("main.func", std::ref(CommandManagerFactory::getInstance()->getCommandRegister()));

	t1.join();
	t2.join();
	t3.join();
	t4.join();
	t5.join();

	CommandManagerFactory::shutdownInstance();

	return 0;
}
