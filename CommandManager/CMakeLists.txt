add_library(CommandManager STATIC
    include/commandmanager/CommandManagerFactory.h
    include/commandmanager/ICommandManager.h
    include/commandmanager/ICommandRegister.h
	src/CommandManager.cpp
    src/CommandManager.h
    src/CommandManagerFactory.cpp
    src/CommandResolver.cpp
    src/CommandResolver.h
    src/FileWatcher.cpp
    src/FileWatcher.h
    src/ICommandListener.h
    src/ICommandPublisher.h
)

include (GenerateExportHeader)          

GENERATE_EXPORT_HEADER(CommandManager           # generates the export header CommandManager_EXPORTS.h automatically
    BASE_NAME CommandManager
    EXPORT_MACRO_NAME CommandManager_EXPORT
    EXPORT_FILE_NAME CommandManager_EXPORTS.h
    STATIC_DEFINE SHARED_EXPORTS_BUILT_AS_STATIC
)

target_include_directories(CommandManager PUBLIC
    include
    ${CMAKE_CURRENT_BINARY_DIR}         # for CommandManager_EXPORTS.h
)

target_link_libraries(CommandManager PRIVATE
    Logger
)

target_compile_definitions(CommandManager PRIVATE ENABLE_SINGLETON_COMMANDMANAGER=true)
