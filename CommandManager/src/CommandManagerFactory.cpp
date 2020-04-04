#include <commandManager/CommandManagerFactory.h>
#include <commandManager/ICommandManager.h>
#include <commandManager/ICommandRegister.h>
#include "CommandManager.h"
#include "FileWatcher.h"

#include <atomic>		// for std::atomic_store, std::atomic_load
#include <utility>		// for std::move

namespace commandmanager {

namespace {

	constexpr bool activate = ENABLE_SINGLETON_COMMANDMANAGER;
	const std::string filename = "commands";
	std::chrono::milliseconds FileReadRetryTimeout(100);

	class NullCommandManager : public ICommandManager
	{
	public:
		NullCommandManager()
		{
		}

		ICommandRegister& getCommandRegister() override
		{
			return m_commandRegister;
		}

	private:
		class NullCommandRegister : public ICommandRegister
		{
		public:
			std::shared_ptr<IRegistrationToken> add(const std::string& /*text*/, std::function<void(const std::vector<std::string>&)>&& /*command*/) override
			{
				return std::make_shared<NullRegistrationToken>();
			}

		private:
			class NullRegistrationToken : public IRegistrationToken
			{
			public:
				void unregister() override
				{
				}
			};
		};

		NullCommandRegister m_commandRegister;
	};

	std::shared_ptr<ICommandManager> conditionallyConstructInstance()
	{
		if (activate)
		{
			return CommandManagerFactory::newInstance(filename, FileReadRetryTimeout);
		}

		return std::make_shared<NullCommandManager>();
	}

	std::shared_ptr<ICommandManager>& getInstanceRef()
	{
		// Meyers Singleton
		static std::shared_ptr<ICommandManager> commandManager = conditionallyConstructInstance();

		return commandManager;
	}

} // anonymous-namespace

std::unique_ptr<ICommandManager> CommandManagerFactory::newInstance(
	const std::string& filename,
	const std::chrono::milliseconds& fileReadRetryTimeout)
{
	auto fileWatcher = std::make_shared<FileWatcher>(filename, fileReadRetryTimeout);
	fileWatcher->start();

	return std::unique_ptr<CommandManager>(new CommandManager(std::move(fileWatcher)));
}

std::shared_ptr<ICommandManager> CommandManagerFactory::getInstance()
{
	return std::atomic_load(&getInstanceRef());
}

void CommandManagerFactory::shutdownInstance()
{
	if (activate)
	{
		std::atomic_store(&getInstanceRef(), (std::shared_ptr<ICommandManager>) std::make_shared<NullCommandManager>());
	}
}

} // namespace commandmanager