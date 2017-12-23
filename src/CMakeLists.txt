file(GLOB SOURCE_FILES
        "sail/*.h"
        "sail/*.cpp"
        "sail/*.c"
        "sail/vw/*.h"
        "sail/vw/*.cpp"
        )

add_library(sail SHARED ${SOURCE_FILES} sail/sail_rei.h sail/sail_rei.cpp)

target_link_libraries(sail vw boost_program_options z allreduce rmutil)
add_dependencies(sail vowpal_wabbit redis redis_modules_sdk)

