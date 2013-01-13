include_directories(
	"${THIRD_PARTY}"
	"${COMMON_INC}"
)

file(GLOB HEADERS "${COMMON_INC}/*.h")
file(GLOB SOURCES "*.cpp")

add_library(common STATIC ${SOURCES} ${HEADERS})
set_target_properties(common PROPERTIES FOLDER "common")