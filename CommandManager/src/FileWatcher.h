#ifndef FILEWATCHER_H
#define FILEWATCHER_H

#include "ICommandPublisher.h"
#include <logger/Logger.h>

#include <string>
#include <vector>
#include <fstream>				 // for std::ifstream
#include <sstream>
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <chrono>

namespace commandmanager {

class ICommandListener;

/// @brief Reads a growing text file and publishes each line separating it by commas.
class FileWatcher : public ICommandPublisher
{
public:
	/// @brief ctor
	/// @param[in] filename The name of the file that will be watched.
	/// @param[in] fileReadRetryTimeout The milliseconds for a retry after a failed attempt to read
	/// text from the file.
	FileWatcher(const std::string& filename, const std::chrono::milliseconds& fileReadRetryTimeout);

	virtual ~FileWatcher();

	void addListener(ICommandListener& listener) override;
	void removeListener(ICommandListener& listener) override;

	void start();
	void stop();

private:
	void publish(const std::vector<std::string>& tokens);
	void run();
	void tryReadNextLine();

	std::string m_filename;
	std::vector<ICommandListener*> m_listeners;
	std::mutex m_listenerMutex;
	std::mutex m_memberMutex;
	std::mutex m_threadMutex;
	logging::Logger m_logger;
	std::ifstream m_ifs;
	std::stringstream m_lineSs;
	std::chrono::milliseconds m_fileReadRetryTimeout;
	std::thread m_thread;
	std::atomic_bool m_shutdown;
	std::condition_variable m_cv;
};

} // namespace commandmanager

#endif // FILEWATCHER_H
