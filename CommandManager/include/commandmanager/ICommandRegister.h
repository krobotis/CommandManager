#ifndef ICOMMANDREGISTER_H
#define ICOMMANDREGISTER_H

#include "CommandManager_EXPORTS.h"

#include <string>
#include <functional>   // for std::function
#include <vector>
#include <memory>       // for std::shared_ptr

namespace commandmanager {

/// @brief A token that can be used to control the lifetime of the command registration.
/// The registration is removed either explicitely or when the token is destroyed.
class CommandManager_EXPORT IRegistrationToken
{
public:
	virtual ~IRegistrationToken() {}

	virtual void unregister() = 0;
};

/// @brief Defines an interface to map a text command to an operation. When the command is received
/// the operation is executed.
class CommandManager_EXPORT ICommandRegister
{
public:
	virtual ~ICommandRegister() {}

	/// @brief This method maps a command text to an operation. When the command is received, it will trigger
	/// the operation.
	/// @param[in] command The command that when received will trigger an operation which cannot contain commas (",").
	/// @param[in] func The operation that will be triggered. It will be called with the comma separated arguments. For
	/// example if the command is "log everything,2", then arg[0]="log everything" and arg[1]="2".
	/// @return a token that can be used to unregister the command. When the token is destroyed the command will be unregisted.
	virtual std::shared_ptr<IRegistrationToken> add(const std::string& command, std::function<void(const std::vector<std::string>&)>&& func) = 0;
};

} // namespace commandmanager

#endif // ICOMMANDREGISTER_H