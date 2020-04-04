## CommandManager
A cpp thread safe library that maps text to functors which are executed at runtime. The text is received by reading a growing text file.

### Sample usage
```C++
std::atomic_bool shutdown = false;
std::string filename = "commands";
auto fileReadRetryTimeout = std::chrono::milliseconds(100);

auto commandManager = CommandManagerFactory::newInstance(filename, fileReadRetryTimeout);

std::shared_ptr<IRegistrationToken> token = commandManager->getCommandRegister().add("exit",
	[&shutdown] (const std::vector<std::string>& /*args*/)
	{
		shutdown = true;
	});

while (!shutdown)
{
}
```

In a shell run `echo exit >> commands`.