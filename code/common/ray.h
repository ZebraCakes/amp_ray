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