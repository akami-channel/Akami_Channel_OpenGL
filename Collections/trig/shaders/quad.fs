#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D generalTexture;

#define PI 3.14159

// maps atan range, which is from -PI/2 to PI/2 to the space between 0.0 and 1.0
float map_atan_range_to_0_to_1 (float x){
    return ( x * 2 / PI + 1.0 ) / 2.0;
}

vec3 color_code_pos_only1 (float x){
    return vec3(x / 10.0, x, x * 10.0);
}

vec3 color_code_pos_and_neg1 (float x){
    return vec3( (x + 10.0) / 20.0, (x + 1.0 / 2.0), (x + 0.1 ) / 0.2 );
    // domain       -10 to 10    -1 to 1                -0.1 to 0.1
    // range        0 to 1       0  to 1                0     to  1
}

void main()
{
       
    // FragColor = vec4(0.5f, 0.0f, 0.0f, 1.0f);
    // FragColor = texture(generalTexture, TexCoord);

    // float adjusted_x = TexCoord.x * 2.0 - 1.0; // ranges from -1.0 to 1.0
    // float adjusted_y = TexCoord.y * 2.0 - 1.0; // ranges from -1.0 to 1.0

    float x = TexCoord.x * 20.0 - 10.0; // ranges from -10.0 to 10.0
    float y = TexCoord.y * 20.0 - 10.0; // ranges from -10.0 to 10.0

    FragColor = vec4(color_code_pos_and_neg1(atan(x/(2*y - x))), 1.0f);
    // FragColor = vec4(color_code_pos_and_neg1(adjusted_x), 1.0);

    // if(pow(TexCoord.x, 2.0) + pow(TexCoord.y, 2.0) <= 1.0){
    //     FragColor = vec4(TexCoord.x, 0.0f, TexCoord.y, 1.0f);
    // } else{
    //     FragColor = vec4(0.0f, 1.0f, 1.0f, 1.0f);
    // }
}







