#ifndef ICOMMANDMANAGER_H
#define ICOMMANDMANAGER_H

#include "CommandManager_EXPORTS.h"

namespace commandmanager {

class ICommandRegister;

class CommandManager_EXPORT ICommandManager
{
public:
	virtual ~ICommandManager() {}

	virtual ICommandRegister& getCommandRegister() = 0;
};

} // namespace commandmanager

#endif // ICOMMANDMANAGER_H