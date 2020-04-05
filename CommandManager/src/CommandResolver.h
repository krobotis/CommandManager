#ifndef COMMANDRESOLVER_H
#define COMMANDRESOLVER_H

#include <commandmanager/ICommandRegister.h>
#include "ICommandListener.h"

#include <string>
#include <vector>
#include <map>             // for std::multimap
#include <memory>          // for std::weak_ptr, std::shared_ptr, std::enable_shared_from_this
#include <functional>      // for std::function
#include <cstdint>         // for std::uint64_t
#include <mutex>

namespace commandmanager {

class ICommandPublisher;

class CommandResolver : public ICommandRegister,
	public std::enable_shared_from_this<CommandResolver>,
	private ICommandListener
{
public:
	using RegistrationId = std::uint64_t;

	CommandResolver();
	~CommandResolver();

	std::shared_ptr<IRegistrationToken> add(const std::string& text, std::function<void(const std::vector<std::string>&)>&& command) override;
	void remove(const RegistrationId& id);

	void doRegister(std::shared_ptr<ICommandPublisher>& commandPublisher);

private:
	struct FuncIdPair
	{
		std::function<void(const std::vector<std::string>&)> func;
		RegistrationId id;
	};

	void newCommand(const std::vector<std::string>& tokens) override;
	void unregister();

	std::multimap<std::string, FuncIdPair> m_textToCommands;
	std::mutex m_mapMutex;

	std::weak_ptr<ICommandPublisher> m_commandPublisher;
	RegistrationId m_nextRegistrationId;
};

} // namespace commandmanager

#endif // COMMANDRESOLVER_H