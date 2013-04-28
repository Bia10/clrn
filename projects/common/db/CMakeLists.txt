include_directories(
	"${THIRD_PARTY}"
	"${COMMON_INC}"
	"${DB_INC}"
)

add_subdirectory(tests)

file(GLOB_RECURSE SOURCES "src/*.cpp" "inc/*.h")

add_library(db STATIC ${SOURCES})
set_target_properties(db PROPERTIES FOLDER "common")

target_link_libraries(db
	sqlite3
	ws2_32
	dbghelp
)