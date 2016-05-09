# TODO: Change this when 1.61 released
find_package(Boost 1.60
    COMPONENTS filesystem
    locale
    program_options
    system
    REQUIRED
    )
find_package(Threads REQUIRED)
find_package(YamlCpp REQUIRED)
include_directories("${PROJECT_SOURCE_DIR}/include")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
configure_file(config.h.in config.h)
add_library(cenisys SHARED
    command/defaultcommandhandlers.cpp
    config/configsection.cpp
    server/server.cpp
    server/terminal/terminalcolor.cpp
    server/terminal/threadedterminalconsole.cpp
    server/terminal/posixasyncterminalconsole.cpp
    server/configmanager.cpp
    )
target_link_libraries(cenisys PRIVATE
    Threads::Threads
    Boost::boost
    Boost::filesystem
    Boost::locale
    Boost::system
    YamlCpp
    )
add_executable(cenisyslauncher main.cpp)
target_link_libraries(cenisyslauncher
    cenisys
    Threads::Threads
    Boost::boost
    Boost::filesystem
    Boost::locale
    Boost::program_options
    Boost::system
    YamlCpp
    )
set_property(TARGET cenisyslauncher cenisys PROPERTY CXX_STANDARD 14)
set_property(TARGET cenisyslauncher cenisys PROPERTY CXX_STANDARD_REQUIRED YES)
install(TARGETS cenisys cenisyslauncher
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    )
