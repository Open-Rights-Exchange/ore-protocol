add_contract(ore.system ore.system ${CMAKE_CURRENT_SOURCE_DIR}/src/ore.system.cpp)
target_include_directories(ore.system.wasm
   PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(ore.system.wasm
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
