cmake_minimum_required(VERSION 2.8)

project(PokerBot)			# Название солюшна

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

set(TOOLSET "vc110")
set(THIRD_PARTY "d:/svn/trunk/ThirdParty/")
link_directories(
	"${THIRD_PARTY}/lib"
	"${THIRD_PARTY}/lib/${CMAKE_CFG_INTDIR}_${TOOLSET}"
	"${LIBRARY_OUTPUT_PATH}"
)

add_subdirectory(injector)
add_subdirectory(client)