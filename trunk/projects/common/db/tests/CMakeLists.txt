include_directories("${NET_INC}")

file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h" "*.rc" "*.def")

add_executable(db_tests ${SOURCES} ${HEADERS} ${CLIENT_SRC})
set_target_properties(db_tests PROPERTIES FOLDER "poker")
target_link_libraries(db_tests
	common
	db
	gtest
	gtest_main
)