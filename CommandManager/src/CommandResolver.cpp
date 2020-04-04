#include "CommandResolver.h"
#include "ICommandPublisher.h"

#include <utility>      // for std::move
#include <exception>    // for std::exception
#include <algorithm>    // for std::find_if

namespace commandmanager {
namespace {

	class RegistrationToken : public IRegistrationToken
	{
	public:
		RegistrationToken(const std::shared_ptr<CommandResolver>& commandResolver,
			const CommandResolver::RegistrationId& registrationId)
			: m_commandResolver(commandResolver)
			, m_registrationId(registrationId)
		{
		}

		~RegistrationToken()
		{
			unregister();
		}

		void unregister() override
		{
			m_commandResolver->remove(m_registrationId);
		}

	private:
		std::shared_ptr<CommandResolver> m_commandResolver;
		CommandResolver::RegistrationId m_registrationId;
	};

} // anonymous-namespace

CommandResolver::CommandResolver()
	: m_commandPublisher()
	, m_nextRegistrationId(1)
{
}

CommandResolver::~CommandResolver()
{
	unregister();
}

std::shared_ptr<IRegistrationToken> CommandResolver::add(const std::string& text, std::function<void(const std::vector<std::string>&)>&& command)
{
	std::lock_guard<std::mutex> lock(m_mapMutex);

	RegistrationId registrationId = m_nextRegistrationId++;

	FuncIdPair funcIdPair = { std::move(command), registrationId };

	m_textToCommands.emplace(text, std::move(funcIdPair));

	return std::make_shared<RegistrationToken>(shared_from_this(), registrationId);
}

void CommandResolver::remove(const RegistrationId& id)
{
	std::lock_guard<std::mutex> lock(m_mapMutex);

	auto it = std::find_if(m_textToCommands.begin(), m_textToCommands.end(), [&id](const std::pair<std::string, FuncIdPair>& elem)
		{
			return elem.second.id == id;
		});

	if (it != m_textToCommands.end())
	{
		m_textToCommands.erase(it);
	}
}

void CommandResolver::doRegister(std::shared_ptr<ICommandPublisher>& commandPublisher)
{
	if (commandPublisher)
	{
		commandPublisher->addListener(*this);
	}

	m_commandPublisher = commandPublisher;
}

void CommandResolver::unregister()
{
	if (auto commandPublisher = m_commandPublisher.lock())
	{
		commandPublisher->removeListener(*this);
	}
}

void CommandResolver::newCommand(const std::vector<std::string>& tokens)
{
	if (!tokens.empty())
	{
		std::string command = tokens.front();

		std::lock_guard<std::mutex> lock(m_mapMutex);

		auto range = m_textToCommands.equal_range(command);

		for (auto i = range.first; i != range.second; ++i)
		{
			if (i->second.func)
			{
				i->second.func(tokens);
			}
		}
	}
}

} // namespace commandmanager