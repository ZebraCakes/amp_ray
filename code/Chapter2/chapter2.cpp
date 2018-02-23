#include <stdio.h>

#include "..\amp_lib\amp_math.h"

void main(int ArgumentCount, char **Arguments)
{
    int Width = 200;
    int Height = 100;

    FILE *outputFile;
    if (ArgumentCount > 1)
    {
        outputFile = fopen(Arguments[1], "w");
    }
    else
    {
        outputFile = fopen("chapter2_output.ppm", "w");
    }

    fprintf(outputFile, "P3\n%d %d\n255\n", Width, Height);
    
    for (i32 y = Height - 1;
        y >= 0;
        --y)
    {
        for (i32 x = 0;
            x < Width;
            ++x)
        {
            v3 PixelColor = V3((r32)x/(r32)Width, (r32)y/(r32)Height, 0.2);

            v3i PixelColorInt = V3i((i32)(255.9*PixelColor.r),
                                    (i32)(255.9*PixelColor.g),
                                    (i32)(255.9*PixelColor.b));

            fprintf(outputFile, "%d %d %d\n", PixelColorInt.r, PixelColorInt.g, PixelColorInt.b);
        }
    }
}