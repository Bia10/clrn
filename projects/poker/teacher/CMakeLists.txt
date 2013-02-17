include_directories(
	"${THIRD_PARTY}/wxWidgets/include/"
	"${THIRD_PARTY}/wxWidgets/include/msvc/"
)

file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h" "*.rc" "*.def")

add_definitions(
	-D_UNICODE
)

link_directories(
	"${THIRD_PARTY}/wxWidgets/lib/vc_lib/"
)

add_executable(teacher WIN32 ${SOURCES} ${HEADERS})
set_target_properties(teacher PROPERTIES FOLDER "poker") 

target_link_libraries(teacher
	common
	pokercmn
)
