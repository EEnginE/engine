#version 330 core

// Matrixes

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uMVP;

// Light stuff

uniform vec3 uAmbientColor;
uniform vec3 uLightColor;
uniform vec3 uLightPos;


in vec3 iVertex;
in vec3 iNormals;

out vec3 toFragColor;


const vec3 cSpecularMaterial = vec3( 0.9, 0.9, 0.9 );

void main() {
   gl_Position = uMVP * vec4( iVertex, 1.0 );

   vec3 lAmbientDiffuseMaterial = clamp(iVertex, 0.0, 1.0);
//    vec3 lAmbientDiffuseMaterial = vec3( 0.9, 0.4, 0.4 );


   // Ambient Light
   vec3 lAmbientLight = lAmbientDiffuseMaterial * uAmbientColor;


   // Specular Light
   vec3  lNormalsWorldSpace  = mat3(uModel) * iNormals;   // Get the normals in world space
   vec3  lVertexWorldSpace   = (uModel * vec4( iVertex, 1.0 )).xyz;
   vec3  lLightDirection     = normalize( uLightPos - lVertexWorldSpace );
   vec3  lCameraPosition     = -(uView * vec4( lVertexWorldSpace, 1)).xyz;
   vec3  lCameraDirection    = normalize( lCameraPosition - lVertexWorldSpace );

   vec3  lReflection         = reflect( -lLightDirection, normalize( -lNormalsWorldSpace ) );
   float spec                = max( 0.0, dot( lCameraDirection, lReflection ) );

   vec3  lSpecularLight      = cSpecularMaterial * pow( spec, 12.0 );


   // Diffuse Light
   float lDiffIntensity      = max( 0.0, dot( normalize( lNormalsWorldSpace ), lLightDirection ) );

   vec3  lDiffuseLight       = vec3(
      lAmbientDiffuseMaterial.r * ( uLightColor.r * lDiffIntensity ),
      lAmbientDiffuseMaterial.g * ( uLightColor.g * lDiffIntensity ),
      lAmbientDiffuseMaterial.b * ( uLightColor.b * lDiffIntensity )
   );


   toFragColor = lAmbientLight + lDiffuseLight + lSpecularLight;
}