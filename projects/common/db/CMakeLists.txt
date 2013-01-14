include_directories(
	"${THIRD_PARTY}"
	"${COMMON_INC}"
	"${DB_INC}"
)

file(GLOB_RECURSE SOURCES "*.cpp" "*.h")

add_library(db STATIC ${SOURCES})
set_target_properties(db PROPERTIES FOLDER "common")