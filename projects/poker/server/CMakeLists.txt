include_directories(
	"${NET_INC}"
	"${DB_INC}"
)

file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h" "*.rc" "*.def")

add_executable(server ${SOURCES} ${HEADERS})
set_target_properties(server PROPERTIES FOLDER "poker") 

target_link_libraries(server
	common
	net
	db
	pokercmn
	evaluator 
	protocols
	libprotobuf
)
