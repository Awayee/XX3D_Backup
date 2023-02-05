#version 450

// layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec2 outUV;

const float DEPTH = 0.5;

const vec2 VERTICES[6] = vec2[6](
    vec2(-1.0, -1.0), vec2(-1.0,  1.0), vec2( 1.0, -1.0),
    vec2(-1.0,  1.0), vec2( 1.0,  1.0), vec2( 1.0, -1.0)
);

void main(){
    vec2 vertex = VERTICES[gl_VertexIndex];
    gl_Position = vec4(vertex, DEPTH, 1.0);
    outUV = vertex * 0.5 + 0.5;
}