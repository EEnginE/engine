#!/usr/bin/gawk -f
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
