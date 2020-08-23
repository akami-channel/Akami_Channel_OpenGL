#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D generalTexture;
uniform vec4 color;

void main()
{
       
    FragColor = vec4(color.r, color.g, color.b, color.a);
    // FragColor = texture(generalTexture, TexCoord);
    // FragColor = vec4(TexCoord.x, 0.0f, TexCoord.y, 1.0f);

}
