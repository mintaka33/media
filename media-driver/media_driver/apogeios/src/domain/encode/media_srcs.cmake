# Copyright (c) 2017, Intel Corporation
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
# OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
# ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
# OTHER DEALINGS IN THE SOFTWARE.

set(TMP_SOURCES_
    ${CMAKE_CURRENT_LIST_DIR}/avc_brc.cpp
    ${CMAKE_CURRENT_LIST_DIR}/avc_hme.cpp
    ${CMAKE_CURRENT_LIST_DIR}/avc_mbenc.cpp
    ${CMAKE_CURRENT_LIST_DIR}/avc_pak.cpp
    ${CMAKE_CURRENT_LIST_DIR}/down_scaling.cpp
    #${CMAKE_CURRENT_LIST_DIR}/down_scaling_cm.cpp
    ${CMAKE_CURRENT_LIST_DIR}/encode_avc_pipeline.cpp
)

set(TMP_HEADERS_
    ${CMAKE_CURRENT_LIST_DIR}/avc_brc.h
    ${CMAKE_CURRENT_LIST_DIR}/avc_hme.h
    ${CMAKE_CURRENT_LIST_DIR}/avc_mbenc.h
    ${CMAKE_CURRENT_LIST_DIR}/avc_pak.h
    ${CMAKE_CURRENT_LIST_DIR}/down_scaling.h
    #${CMAKE_CURRENT_LIST_DIR}/down_scaling_cm.h
    ${CMAKE_CURRENT_LIST_DIR}/encode_avc_pipeline.h
)

set(SOURCES_
    ${SOURCES_}
    ${TMP_SOURCES_}
 )

set(HEADERS_
    ${HEADERS_}
    ${TMP_HEADERS_}
)

media_add_curr_to_include_path()