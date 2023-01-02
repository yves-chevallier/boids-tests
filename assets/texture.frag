#version 150
in vec4 finalColour;
in vec4 centerPosition;
out vec4 out_Colour;

uniform float screenWidth;
uniform float radius;
uniform vec2 iResolution;
uniform sampler2D iChannel0;
void main(void){
    vec2 resolution = iResolution / 2.0;
    vec2 uv = gl_FragCoord.xy / resolution.xy;
    vec4 tex = texture(iChannel0, uv) *0.8;
    out_Colour = vec4(tex.rgb, 1.0);
}