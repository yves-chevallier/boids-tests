#version 150

layout (points) in;
layout (triangle_strip, max_vertices = 3) out;

in float angle[];

out vec4 centerPosition;
out vec4 finalColour;
out float radius;

uniform float boidSize;

uniform mat4 projectionViewMatrix;

void createVertex(vec3 offset) {
    vec4 worldPosition = gl_in[0].gl_Position;
    worldPosition.xy += offset.xy;
    gl_Position = worldPosition;
    EmitVertex();
}

void createAngularVertex(float angle, float radius) {
    vec4 worldPosition = gl_in[0].gl_Position;
    worldPosition.xy += vec2(cos(angle), sin(angle)) * radius;
    gl_Position = worldPosition;
    EmitVertex();
}

void triangle(float angle, float radius) {
    float pi = 3.1415;
    float angleOffset = 0.5;
    createAngularVertex(angle, radius);
    createAngularVertex(angle + pi + angleOffset, radius);
    createAngularVertex(angle + pi - angleOffset, radius);
    EndPrimitive();
}

void main(void) {
    triangle(angle[0], boidSize);
}
