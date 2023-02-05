#version 450

layout(location = 0) in vec3 worldNormal;

layout(set=2, binding=0)uniform MaterialUniform{
    vec4 Color;
}materialUniform;


layout(location = 0) out vec4 outNormal;
layout(location = 1) out vec4 outAlbedo;

void main(){
    outNormal = vec4(worldNormal, 1.0);
    outAlbedo = materialUniform.Color;
}