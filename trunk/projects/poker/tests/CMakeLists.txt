include_directories("${NET_INC}")

file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h" "*.rc" "*.def")

add_executable(poker_tests ${SOURCES} ${HEADERS} ${CLIENT_SRC})
set_target_properties(poker_tests PROPERTIES FOLDER "poker")
target_link_libraries(poker_tests
	evaluator
	common
	net
	pokercmn
	gtest
	gtest_main
	protocols
	libprotobuf
	clientlib
	neuro
)