#!/usr/bin/env gawk -f

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

BEGIN {
    scaleX = scaleY = scaleZ = 1
    moveX = moveY = moveZ = 0
    shift = 0
}
$0 ~ "^#!SCALE:([ ](-)?[0-9]*(.([0-9])*)?){1,3}$"{
    scaleX = scaleY = scaleZ = $2
    if(NF == 4) {
        scaleX = $2
        scaleY = $3
        scaleZ = $4
    }
}
$0 ~ "^#!MOVE:([ ](-)?[0-9]*(.([0-9])*)?){1,3}$" {
    moveX = moveY = moveZ = $2
    if(NF == 4) {
        moveX = $2
        moveY = $3
        moveZ = $4
    }
}
$0 ~ "^#!SHIFT: ((-)?[0-9]*(.([0-9])*)?)$" {
    shift = $2
}
END {
    root = ENVIRON["ENGINE_ROOT"]
    pConvertToTriangle = root "/tools/objConvertToTriangle.awk "
    pFaceShift = root "/tools/objFaceShift.awk "
    pMove = root "/tools/objMove.awk "
    pScaleXYZ = root "/tools/objScaleXYZ.awk "

    cmd = pConvertToTriangle FILENAME " | " pFaceShift shift " | " pMove moveX " " moveY " " moveZ " | " pScaleXYZ scaleX " " scaleY " " scaleZ
    system(cmd)
}
