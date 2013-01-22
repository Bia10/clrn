include_directories(
	"${THIRD_PARTY}"
	"${COMMON_INC}"
	"${NET_INC}"
)

file(GLOB_RECURSE SOURCES "*.cpp" "*.h")

add_library(net STATIC ${SOURCES})
set_target_properties(net PROPERTIES FOLDER "common")