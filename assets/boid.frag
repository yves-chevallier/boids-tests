#version 150

in vec4 finalColour;

out vec4 out_Colour;
uniform vec3 boidColor;

void main(void){
    out_Colour = vec4(boidColor, 1.0);
    return;
}