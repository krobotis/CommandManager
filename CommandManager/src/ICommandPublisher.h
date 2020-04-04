#ifndef ICOMMANDPUBLISHER_H
#define ICOMMANDPUBLISHER_H

namespace commandmanager {

class ICommandListener;

class ICommandPublisher
{
public:
	virtual ~ICommandPublisher() {}

	virtual void addListener(ICommandListener& listener) = 0;
	virtual void removeListener(ICommandListener& listener) = 0;
};

} // namespace commandmanager

#endif // ICOMMANDPUBLISHER_H