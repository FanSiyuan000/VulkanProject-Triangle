#version 450

vec2 positions[3] = vec2[](
	vec2(0.0, -0.5),
	vec2(0.5, 0.5),
	vec2(-0.5, 0.5)
	);

void main(){
	gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
}//GLSL  另外编译，报错正常

//使用glslc命令，编译.vert, .frag为二进制（SPIR-V）.spv格式