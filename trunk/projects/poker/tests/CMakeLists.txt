file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h" "*.rc" "*.def")

add_executable(tests ${SOURCES} ${HEADERS} ${CLIENT_SRC})
set_target_properties(tests PROPERTIES FOLDER "poker")
target_link_libraries(tests
	evaluator
	common
	pokercmn
	gtest
	gtest_main
	protocols
	libprotobuf
	clientlib
)