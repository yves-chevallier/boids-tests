#version 140
in vec3 vposition;
out vec4 out_Colour;

uniform sampler2D iChannel0;
uniform vec2 iResolution;
uniform vec2 iMouse;
uniform float iTime;
void main(void) {
    vec3 color = vec3(42.0/200, 120.0/200, 200.0/200.0);
    vec2 uv = (gl_FragCoord.xy - vposition.xy) / iResolution.xy * (sin(iTime*30.0)/5.0 + 6.0);
    vec3 blob = vec3(.11 - clamp(length(uv), 0, .111)) * 2.5;
    blob += vec3(.11 - clamp(length(uv + vec2(.1, .1)), 0, .111)) * 2.5;
    vec3 stack = texture(iChannel0, gl_FragCoord.xy / iResolution.xy).xyz * color;
    out_Colour = vec4(stack + blob, 1.0);
}
