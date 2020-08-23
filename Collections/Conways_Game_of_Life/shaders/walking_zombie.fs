#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D generalTexture;
uniform float time;

void main()
{
       
    // FragColor = vec4(color.r, color.g, color.b, color.a);
    // FragColor = vec4(0.0f, 1.0f, 1.0f, 1.0f);
    if (sin(time * 6.0) > 0.0){
    	FragColor = texture(generalTexture, vec2(TexCoord.x / 3.0, (TexCoord.y)/ 4.0 + 2.0 / 4.0));
	}else{
		FragColor = texture(generalTexture, vec2(TexCoord.x / 3.0 + 1.0 / 3.0, (TexCoord.y)/ 4.0 + 2.0 / 4.0));
	}
    
    // FragColor = vec4(TexCoord.x, 0.0f, TexCoord.y, 1.0f);


}
