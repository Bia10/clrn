include_directories(
	"${THIRD_PARTY}"
	"${DB_INC}"
)

file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h" "*.rc" "*.def")

add_library(neuro STATIC ${SOURCES} ${HEADERS})
set_target_properties(neuro PROPERTIES FOLDER "poker")
target_link_libraries(neuro
	common
	pokercmn
	fannfloat
	db
)
