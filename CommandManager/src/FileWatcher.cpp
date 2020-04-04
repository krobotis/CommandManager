#include "FileWatcher.h"
#include "ICommandListener.h"

#include <fstream>                        // for std::ifstream, std::ofstream
#include <algorithm>                      // for std::remove
#include <utility>                        // for std::move
#include <exception>                      // for std::exception

namespace commandmanager {
namespace {
	
	std::vector<std::string> split(const std::string& s, char delimiter)
	{
		std::vector<std::string> tokens;
		std::string token;
		std::istringstream tokenStream(s);
		
		while (std::getline(tokenStream, token, delimiter))
		{
			tokens.push_back(token);
		}
		
		return tokens;
	}

	void createFileIfNotExists(const std::string& name, logging::Logger& logger)
	{
		std::ofstream ofs(name, std::ofstream::out | std::ofstream::app);

		if (!ofs)
		{
			logger << logging::LogLevel::Error << "Error opening file: " << name << "\n";
		}
	}

} // anonymous-namespace

FileWatcher::FileWatcher(const std::string& filename, const std::chrono::milliseconds& fileReadRetryTimeout)
	: m_filename(filename)
	, m_listeners()
	, m_listenerMutex()
	, m_memberMutex()
	, m_threadMutex()
	, m_logger("filewatcher")
	, m_ifs()
	, m_lineSs()
	, m_fileReadRetryTimeout(fileReadRetryTimeout)
	, m_thread()
	, m_shutdown(false)
	, m_cv()
{
}

FileWatcher::~FileWatcher()
{
	try {
		stop();
	}
	catch (...)
	{
		m_logger << logging::LogLevel::Error << "Exception when stopping thread.\n";
	}
}

void FileWatcher::addListener(ICommandListener& listener)
{
	std::lock_guard<std::mutex> lock(m_listenerMutex);

	m_listeners.emplace_back(&listener);
}

void FileWatcher::removeListener(ICommandListener& listener)
{
	std::lock_guard<std::mutex> lock(m_listenerMutex);

	m_listeners.erase(std::remove(m_listeners.begin(), m_listeners.end(), &listener), m_listeners.end());
}

void FileWatcher::start()
{
	m_shutdown = false;

	std::lock_guard<std::mutex> lock(m_memberMutex);

	createFileIfNotExists(m_filename, m_logger);

	// open file (at end)
	m_ifs.open(m_filename, std::ifstream::ate);

	if (!m_ifs.is_open())
	{
		m_logger << logging::LogLevel::Error << "Error opening file: " << m_filename << '\n';
		return;
	}

	m_thread = std::thread(&FileWatcher::run, this);
}

void FileWatcher::run()
{
	std::unique_lock<std::mutex> lock(m_memberMutex);

	while (true)
	{
		tryReadNextLine();

		if (m_cv.wait_for(lock, m_fileReadRetryTimeout, [this] { return this->m_shutdown.load(); }))
		{
			break;
		}
	}
}

void FileWatcher::tryReadNextLine()
{
	while (true)
	{
		char c;

		if (!m_ifs.get(c) || m_ifs.eof())
		{
			// if we fail, clear stream
			m_ifs.clear();

			// and wait to try again
			break;
		}

		if (c == '\n')
		{
			m_logger << logging::LogLevel::Info << "Publishing: " << m_lineSs.str() << "\n";

			auto tokens = split(m_lineSs.str(), ',');

			publish(tokens);

			// clear the stream
			m_lineSs.str(std::string());
		}
		else
		{
			m_lineSs << c;
		}
	}
}

void FileWatcher::stop()
{
	m_shutdown = true;
	m_cv.notify_one();

	{
		std::lock_guard<std::mutex> lock(m_threadMutex);
	
		if (m_thread.joinable())
		{
			m_thread.join();
		}
	}

	std::lock_guard<std::mutex> lock(m_memberMutex);
	m_ifs.close();
}

void FileWatcher::publish(const std::vector<std::string>& tokens)
{
	std::lock_guard<std::mutex> lock(m_listenerMutex);

	for (auto& listener : m_listeners)
	{
		try {
			listener->newCommand(tokens);
		}
		catch (std::exception& e)
		{
			m_logger << logging::LogLevel::Error << "Listener exception:" << e.what() << "\n";
		}
	}
}

} // namespace commandmanager