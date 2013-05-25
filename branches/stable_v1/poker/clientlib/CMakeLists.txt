include_directories(
	"${THIRD_PARTY}"
	"${NET_INC}"
)

file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h" "*.rc" "*.def")

add_library(clientlib STATIC ${SOURCES} ${HEADERS})
set_target_properties(clientlib PROPERTIES FOLDER "poker")
target_link_libraries(clientlib
	common
	pokercmn
	evaluator
	protocols
	libprotobuf
	net
)
