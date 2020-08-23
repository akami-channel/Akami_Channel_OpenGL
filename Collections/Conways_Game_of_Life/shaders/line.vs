#version 330 core

layout (location = 0) in vec3 pos;

uniform vec2 line_start_point;
uniform vec2 resolution;
uniform vec2 scale;

void main()
{
	// gl_Position = vec4(2.0 * (pos.x + (line_start_point.x / resolution.x)) - 1.0, 0.0, pos.z, 1.0);
	// gl_Position = vec4(2.0 * (pos.x + (line_start_point.x / resolution.x)) - 1.0, -(pos.y + (line_start_point.y / resolution.y)), pos.z, 1.0);
   // gl_Position = vec4(2.0 * (pos.x + (line_start_point.x / resolution.x)) - 1.0, -(2.0 * (pos.y + (line_start_point.y / resolution.y)) - 1.0), pos.z, 1.0);
   gl_Position = vec4(2.0 * (scale.x * pos.x + (line_start_point.x / resolution.x)) - 1.0, -(2.0 * (scale.y * pos.y + (line_start_point.y / resolution.y)) - 1.0), pos.z, 1.0);
}
