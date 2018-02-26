#include <stdio.h>

#include "..\amp_lib\amp_math.h"
#include "..\common\ray.h"
#include "..\common\camera.cpp"

inline v3
RandomInUnitSphere()
{
    v3 Pos;
    v3 OneVec = V3(1.0, 1.0, 1.0);
    do
    {
        Pos = 2.0*V3(RandomRealInRange(0.0, 1.0),
                     RandomRealInRange(0.0, 1.0),
                     RandomRealInRange(0.0, 1.0)) - OneVec;
    } while (LengthSquared(Pos) >= 1.0);
    return Pos;
}

inline bool
HitSphere(sphere *Sphere, ray *Ray, r32 tMin, r32 tMax, hit_record *Record)
{
    r32 result = false;
    v3 CenterToOrigin = Ray->Origin - Sphere->Center;

    r32 a = Inner(Ray->Direction, Ray->Direction);
    r32 b = Inner(CenterToOrigin, Ray->Direction);
    r32 c = Inner(CenterToOrigin, CenterToOrigin) - Sphere->Radius*Sphere->Radius;
    r32 discriminant = b*b - a*c;

    if (discriminant > 0.0)
    {
        r32 discriminantRoot = sqrt(discriminant);
        r32 root = (-b - discriminantRoot) / a;
        
        if (root > tMax || root < tMin)
        {
            root = (-b + discriminantRoot) / a;
        }
        if (root < tMax && root > tMin)
        {

            Record->t = root;
            Record->Pos = PointAtParameter(Ray, Record->t);
            Record->Normal = (Record->Pos - Sphere->Center) / Sphere->Radius;
            result = true;
        }
    }

    return result;
}

inline bool
HitSomething(ray *Ray, sphere *SphereList, u32 SphereCount, r32 tMin, r32 tMax, hit_record *Record)
{
    hit_record Candidate = {};
    bool Result = false;

    r64 Closest = tMax;
    for (u32 Index = 0;
        Index < SphereCount;
        ++Index)
    {
        sphere *Sphere = SphereList + Index;
        if (HitSphere(Sphere, Ray, tMin, Closest, &Candidate))
        {
            Result = true;
            Closest = Candidate.t;
            *Record = Candidate;
        }
    }

    return Result;
}

inline v3
ComputePixel(ray *Ray, sphere *Spheres, u32 SphereCount)
{
    v3 Result = {};
    hit_record Record;

    if (HitSomething(Ray, Spheres, SphereCount, 0.001, 1000000, &Record))
    {
        v3 Target = Record.Pos + Record.Normal + RandomInUnitSphere();
        ray Reflection;
        Reflection.Origin = Record.Pos;
        Reflection.Direction = Target - Record.Pos;
        Result = 0.5*ComputePixel(&Reflection, Spheres, SphereCount);
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
    int Width = 3840;
    int Height = 2160;
    int SamplesPerPixel = 100;

    FILE *outputFile;
    if (ArgumentCount > 1)
    {
        outputFile = fopen(Arguments[1], "w");
    }
    else
    {
        outputFile = fopen("chapter7_output.ppm", "w");
    }

    fprintf(outputFile, "P3\n%d %d\n255\n", Width, Height);
    
    camera Camera = DefaultCamera();

    sphere Spheres[2];
    Spheres[0].Center = V3(0.0, 0.0, -1.0);
    Spheres[0].Radius = 0.5;

    Spheres[1].Center = V3(0.0, -100.5, -1.0);
    Spheres[1].Radius = 100;

    for (i32 y = Height - 1;
        y >= 0;
        --y)
    {
        for (i32 x = 0;
            x < Width;
            ++x)
        {
            v3 PixelColor = V3(0.0, 0.0, 0.0);

            for (int Sample = 0;
                Sample < SamplesPerPixel;
                ++Sample)
            {
                v2 uv = V2(((r32)x + RandomRealInRange(0.0, 1.0)) / (r32)Width, ((r32)y + RandomRealInRange(0.0, 1.0)) / (r32)Height);

                ray CurrentRay = RayAtPoint(&Camera, &uv);

                v3 Pos = PointAtParameter(&CurrentRay, 2.0);
                PixelColor += ComputePixel(&CurrentRay, Spheres, ARRAY_COUNT(Spheres));

            }
            
            PixelColor /= (r32)SamplesPerPixel;
            //NOTE(Adam):  Gamma correct output pixel
            PixelColor = V3(sqrt(PixelColor.x), sqrt(PixelColor.y), sqrt(PixelColor.z));
            v3i PixelColorInt = V3i((i32)(255.9*PixelColor.r),
                                    (i32)(255.9*PixelColor.g),
                                    (i32)(255.9*PixelColor.b));

            fprintf(outputFile, "%d %d %d\n", PixelColorInt.r, PixelColorInt.g, PixelColorInt.b);
        }
    }
}