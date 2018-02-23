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
        outputFile = fopen("chapter1_output.ppm", "w");
    }

    fprintf(outputFile, "P3\n%d %d\n255\n", Width, Height);
    
    for (i32 y = Height - 1;
        y >= 0;
        --y)
    {
        for (int x = 0;
            x < Width;
            ++x)
        {
            r32 Red = (r32)x / (r32)Width;
            r32 Green = (r32)y / (r32)Height;
            r32 Blue = 0.2;

            int RedInt =    (i32)(255.9*Red);
            int GreenInt =  (i32)(255.9*Green);
            int BlueInt =   (i32)(255.9*Blue);

            fprintf(outputFile, "%d %d %d\n", RedInt, GreenInt, BlueInt);
        }
    }
}