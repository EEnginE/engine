precision highp float;
uniform vec4 u_diffuse;
void main(void) {
vec4 color = vec4(0., 0., 0., 0.);
vec4 diffuse = vec4(0., 0., 0., 1.);
diffuse = u_diffuse;
color.xyz += diffuse.xyz;
color = vec4(color.rgb * diffuse.a, diffuse.a);
gl_FragColor = color;
}
