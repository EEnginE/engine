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

out vec4 toFragColor;


const vec3 cSpecularMaterial = vec3( 0.9, 0.9, 0.9 );

void main() {
   gl_Position = uMVP * vec4( iVertex, 1.0 );

   vec3 lAmbientDiffuseMaterial = clamp(iVertex, 0.0, 1.0);
//    vec3 lAmbientDiffuseMaterial = vec3( 0.9, 0.4, 0.4 );


   // Ambient Light
   vec3 lAmbientLight = lAmbientDiffuseMaterial * uAmbientColor;


   // Specular Light
   vec3  lNormalsWorldSpace  = (uModel * vec4( iNormals, 1.0 )).xyz;   // Get the normals in world space
   vec3  lVertexWorldSpace   = (uModel * vec4( iVertex, 1.0 )).xyz;
   vec3  lLightDirectionVert = normalize( uLightPos - lVertexWorldSpace );
   vec3  lCameraPosition     = vec3( 0.0, 0.0, 0.0 ) - ( uView * vec4(lVertexWorldSpace, 1.0)).xyz;
   vec3  lCameraDirection    = normalize( lCameraPosition - lVertexWorldSpace );

   vec3  lReflection         = reflect( -lLightDirectionVert, lNormalsWorldSpace );
   float spec                = max( 0.0, dot( lCameraDirection, lReflection ) );

   vec3  lSpecularLight      = cSpecularMaterial * spec;


   // Diffuse Light
   vec3  lLightDirection     = normalize( uLightPos + lVertexWorldSpace ); // Get the Vectror from Light source to the normal/vertex
   lLightDirection     = normalize( vec3( 2, -2, -2 ) );
   float lDiffIntensity      = max( 0.0, dot( normalize( lNormalsWorldSpace ), lLightDirection ) );

   vec3  lDiffuseLight       = vec3(
      lAmbientDiffuseMaterial.r * ( uLightColor.r * lDiffIntensity ),
      lAmbientDiffuseMaterial.g * ( uLightColor.g * lDiffIntensity ),
      lAmbientDiffuseMaterial.b * ( uLightColor.b * lDiffIntensity )
   );


   toFragColor = vec4( lAmbientLight + lDiffuseLight + lSpecularLight, 1.0 );

   if( toFragColor.a == 1.0 ) {
      toFragColor = vec4( lDiffIntensity, lDiffIntensity, lDiffIntensity, 1 );
//       toFragColor = vec4(  normalize( lNormalsWorldSpace ), 1 );
   }

//    if( lDiffIntensity == 0 ) {
//       toFragColor.rgb = vec3( 1, 0, 0 );
//    } else {
//       toFragColor.rgb = vec3( 0, 1, 0 );
//    }

}