#include <stdio.h>

#include "..\amp_lib\amp_math.h"
#include "..\amp_lib\amp_ray.h"

inline bool
HitSphere(v3 *Center, r32 Radius, ray *Ray)
{
    bool result = false;
    v3 CenterToOrigin = Ray->Origin - *Center;

    r32 a = LengthSquared(Ray->Direction);
    r32 b = 2.0 * Inner(CenterToOrigin, Ray->Direction);
    r32 c = LengthSquared(CenterToOrigin) - Radius*Radius;
    r32 discriminant = b*b - 4*a*c;

    result =  discriminant > 0.0;

    return result;
}

inline v3
ComputePixel(ray *Ray)
{
    v3 Result = {};

    if (HitSphere(&V3(0.0, 0.0, -1.0), 0.5, Ray))
    {
        Result.r = 1.0;
    }
    else
    {
        v3 UnitDirection = Normalize(Ray->Direction);
        r32 t = 0.5*(UnitDirection.y + 1.0);
        Result = (1.0 - t)*V3(1.0, 1.0, 1.0) + t*V3(0.5, 0.7, 1.0);
    }

    return Result;
}

void
main(int ArgumentCount, char **Arguments)
{
    int Width = 2000;
    int Height = 1000;

    FILE *outputFile;
    if (ArgumentCount > 1)
    {
        outputFile = fopen(Arguments[1], "w");
    }
    else
    {
        outputFile = fopen("chapter3_output.ppm", "w");
    }

    fprintf(outputFile, "P3\n%d %d\n255\n", Width, Height);
    
    v3 LowerLeftCorner = V3(-2.0, -1.0, -1.0);
    v3 Horizontal = V3(4.0, 0.0, 0.0);
    v3 Vertical = V3(0.0, 2.0, 0.0);
    v3 Origin = {};

    for (i32 y = Height - 1;
        y >= 0;
        --y)
    {
        for (i32 x = 0;
            x < Width;
            ++x)
        {
            v2 uv = V2((r32)x / (r32)Width, (r32)y/(r32)Height);
            ray CurrentRay = { Origin, LowerLeftCorner + uv.u*Horizontal + uv.v*Vertical };
            v3 PixelColor = ComputePixel(&CurrentRay);

            v3i PixelColorInt = V3i((i32)(255.9*PixelColor.r),
                                    (i32)(255.9*PixelColor.g),
                                    (i32)(255.9*PixelColor.b));

            fprintf(outputFile, "%d %d %d\n", PixelColorInt.r, PixelColorInt.g, PixelColorInt.b);
        }
    }
}