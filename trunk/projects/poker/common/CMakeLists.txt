include_directories(
	"${THIRD_PARTY}"
	"${COMMON_INC}"
)

file(GLOB_RECURSE SOURCES "*.cpp" "*.h")

add_library(pokercmn STATIC ${SOURCES})
set_target_properties(pokercmn PROPERTIES FOLDER "poker/common")
target_link_libraries(pokercmn
	#empty
)