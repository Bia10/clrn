file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h" "*.rc" "*.def")

add_executable(db_converter ${SOURCES} ${HEADERS})
set_target_properties(db_converter PROPERTIES FOLDER "tools")
target_link_libraries(db_converter
	serverlib
)