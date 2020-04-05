#ifndef COMMANDMANAGER_H
#define COMMANDMANAGER_H

#include <commandmanager/ICommandManager.h>

#include <memory>    // for std::shared_ptr

namespace commandmanager {

class ICommandRegister;
class ICommandPublisher;

/// @brief Container of a command publisher and register.
class CommandManager : public ICommandManager
{
public:
	CommandManager(std::shared_ptr<ICommandPublisher>&& commandPublisher);

	ICommandRegister& getCommandRegister() override;

private:
	std::shared_ptr<ICommandRegister> m_commandRegister;
	std::shared_ptr<ICommandPublisher> m_commandPublisher;
};

} // namespace commandmanager

#endif // COMMANDMANAGER_H