#include "CommandManager.h"
#include "FileWatcher.h"
#include "CommandResolver.h"

#include <utility>		// for std::move

namespace commandmanager {

CommandManager::CommandManager(std::shared_ptr<ICommandPublisher>&& commandPublisher)
	: m_commandRegister()
	, m_commandPublisher(std::move(commandPublisher))
{
	std::shared_ptr<CommandResolver> commandResolver = std::make_shared<CommandResolver>();
	commandResolver->doRegister(m_commandPublisher);

	m_commandRegister = commandResolver;
}

ICommandRegister& CommandManager::getCommandRegister()
{
	return *m_commandRegister;
}

} // namespace commandmanager