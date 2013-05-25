include_directories(
	"${THIRD_PARTY}"
	"${COMMON_INC}"
)

file(GLOB_RECURSE SOURCES "*.cc" "*.h")

add_library(protocols STATIC ${SOURCES})
set_target_properties(protocols PROPERTIES FOLDER "poker/common")