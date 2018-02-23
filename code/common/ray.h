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

union sphere
{
    struct
    {
        v3  Center;
        r32 Radius;
    };
};

struct hit_record
{
    r32 t;
    v3 Pos;
    v3 Normal;
};

inline v3
PointAtParameter(ray *Ray, r32 t)
{
    v3 Result = Ray->Origin + t*Ray->Direction;

    return Result;
}
