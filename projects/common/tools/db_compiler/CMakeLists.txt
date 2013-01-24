include_directories(
	"${THIRD_PARTY}"
	"${DB_INC}"
	"${COMMON_INC}"
)

file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h" "*.rc" "*.def")

add_executable(db_compiler ${SOURCES} ${HEADERS})
set_target_properties(db_compiler PROPERTIES FOLDER "common")
target_link_libraries(db_compiler
	common
	db
	sqlite3
)