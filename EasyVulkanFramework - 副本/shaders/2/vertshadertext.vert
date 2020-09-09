#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (std140,set = 0, binding = 0) uniform bufferVals {//一致块
    mat4 mvp;//总变换矩阵
} myBufferVals;

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 inTexCoor;
layout (location = 0) out vec2 outTexCoor;
out gl_PerVertex { 
	vec4 gl_Position;
};
void main() {
    outTexCoor = inTexCoor;
    gl_Position = myBufferVals.mvp * vec4(pos,1.0);
}
