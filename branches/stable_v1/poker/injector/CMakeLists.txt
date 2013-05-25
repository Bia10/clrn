file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h" "*.rc" "*.def")

add_executable(injector ${SOURCES} ${HEADERS})
set_target_properties(injector PROPERTIES FOLDER "poker")