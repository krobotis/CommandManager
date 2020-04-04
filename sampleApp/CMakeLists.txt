cmake_minimum_required (VERSION 3.8)

add_executable (sampleApp
	main.cpp
)

target_link_libraries(sampleApp PRIVATE
	Logger
	CommandManager
)