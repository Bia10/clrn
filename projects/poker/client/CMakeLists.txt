include_directories(
	"${THIRD_PARTY}"
)

file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h" "*.rc" "*.def")

add_library(client SHARED ${SOURCES} ${HEADERS})
