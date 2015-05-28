#!/usr/bin/env bash

# Copyright (C) 2015 EEnginE project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

doGlew() {
  case "$OPERATING_SYSTEM" in
    Linux)   doGlewLinux   ;;
    Windows) doGlewWindows ;;
    *)       error "Building GLEW for '$OPERATING_SYSTEM' is not supported" ;;
  esac
}

doGlewWindows() {
  warning "doGlewWindows is a stub!"
}

doGlewLinux() {
  cd dependencies/GLEW

  msg1 "Building GLEW"

  msg2 "Making extensions... this can take a while"
  make extensions &> /dev/null

  msg2 "Building... this can take a while"
  make &> /dev/null

  rm build/*.rc # Clean some build files

  cd ../..
}
