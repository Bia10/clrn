include_directories(
	"${THIRD_PARTY}"
	"${COMMON_INC}"
	"${DB_INC}"
)

file(GLOB HEADERS "${DB_INC}/*.h")
file(GLOB SOURCES "*.cpp")

add_library(db STATIC ${SOURCES} ${HEADERS})
set_target_properties(db PROPERTIES FOLDER "common")