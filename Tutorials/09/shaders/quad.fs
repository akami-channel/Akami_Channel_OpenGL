#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D generalTexture;
uniform vec3 color;

void main()
{
    // x ^ 2 + y ^ 2 = r ^ 2
       
    FragColor = vec4(color.r, color.g, color.b, 1.0f);
    // FragColor = texture(generalTexture, TexCoord);
    // FragColor = vec4(TexCoord.x, 0.0f, TexCoord.y, 1.0f);

    // x^2 + y^2 <= r^2

    //if(pow(TexCoord.x, 2.0) + pow(TexCoord.y, 2.0) <= 1.0){
    //    FragColor = vec4(TexCoord.x, 0.0f, TexCoord.y, 1.0f);
    //} else{
    //    FragColor = vec4(0.0f, 1.0f, 1.0f, 1.0f);
    //}
}
