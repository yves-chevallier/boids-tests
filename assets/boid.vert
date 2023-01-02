#version 150
in vec3 position;

out float angle;

uniform mat4 projectionViewMatrix;

void main(void){
    gl_Position = projectionViewMatrix * vec4(position.xy, 0.0 ,1.0);
    angle = position.z;
}
