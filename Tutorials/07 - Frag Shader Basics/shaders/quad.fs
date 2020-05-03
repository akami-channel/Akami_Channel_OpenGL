#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D generalTexture;

void main()
{
   // x ^ 2 + y ^ 2 = r ^ 2
       
   // FragColor = vec4(0.5f, 0.0f, 0.0f, 1.0f);
   FragColor = texture(generalTexture, TexCoord);
}
