#version 150

layout (points) in;
layout (triangle_strip, max_vertices = 10) out;

in float angle[];
out vec4 centerPosition;
out vec4 finalColour;

uniform mat4 projectionViewMatrix;
uniform vec3 radiusColor;
uniform float radius;

void createVertex(vec3 offset) {
    vec4 worldPosition = gl_in[0].gl_Position;
    worldPosition.xy += offset.xy;
    gl_Position = worldPosition;
    EmitVertex();
}

void square(float size, float z) {
    createVertex(vec3(-size, -size, z));
    createVertex(vec3(size, -size, z));
    createVertex(vec3(-size, size, z));
    createVertex(vec3(size, size, z));
    EndPrimitive();
}

void main(void) {
    vec3 colour = vec3(radiusColor);
    finalColour = vec4(colour, 1.0);
    centerPosition = gl_in[0].gl_Position;
    square(radius, 0.0);
}
