#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 texCoord;

out vec2 TexCoord;
uniform float transX;

void main()
{
   // macbook screen is 16:10
   gl_Position = vec4( aPos.x * 1.0, aPos.y * 1.6 , aPos.z, 1.0);
   TexCoord = texCoord;
}
