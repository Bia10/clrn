include_directories(
	"${THIRD_PARTY}"
)

file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h" "*.rc" "*.def")

add_library(client SHARED ${SOURCES} ${HEADERS})
set_target_properties(client PROPERTIES FOLDER "poker")
target_link_libraries(client
	common
	pokercmn
	evaluator
)
