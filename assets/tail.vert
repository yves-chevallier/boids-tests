#version 150
in vec3 position;
out vec3 vPosition;
uniform mat4 projectionViewMatrix;

void main(void){
    vPosition = position;
    gl_Position = projectionViewMatrix * vec4(position.xy, 0.0 ,1.0);
}
