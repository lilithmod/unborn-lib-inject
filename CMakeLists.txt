cmake_minimum_required(VERSION 3.9)
cmake_policy(SET CMP0042 NEW)
set (CMAKE_CXX_STANDARD 11)

project(inject)

set(SOURCE
        "src/bindings/bindings.cc"
        "src/bindings/inject.hh"
        "src/bindings/impl/win32.cc")
        
add_definitions(-DNAPI_EXPERIMENTAL)

include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_JS_INC})

add_library(${PROJECT_NAME} SHARED ${SOURCE} ${CMAKE_JS_SRC})

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

execute_process(COMMAND node -p "require('node-addon-api').include"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE NODE_ADDON_API_DIR
        )
string(REGEX REPLACE "[\r\n\"]" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})

target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB})
target_include_directories(${PROJECT_NAME} PRIVATE ${NODE_ADDON_API_DIR})