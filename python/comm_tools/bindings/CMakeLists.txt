# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Check if there is C++ code at all
########################################################################
if(NOT comm_tools_sources)
    MESSAGE(STATUS "No C++ sources... skipping python bindings")
    return()
endif(NOT comm_tools_sources)

########################################################################
# Check for pygccxml
########################################################################
GR_PYTHON_CHECK_MODULE_RAW(
    "pygccxml"
    "import pygccxml"
    PYGCCXML_FOUND
    )

include(GrPybind)

########################################################################
# Python Bindings
########################################################################

list(APPEND comm_tools_python_files
    bpsk_phase_ambiguity_solver_python.cc
    qpsk_phase_ambiguity_solver_python.cc
    stream_to_iq_streams_python.cc
    pdu_phase_ambiguity_solver_python.cc python_bindings.cc)

GR_PYBIND_MAKE_OOT(comm_tools
   ../../..
   gr::comm_tools
   "${comm_tools_python_files}")

# copy bindings extension for use in QA test module
add_custom_command(TARGET comm_tools_python POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:comm_tools_python>
    ${CMAKE_BINARY_DIR}/test_modules/gnuradio/comm_tools/
)

install(TARGETS comm_tools_python DESTINATION ${GR_PYTHON_DIR}/gnuradio/comm_tools COMPONENT pythonapi)
