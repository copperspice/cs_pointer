add_library(CsPointer INTERFACE)
add_library(CsPointer::CsPointer ALIAS CsPointer)

target_compile_features(CsPointer
   INTERFACE
   cxx_std_17
)

target_include_directories(CsPointer
   INTERFACE
   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
   $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set(CS_POINTER_INCLUDES
   ${CMAKE_CURRENT_SOURCE_DIR}/src/cs_enable_shared.h
   ${CMAKE_CURRENT_SOURCE_DIR}/src/cs_intrusive_pointer.h
   ${CMAKE_CURRENT_SOURCE_DIR}/src/cs_shared_pointer.h
   ${CMAKE_CURRENT_SOURCE_DIR}/src/cs_shared_array_pointer.h
   ${CMAKE_CURRENT_SOURCE_DIR}/src/cs_unique_pointer.h
   ${CMAKE_CURRENT_SOURCE_DIR}/src/cs_unique_array_pointer.h
   ${CMAKE_CURRENT_SOURCE_DIR}/src/cs_weak_pointer.h
)

install(
   TARGETS CsPointer
   EXPORT CsPointerLibraryTargets ${INSTALL_TARGETS_DEFAULT_ARGS}
   RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
   LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
   ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(
   FILES ${CS_POINTER_INCLUDES}
   DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
   COMPONENT CsPointer
)

install(
   EXPORT CsPointerLibraryTargets
   FILE CsPointerLibraryTargets.cmake
   DESTINATION ${PKG_PREFIX}
)
