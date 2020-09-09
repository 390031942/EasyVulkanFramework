#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (push_constant) uniform constantVals {//推送常量块
	mat4 mvp; //最终变换矩阵
} myConstantVals;

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 inTexCoor;
layout (location = 0) out vec2 outTexCoor;
out gl_PerVertex { 
	vec4 gl_Position;
};
void main() {
    outTexCoor = inTexCoor;
    gl_Position = myConstantVals.mvp* vec4(pos,1.0);
}
