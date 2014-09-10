#version 330

layout (location = 0) in vec4 position;

// Just for testing the rLinker

struct someStuff {
  vec4 someVec;
  vec4 someMat;
};

uniform someOtherStuff {
  uniform vec4 someOtherVec;
  mat4 someOtherMat;
};

uniform mat4 aUniformValue;

uniform someStuff fancyStuff;
uniform someStuff notSoFancyStuff;

void main()
{
   vec4 someSpecialStuff = someOtherVec * someOtherMat;
   vec4 etcStuff = notSoFancyStuff.someMat * notSoFancyStuff.someVec;
   vec4 anOtherVec = fancyStuff.someMat * fancyStuff.someVec;
	gl_Position = aUniformValue * position * etcStuff * someSpecialStuff * anOtherVec;
}
