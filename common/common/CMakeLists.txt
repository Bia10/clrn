include_directories(
	"${THIRD_PARTY}"
	"${COMMON_INC}"
)

file(GLOB_RECURSE SOURCES "*.cpp" "*.h")

add_library(common STATIC ${SOURCES})
set_target_properties(common PROPERTIES FOLDER "common")