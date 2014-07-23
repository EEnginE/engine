#version 330

layout (location = 0) in vec4 position;

// Just for testing the rLinker

uniform someStuff {
  uniform vec4 someVec;
  mat4 someMat;
};

uniform someOtherStuff {
  uniform vec4 someOtherVec;
  mat4 someOtherMat;
};

uniform mat4 aUniformValue;

void main()
{
   vec4 someSpecialStuff = someOtherVec * someOtherMat;
   vec4 etcStuff = someMat * someVec;
	gl_Position = aUniformValue * position * etcStuff * someSpecialStuff;
}
