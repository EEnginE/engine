#version 330

// Matrixes

uniform mat4 uModelView;
uniform mat4 uMVP;

// Light stuff

uniform vec3 uAmbientColor;
uniform vec3 uLightColor;
uniform vec3 uLightPos;


in vec3 iVertex;
in vec3 iNormals;

out vec4 toFragColor;


const vec3 cSpecularMaterial = vec3( 0.9, 0.9, 0.9 );

void main() {
   mat4 lNormalMatrix = transpose( inverse( uModelView ) ); // NEVER DO THIS IN A SHADER!! ONLY FOR TESTING

   vec3 lAmbientDiffuseMaterial = clamp(iVertex, 0.0, 1.0);

   // Ambient Light
   vec3 lAmbientLight = lAmbientDiffuseMaterial * uAmbientColor;


   // Specular Light
   vec3  lNormalsEyeSpace  = mat3(lNormalMatrix) * iNormals;   // Get the normals in world space
   vec3  lVertexWorldSpace = (uModelView * vec4( iVertex, 1.0 )).xyz;
   vec3  lLightDirection   = normalize( uLightPos - lVertexWorldSpace );

   vec3  lReflection       = normalize( reflect( -lLightDirection, lNormalsEyeSpace) );
   float spec              = max( 0.0, dot( -normalize(lVertexWorldSpace), lReflection ) );

   vec3  lSpecularLight    = cSpecularMaterial * pow( spec, 20.0 );


   // Diffuse Light
   float lDiffIntensity      = max( 0.0, dot( lNormalsEyeSpace, lLightDirection ) );
   vec3  lDiffuseLight       = lAmbientDiffuseMaterial * uLightColor * lDiffIntensity;

   toFragColor = vec4( lAmbientLight + lDiffuseLight + lSpecularLight, 1 );
   gl_Position = uMVP * vec4( iVertex, 1.0 );
}