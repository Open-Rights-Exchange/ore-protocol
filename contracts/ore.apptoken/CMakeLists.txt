add_contract(ore.apptoken ore.apptoken ${CMAKE_CURRENT_SOURCE_DIR}/src/ore.apptoken.cpp)

target_include_directories(ore.apptoken.wasm
   PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(ore.apptoken.wasm
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
