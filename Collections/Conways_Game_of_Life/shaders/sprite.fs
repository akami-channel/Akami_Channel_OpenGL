#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D generalTexture;
uniform vec4 color;

void main()
{
       
    // FragColor = vec4(color.r, color.g, color.b, color.a);
    // FragColor = vec4(0.0f, 1.0f, 1.0f, 1.0f);
    FragColor = texture(generalTexture, vec2(TexCoord.x / 4.0, (TexCoord.y)/ 3.0 + 0.3333 ));
    // FragColor = vec4(TexCoord.x, 0.0f, TexCoord.y, 1.0f);


}
