#include <stdio.h>
#include <thread>

#include "..\amp_lib\amp_math.h"
#include "..\common\ray.h"
#include "..\common\camera.cpp"
#include "..\common\material.cpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "..\stb_lib\stb_image_write.h"

internal bool
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
            Record->Material = Sphere->Material;
            result = true;
        }
    }

    return result;
}

internal bool
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

internal v3
ComputePixel(ray *Ray, sphere *Spheres, u32 SphereCount, i32 Bounces)
{
    v3 Result = {};
    hit_record Record;

    if (HitSomething(Ray, Spheres, SphereCount, 0.001, 1000000, &Record))
    {
        ray ScatteredRay;
        v3 Attenuation;
        if (Bounces < 50 && Scatter(Ray, &Record, &Attenuation, &ScatteredRay))
        {
            Result = Hadamard(Attenuation, ComputePixel(&ScatteredRay, Spheres, SphereCount, Bounces + 1));
        }
        else
        {
            Result = V3(0.0, 0.0, 0.0);
        }
    }
    else
    {
        v3 UnitDirection = Normalize(Ray->Direction);
        r32 t = 0.5*(UnitDirection.y + 1.0);
        Result = (1.0 - t)*V3(1.0, 1.0, 1.0) + t*V3(0.5, 0.7, 1.0);
    }

    return Result;
}

internal void
CreateRandomScene(sphere *Spheres, u32 SphereCount)
{
    u32 NextSphere = 0;
    Spheres[NextSphere++] = { V3(0.0, -1000.0, 0.0), 1000, LambertianMaterial(&V3(0.5, 0.5, 0.5)) };
    i32 i = 1;

    for (i32 a = -11;
        a < 11;
        ++a)
    {
        for (i32 b = -11;
            b < 11;
            ++b)
        {
            r32 MaterialChoice = RandomCanonical();
            v3 Center = V3(a + 0.9*RandomCanonical(), 0.2, b + 0.9*RandomCanonical());
            if (Length(Center - V3(4.0, 0.2, 0.0)) > 0.9)
            {
                if (MaterialChoice < 0.8)
                {
                    Spheres[NextSphere].Center = Center;
                    Spheres[NextSphere].Radius = 0.2;
                    Spheres[NextSphere++].Material = LambertianMaterial(&V3(RandomCanonical(), RandomCanonical(), RandomCanonical()));
                }
                else if (MaterialChoice < 0.95)
                {
                    Spheres[NextSphere].Center = Center;
                    Spheres[NextSphere].Radius = 0.2;
                    Spheres[NextSphere++].Material = MetalMaterial(&V3(0.5*(1.0 + RandomCanonical()), 0.5*(1.0 + RandomCanonical()), 0.5*(1.0 + RandomCanonical())),
                                                                   0.5*RandomCanonical());
                }
                else
                {
                    Spheres[NextSphere].Center = Center;
                    Spheres[NextSphere].Radius = 0.2;
                    Spheres[NextSphere++].Material = DialectricMaterial(1.5);
                }
            }
        }
    }

    Spheres[NextSphere].Center = V3(0.0, 1.0, 0.0);
    Spheres[NextSphere].Radius = 1.0;
    Spheres[NextSphere++].Material = DialectricMaterial(1.5);

    Spheres[NextSphere].Center = V3(-4.0, 1.0, 0.0);
    Spheres[NextSphere].Radius = 1.0;
    Spheres[NextSphere++].Material = LambertianMaterial(&V3(0.4, 0.2, 0.1));

    Spheres[NextSphere].Center = V3(4.0, 1.0, 0.0);
    Spheres[NextSphere].Radius = 1.0;
    Spheres[NextSphere++].Material = MetalMaterial(&V3(0.7, 0.6, 0.5), 0.0);
}



