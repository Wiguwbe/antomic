# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#.rst:
# FindASDL
# ---------
#
# Find ``asdl`` executable and provide a macro to generate custom build rules.
#
# The module defines the following variables:
#
# ``ASDL_EXECUTABLE``
#   path to the ``asdl`` program
#
# ``ASDL_FOUND``
#   true if the program was found
#
# If ``asdl`` is found, the module defines the macro:
#
#   ASDL_TARGET(<Name> <ASDLInput> <HeaderOutputDir> <CodeOutputDir>)
#
# which will create a custom rule to generate the data structures.
#
# The macro defines the following variables:
#
# ``ASDL_<Name>_DEFINED``
#   true if the macro ran successfully
#
# ``ASDL_<Name>_INPUT``
#   The input source file, an alias for <ASDLInput>
#
# ``ASDL_<Name>_OUTPUTS``
#   All files generated by ASDL: the source and the header
#
# Example usage:
#
# .. code-block:: cmake
#
#   find_package(ASDL)
#   ASDL_TARGET(MySyntax syntax.asdl
#               ${CMAKE_CURRENT_BINARY_DIR}/syntax.h
#               ${CMAKE_CURRENT_BINARY_DIR}/syntax.cpp)
#   add_executable(Foo main.cpp ${ASDL_MySyntax_OUTPUTS})

find_program(ASDL_EXECUTABLE asdl_cpp.py
             PATHS ${CMAKE_CURRENT_SOURCE_DIR}/tools/asdl
             DOC "path to the asdl executable")
mark_as_advanced(ASDL_EXECUTABLE)

if(ASDL_EXECUTABLE)
  #============================================================
  # ASDL_TARGET (public macro)
  #============================================================
  #
  macro(ASDL_TARGET Name ASDLInput ASDLOutputHeader ASDLOutputSource)
    set(ASDL_TARGET_outputs "${ASDLOutputHeader};${ASDLOutputSource}")

    message(STATUS "ASDL on CMake has ${ASDLOutputHeader} header and ${ASDLOutputSource} source")

    add_custom_command(OUTPUT ${ASDL_TARGET_outputs}
      COMMAND python ${ASDL_EXECUTABLE} -h ${ASDLOutputHeader} -c ${ASDLOutputSource} ${ASDLInput}
      VERBATIM
      DEPENDS ${ASDLInput}
      COMMENT "[ASDL][${Name}] Building syntax structures with ASDL"
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

    # define target variables
    set(ASDL_${Name}_DEFINED TRUE)
    set(ASDL_${Name}_INPUT ${ASDLInput})
    set(ASDL_${Name}_OUTPUTS ${ASDL_TARGET_outputs})

  endmacro()
  #
  #============================================================

endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ASDL REQUIRED_VARS ASDL_EXECUTABLE)
