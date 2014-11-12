#!/usr/bin/env bash

# JavaScriptCoreCPP
# Author: Matthew D. Langston
#
# Copyright (c) 2014 by Appcelerator, Inc. All Rights Reserved.
# Licensed under the terms of the Apache Public License.
# Please see the LICENSE included with this distribution for details.

if ! test -d "${GTEST_ROOT}"; then
    echo "GTEST_ROOT must point to your Google Test installation."
    exit 1
fi

declare -rx VERBOSE=1

declare -r SLN_FILE_NAME="JavaScriptCoreCPP.sln"
declare -r MSBUILD_PATH="c:/Program Files (x86)/MSBuild/12.0/Bin/MSBuild.exe"
declare -r BUILD_DIR="build"

function echo_and_eval {
    local -r cmd="${1:?}"
    echo "${cmd}" && eval "${cmd}"
}

cmd+="cmake"
#cmd+=" -DGTEST_MSVC_SEARCH=MT"
#cmd+=" --debug-output"
cmd+=" ../"

#echo_and_eval "rm -rf \"${BUILD_DIR}\""
echo_and_eval "mkdir -p \"${BUILD_DIR}\""
echo_and_eval "pushd \"${BUILD_DIR}\""
echo_and_eval "${cmd}"
echo_and_eval "\"${MSBUILD_PATH}\" ${SLN_FILE_NAME}"
echo_and_eval "ctest"
echo_and_eval "popd"