void
ProcessRows(volatile u32 *FrameBuffer, int StartRow, int RowCount,
            int BufferWidth, int BufferHeight,
            int Stride, int SamplesPerPixel,
            camera *Camera, sphere *Spheres, int SphereCount)
{
    u8 *Pixel = (u8*)(FrameBuffer + StartRow*BufferWidth);

    for (i32 y = BufferHeight - StartRow - 1;
        y >= BufferHeight - (StartRow + RowCount);
        --y)
    {
        for (i32 x = 0;
            x < Stride;
            ++x)
        {
            v3 PixelColor = V3(0.0, 0.0, 0.0);

            for (int Sample = 0;
                Sample < SamplesPerPixel;
                ++Sample)
            {
                v2 uv = V2(((r32)x + RandomRealInRange(0.0, 1.0)) / (r32)BufferWidth, ((r32)y + RandomRealInRange(0.0, 1.0)) / (r32)BufferHeight);

                ray CurrentRay = RayAtPoint(Camera, &uv);

                v3 Pos = PointAtParameter(&CurrentRay, 2.0);
                PixelColor += ComputePixel(&CurrentRay, Spheres, SphereCount, 0);

            }

            PixelColor /= (r32)SamplesPerPixel;
            //NOTE(Adam):  Gamma correct output pixel
            PixelColor = V3(sqrt(PixelColor.x), sqrt(PixelColor.y), sqrt(PixelColor.z));
            v3i PixelColorInt = V3i((i32)(255.9*PixelColor.r),
                (i32)(255.9*PixelColor.g),
                (i32)(255.9*PixelColor.b));

            *Pixel++ = PixelColorInt.r;
            *Pixel++ = PixelColorInt.g;
            *Pixel++ = PixelColorInt.b;
            *Pixel++ = 0xFF;
        }
    }
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
        outputFile = fopen("chapter12_output.ppm", "w");
    }

    fprintf(outputFile, "P3\n%d %d\n255\n", Width, Height);
    SeedRand();
    
    volatile u32 *FrameBuffer = (u32*)malloc(sizeof(u32)*Width*Height);


    v3 Eye = V3(13.0, 2.0, 3.0);
    v3 LookAt = V3(0.0, 0.0, 0.0);
    r32 FocalLength = 10.0;
    r32 Aperture = 0.1;
    camera Camera = MakeCameraWithDoF(&Eye, &LookAt, &V3(0.0, 1.0, 0.0), 20.0, (r32)Width/(r32)Height, Aperture, FocalLength);

    r32 R = cos(PI / 4.0);

    sphere Spheres[488];

    CreateRandomScene(Spheres, ARRAY_COUNT(Spheres));

    u32 ThreadCount = std::thread::hardware_concurrency();
    std::thread *Threads = (std::thread*)malloc(sizeof(std::thread)*(ThreadCount - 1));

    u32 RangePerThread = Height / 4;
    std::thread thread0 = std::thread(ProcessRows, FrameBuffer, RangePerThread*0, RangePerThread, Width, Height, Width, SamplesPerPixel, &Camera, Spheres, ARRAY_COUNT(Spheres));
    std::thread thread1 = std::thread(ProcessRows, FrameBuffer, RangePerThread*1, RangePerThread, Width, Height, Width, SamplesPerPixel, &Camera, Spheres, ARRAY_COUNT(Spheres));
    std::thread thread2 = std::thread(ProcessRows, FrameBuffer, RangePerThread*2, RangePerThread, Width, Height, Width, SamplesPerPixel, &Camera, Spheres, ARRAY_COUNT(Spheres));
    std::thread thread3 = std::thread(ProcessRows, FrameBuffer, RangePerThread*3, RangePerThread, Width, Height, Width, SamplesPerPixel, &Camera, Spheres, ARRAY_COUNT(Spheres));
    
    thread0.join();
    thread1.join();
    thread2.join();
    thread3.join();

    stbi_write_png("output_chapter_12.png", Width, Height, 4, (void*)FrameBuffer, Width * 4);
    

    return;
}