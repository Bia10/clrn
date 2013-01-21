set(POKER_COMMON_INC "${CMAKE_CURRENT_LIST_DIR}/common/inc/")
include_directories("${POKER_COMMON_INC}")

add_subdirectory(common)
add_subdirectory(injector)
add_subdirectory(client)
add_subdirectory(evaluator)
add_subdirectory(tests)
add_subdirectory(protocols)