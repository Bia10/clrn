cmake_minimum_required(VERSION 2.8)

project(Projects)			# Название солюшна

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(COMMON_INC "${CMAKE_CURRENT_LIST_DIR}/common/common/inc/")
set(DB_INC "${CMAKE_CURRENT_LIST_DIR}/common/db/inc/")

set(TOOLSET "vc110")
set(THIRD_PARTY "d:/svn/trunk/ThirdParty/")
link_directories(
	"${THIRD_PARTY}/lib"
	"${THIRD_PARTY}/lib/${CMAKE_CFG_INTDIR}_${TOOLSET}"
	"${LIBRARY_OUTPUT_PATH}"
)

add_subdirectory(common)
if (POKER_BOT)
	add_subdirectory(poker)
endif()