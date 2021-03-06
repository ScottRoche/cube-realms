#version 450

layout(location = 0) in vec2 inPositions;
layout(location = 1) in vec3 inColors;

layout(location = 0) out vec3 fragColor;

void main() {
	gl_Position = vec4(inPositions, 0.0, 1.0);
	fragColor = inColors;
}