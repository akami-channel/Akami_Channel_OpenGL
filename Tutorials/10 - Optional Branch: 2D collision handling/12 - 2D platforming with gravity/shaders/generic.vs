#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 texCoord;

out vec2 TexCoord;
uniform vec2 trans;
uniform vec2 scale;

void main()
{
   gl_Position = vec4(aPos.x * scale.x + trans.x, aPos.y * scale.y + trans.y, aPos.z, 1.0);
   TexCoord = texCoord;
}
