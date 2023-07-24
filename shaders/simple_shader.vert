#version 450

layout(location = 0) in vec2 position;
layout(location = 1) in vec3 color;

layout(push_constant) uniform Push{
	mat2 transform;
	vec2 offset;
	vec3 color;
}push;

void main(){
	gl_Position = vec4(push.transform * position + push.offset, 0.0, 1.0);
}
//GLSL  另外编译，报错正常

//使用glslc命令，编译.vert, .frag为二进制（SPIR-V）.spv格式