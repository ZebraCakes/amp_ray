#pragma once

union ray
{
    struct
    {
        v3 A;
        v3 B;
        r32 CastTime;
    };
    
    struct
    {
        v3 Origin;
        v3 Direction;
        r32 _pad;
    };

    struct
    {
        v3 E[2];
        r32 pad_;
    };
};

enum MATERIAL_TYPE
{
    LAMBERTIAN = 0x1,
    METAL = 0x2,
    DIALECTRIC = 0x4,
};

struct material
{
    MATERIAL_TYPE Type;
    v3 Albedo;
    r32 Perturbation;
    r32 RefractionIndex;
};

enum SPHERE_STATE
{
    STATIC,
    MOVING,
};

union sphere
{
    struct
    {
        v3  Center;
        r32 Radius;
        material Material;

        SPHERE_STATE State;
        v3 StartPos;
        v3 EndPos;
        r32 StartTime;
        r32 EndTime;
    };
};

enum hitable_type
{
    SPHERE,
    PLANE,
    BVH,
};

struct hitable
{
    hitable_type Type;
    void *Data;
}

struct hit_record
{
    r32 t;
    v3 Pos;
    v3 Normal;
    material Material;
};

inline v3
PointAtParameter(ray *Ray, r32 t)
{
    v3 Result = Ray->Origin + t*Ray->Direction;

    return Result;
}

inline v3
SphereCenterAtTime(sphere *Sphere, r32 CurrentTme)
{
    v3 Result = Sphere->Center;
    
    if(Sphere->State == MOVING)
    {
        Result = Sphere->StartPos + ((CurrentTme - Sphere->StartTime) / (Sphere->EndTime - Sphere->StartTime))*(Sphere->EndPos - Sphere->StartPos);
    }

    return Result;
}

// internal b32
// RayBoxIntersection(rect3 *Rect, ray *Ray, r32 tMin, r32 tMax)
// {
//     b32 Result = true;

//     r32 tx0 = ffmin((Rect->Min.x - Ray->Origin.x) / Ray->Direction.x), (Rect->Max.x - Ray->Origin.x) / Ray->Direction.x);
//     r32 tx1 = ffmax((Rect->Min.x - Ray->Origin.x) / Ray->Direction.x), (Rect->Max.x - Ray->Origin.x) / Ray->Direction.x);

//     r32 ty0 = ffmin((Rect->Min.y - Ray->Origin.y) / Ray->Direction.y), (Rect->Max.y - Ray->Origin.y) / Ray->Direction.y);
//     r32 ty1 = ffmax((Rect->Min.y - Ray->Origin.y) / Ray->Direction.y), (Rect->Max.y - Ray->Origin.y) / Ray->Direction.y);

//     r32 tz0 = ffmin((Rect->Min.z - Ray->Origin.z) / Ray->Direction.z), (Rect->Max.z - Ray->Origin.z) / Ray->Direction.z);
//     r32 tz1 = ffmax((Rect->Min.z - Ray->Origin.z) / Ray->Direction.z), (Rect->Max.z - Ray->Origin.z) / Ray->Direction.z);

//     r32 xMin = tMin = ffmax(tx0, tMin);
//     r32 xMax = tMax = ffmin(tx1, tMax);

//     r32 yMin = tMin = ffmax(ty0, tMin);
//     r32 yMax = tMax = ffmin(ty1, tMax);

//     r32 zMin = tMin = ffmax(tz0, tMin);
//     r32 zMax = tMax = ffmin(tz1, tMax);
    
//     if (xMin > xMax || yMin > yMax || zMin > zMax)
//     {
//         Result = false;
//     }

//     return Result;
// }

inline b32
FastRayBoxIntersection(rect3 *Rect, ray *Ray, r32 tMin, r32 tMax)
{
    b32 Result = true;
    for(u32 Index = 0;
        Index < 3;
        ++Index)
    {
        r32 InverseDir = 1.0 / Ray->Direction.E[Index];
        r32 t0 = (Rect->Min.E[Index] - Ray->Origin.E[Index])*InverseDir;
        r32 t1 = (Rect->Max.E[Index] - Ray->Origin.E[Index])*InverseDir;

        if(InverseDir < 0.0)
        {
            SWAP(t0, t1, r32);
        }

        tMin = (t0 > tMin) ? t0 : tMin;
        tMax = (t1 > tMax) ? t1 : tMax;

        if(tMax <= tMin)
        {
            Result = false;
            break;
        }
    }

    return Result;
}

inline b32
AABB(sphere *Sphere, r32 t0, r32 t1, rect3 *Box)
{
    b32 Result = true;

    *Box = Rectangle3(Sphere->Center - V3(Sphere->Radius, Sphere->Radius, Sphere->Radius), Sphere->Center + V3(Sphere->Radius, Sphere->Radius, Sphere->Radius));

    return Result;
}

rect3
AABB(rect3 *Box0, rect3 *Box1)
{
    rect3 Result;

    v3 minBox = V3(fmin(Box0->Min.x, Box1->Min.x), 
                   fmin(Box0->Min.y, Box1->Min.y),
                   fmin(Box0->Min.z, Box1->Min.z));

    v3 maxBox = V3(fmax(Box0->Max.x, Box1->Max.x), 
                   fmax(Box0->Max.y, Box1->Max.y),
                   fmax(Box0->Max.z, Box1->Max.z));

    Result.Min = minBox;
    Result.Max = maxBox;

    return Result;
}

inline b32
AABB(sphere *Spheres, u32 SphereCount, r32 t0, r32 t1, rect3 *Box)
{
    b32 Result = false;
    if(SphereCount > 0)
    {
        rect3 TempBox;
        Result = AABB(Spheres, t0, t1, &TempBox);

        if(Result)
        {
            *Box = TempBox;
            for(u32 Index = 1;
                Index < SphereCount;
                ++Index)
            {
                if(AABB(Spheres + Index, t0, t1, &TempBox))
                {
                    *Box = AABB(Box, &TempBox);
                }
                else
                {
                    Result = false;
                    break;
                }
            }
        }
    }

    return Result;
}