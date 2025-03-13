// glslc.exe triangle.frag -o triangle.frag.spv

#version 450

#include "MaterialUniformBlock.glsl"


layout(location = 0) out vec4 outColor;

void main() {
	outColor = vec4(ubo.color, 1.0);
}
