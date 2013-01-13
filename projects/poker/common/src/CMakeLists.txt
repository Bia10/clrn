include_directories(
	"${THIRD_PARTY}"
	"${COMMON_INC}"
)

file(GLOB HEADERS "${POKER_COMMON_INC}/*.h")
file(GLOB SOURCES "*.cpp")

add_library(pokercmn STATIC ${SOURCES} ${HEADERS})
set_target_properties(pokercmn PROPERTIES FOLDER "poker/common")