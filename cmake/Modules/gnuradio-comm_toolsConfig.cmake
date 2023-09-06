find_package(PkgConfig)

PKG_CHECK_MODULES(PC_GR_COMM_TOOLS gnuradio-comm_tools)

FIND_PATH(
    GR_COMM_TOOLS_INCLUDE_DIRS
    NAMES gnuradio/comm_tools/api.h
    HINTS $ENV{COMM_TOOLS_DIR}/include
        ${PC_COMM_TOOLS_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    GR_COMM_TOOLS_LIBRARIES
    NAMES gnuradio-comm_tools
    HINTS $ENV{COMM_TOOLS_DIR}/lib
        ${PC_COMM_TOOLS_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/gnuradio-comm_toolsTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GR_COMM_TOOLS DEFAULT_MSG GR_COMM_TOOLS_LIBRARIES GR_COMM_TOOLS_INCLUDE_DIRS)
MARK_AS_ADVANCED(GR_COMM_TOOLS_LIBRARIES GR_COMM_TOOLS_INCLUDE_DIRS)
