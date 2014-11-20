#!/usr/bin/gawk -f
BEGIN {
    fEnd = "_f.obj"
    scaleX = scaleY = scaleZ = 0
    moveX = moveY = moveZ = 0
    shift = 0
}
$0 ~ "^#!SCALE:([ ](-)?[0-9]*(.([0-9])*)?){1,3}"{
    scaleX = scaleY = scaleZ = $2
    if(NF == 4) {
        scaleX = $2
        scaleY = $3
        scaleZ = $4
    }
}
$0 ~ "^#!MOVE:([ ](-)?[0-9]*(.([0-9])*)?){1,3}" {
    moveX = moveY = moveZ = $2
    if(NF == 4) {
        moveX = $2
        moveY = $3
        moveZ = $4
    }
}
$0 ~ "^#!SHIFT: ((-)?[0-9]*(.([0-9])*)?)" {
    shift = $2
}
END {
    if(index(FILENAME, fEnd) != 0) { # File already processed
        exit 1
    }

    target = FILENAME
    sub(".obj", fEnd, target)
    cmd = "../../tools/objConvertToTriangle.awk " FILENAME " | " "../../tools/objFaceShift.awk " shift " | " "../../tools/objMove.awk " moveX " " moveY " " moveZ " | " "../../tools/objScaleXYZ.awk " scaleX " " scaleY " " scaleZ " > " target
    system(cmd)
}
