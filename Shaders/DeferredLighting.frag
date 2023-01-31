/*
#version 450

layout(location = 0) in vec2 inUV;

layout(location = 0) out vec4 outColor;

// layout(set=0, binding=0) uniform sampler uSampler;
// layout(set=0, binding=1) uniform image2D uNormal;
// layout(set=0, binding=2) uniform image2D uAlbedo;
layout(set=0, binding=1) uniform sampler2D uNormal;
layout(set=0, binding=2) uniform sampler2D uAlbedo;

void main(){
    vec4 sampledNormal = texture(uNormal, inUV);
    vec4 sampledAlbedo = texture(uNormal, inUV);
    outColor = vec4(sampledNormal.xyz, 1.0);
}
*/

#version 450

layout(location = 0) in vec2 inUV;

layout(location = 0) out vec4 outColor;

layout(set=0, binding=0) uniform SceneUniform{
    vec4 SceneLightDir;
    vec4 SceneLightColor;
};
layout(input_attachment_index=0, set=0, binding=1) uniform subpassInput uNormal;
layout(input_attachment_index=1, set=0, binding=2) uniform subpassInput uAlbedo;

void main(){
    vec4 gNormal = subpassLoad(uNormal).rgba;
    vec4 gAlbedo = subpassLoad(uAlbedo).rgba;
    outColor = vec4(gNormal.xyz, 1.0);
}