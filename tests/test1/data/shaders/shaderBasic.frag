#version 330

out vec4 vFinalColor;

in vec4 vColorVarying;

void main(void) {
   vFinalColor = vColorVarying;
}
