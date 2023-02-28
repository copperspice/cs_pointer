include(Load_Catch2)
include(CTest)

add_executable(CsPointerTest "")
set_target_properties(CsPointerTest
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/test"
)

file(WRITE ${CMAKE_BINARY_DIR}/bin/test/run_test.bat
   "@SET PATH=..;%PATH%\n@CsPointerTest\n"
)

target_link_libraries(CsPointerTest
   PUBLIC
   CsPointer
   Catch2::Catch2
)

include_directories(
   ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(CsPointerTest
   PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/cs_catch2.h

   ${CMAKE_CURRENT_SOURCE_DIR}/test_main.cpp
)

include(ParseAndAddCatchTests)
ParseAndAddCatchTests(CsPointerTest)
