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


// #version 330 core
// layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
// out vec2 TexCoords;

// uniform mat4 projection;

// void main()
// {
//     gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
//     TexCoords = vertex.zw;
// }  