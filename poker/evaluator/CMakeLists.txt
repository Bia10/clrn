file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h" "*.rc" "*.def")

add_library(evaluator STATIC ${SOURCES} ${HEADERS})
set_target_properties(evaluator PROPERTIES FOLDER "poker")