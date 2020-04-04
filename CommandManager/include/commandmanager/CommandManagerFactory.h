#ifndef COMMANDMANAGERFACTORY_H
#define COMMANDMANAGERFACTORY_H

#include "CommandManager_EXPORTS.h"

#include <memory>       // for std::unique_ptr, std::shared_ptr
#include <string>
#include <chrono>

namespace commandmanager {

class ICommandManager;

/// @brief Factory that provides instances which are used to map operations to text and execute
/// those operations.
class CommandManager_EXPORT CommandManagerFactory
{
public:
	/// @brief Constructs a new instance.
	static std::unique_ptr<ICommandManager> newInstance(
		const std::string& filename,
		const std::chrono::milliseconds& fileReadRetryTimeout);

	/// Singeton accessors.

	/// @brief Returns the singleton instance which is created in the first call of this method.
	static std::shared_ptr<ICommandManager> getInstance();

	/// @brief Shuts down the singleton instance.
	static void shutdownInstance();
};

} // namespace commandmanager

#endif // COMMANDMANAGERFACTORY_H