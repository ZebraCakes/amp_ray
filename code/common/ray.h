#pragma once

union ray
{
    struct
    {
        v3 A;
        v3 B;
    };
    
    struct
    {
        v3 Origin;
        v3 Direction;
    };

    v3 E[2];
};

enum MATERIAL_TYPE
{
    LAMBERTIAN = 0x1,
    METAL = 0x2,
};
struct material
{
    MATERIAL_TYPE Type;
    v3 Albedo;
    r32 Perturbation;
};

union sphere
{
    struct
    {
        v3  Center;
        r32 Radius;
        material Material;
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
