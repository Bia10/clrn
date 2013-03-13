include_directories(
	"${THIRD_PARTY}"
	"${NET_INC}"
	"${DB_INC}"
)

file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h" "*.rc" "*.def")

add_library(serverlib STATIC ${SOURCES} ${HEADERS})
set_target_properties(serverlib PROPERTIES FOLDER "poker")
target_link_libraries(serverlib
	common
	net
	db
	pokercmn
	evaluator 
	protocols
	libprotobuf
	neuro
)
