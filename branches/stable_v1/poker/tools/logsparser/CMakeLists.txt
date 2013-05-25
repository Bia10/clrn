file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h" "*.rc" "*.def")

add_executable(logsparser ${SOURCES} ${HEADERS})
set_target_properties(logsparser PROPERTIES FOLDER "poker")