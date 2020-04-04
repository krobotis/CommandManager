#ifndef ICOMMANDLISTENER_H
#define ICOMMANDLISTENER_H

#include <vector>
#include <string>

namespace commandmanager {

class ICommandListener
{
public:
	virtual ~ICommandListener() {}

	virtual void newCommand(const std::vector<std::string>& tokens) = 0;
};

} // namespace commandmanager

#endif // ICOMMANDLISTENER_H
