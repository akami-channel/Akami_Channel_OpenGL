

#include <stdio.h>

typedef struct Color {
    float r, g, b, a;
} Color;

typedef struct Rectangle {
    float x, y;
    float width;
    float height;
    Color color;
} Rectangle;


int main(){


    Rectangle r1, r2, r3, r4;
    r1.x = -1.0; r1.y = -1.0; r1.width = 0.1; r1.height = 0.1; r1.color.r = 0.7; r1.color.g = 0.2; r1.color.b = 1.0; r1.color.a = 0.9;
    r2.x = 0.0; r2.y = 0.0; r2.width = 0.1; r2.height = 0.1; r2.color.r = 0.3; r2.color.g = 0.2; r2.color.b = 1.0; r2.color.a = 0.9;
    r3.x = -0.4; r3.y = -0.4; r3.width = 0.1; r3.height = 0.1; r3.color.r = 0.3; r3.color.g = 0.2; r3.color.b = 0.8; r3.color.a = 0.9;
    r4.x = -0.1; r4.y = 0.6; r4.width = 0.1; r4.height = 0.1; r4.color.r = 0.3; r4.color.g = 0.2; r4.color.b = 0.2; r4.color.a = 0.9;

    int numBlocks = 4;
    Rectangle* blocks[4] = {&r1, &r2, &r3, &r4};
    printf("%s\n", "hi");
    printf("%d", sizeof(blocks));
    return 0;




}














