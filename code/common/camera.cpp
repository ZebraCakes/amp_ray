#include "camera.h"

internal v3
RandomUnitInDisk()
{
    v3 Result;

    do
    {
        Result = 2.0*V3(RandomRealInRange(0.0, 1.0), RandomRealInRange(0.0, 1.0), 0.0) - V3(1.0, 1.0, 0.0);
    } while (Dot(Result, Result) >= 1.0);

    return Result;
}

internal camera
DefaultCamera()
{
    camera Result;
    Result.LowerLeft = V3(-2.0, -1.0, -1.0);
    Result.Horizontal = V3(4.0, 0.0, 0.0);
    Result.Vertical = V3(0.0, 2.0, 0.0);
    Result.Origin = V3(0.0, 0.0, 0.0);

    return Result;
}

internal camera
DefaultCameraWithFoV(r32 fovY, r32 Aspect)
{
    camera Result = {};

    r32 Theta = ToRadians(fovY);
    r32 HalfHeight = tan(Theta / 2.0);
    r32 HalfWidth = Aspect*HalfHeight;

    Result.LowerLeft = V3(-HalfWidth, -HalfHeight, -1.0);
    Result.Horizontal = V3(HalfWidth*2.0, 0.0, 0.0);
    Result.Vertical = V3(0.0, HalfHeight*2.0, 0.0);
    Result.Origin = V3(0.0, 0.0, 0.0);

    return Result;
}

internal camera
MakeCamera(v3 *Eye, v3 *LookAt, v3 *Up, r32 fovY, r32 Aspect)
{
    camera Result;

    v3 u, v, w;
    r32 Theta = ToRadians(fovY);
    r32 HalfHeight = tan(Theta / 2.0);
    r32 HalfWidth = Aspect*HalfHeight;

    Result.Origin = *Eye;
    
    w = Normalize(*Eye - *LookAt);
    u = Normalize(Cross(*Up, w));
    v = Cross(w, u);

    Result.LowerLeft = V3(-HalfWidth, -HalfHeight, -1.0);
    Result.LowerLeft = Result.Origin - HalfWidth*u - HalfHeight*v - w;
    Result.Horizontal = 2.0*HalfWidth*u;
    Result.Vertical = 2.0*HalfHeight*v;

    return Result;
}

internal camera
MakeCameraWithDoF(v3 *Eye, v3 *LookAt, v3 *Up, r32 fovY, r32 Aspect, r32 Aperture, r32 FocalLength)
{
    camera Result;

    Result.LensRadius = Aperture / 2.0;
    r32 Theta = ToRadians(fovY);
    r32 HalfHeight = tan(Theta / 2.0);
    r32 HalfWidth = Aspect*HalfHeight;

    Result.Origin = *Eye;

    Result.w = Normalize(*Eye - *LookAt);
    Result.u = Normalize(Cross(*Up, Result.w));
    Result.v = Cross(Result.w, Result.u);

    Result.LowerLeft = Result.Origin - HalfWidth*FocalLength*Result.u - HalfHeight*FocalLength*Result.v - FocalLength*Result.w;
    Result.Horizontal = 2.0*HalfWidth*FocalLength*Result.u;
    Result.Vertical = 2.0*HalfHeight*FocalLength*Result.v;

    return Result;
}

inline ray
RayAtPoint(camera *Camera, v2 *st)
{
    ray Result;

    v3 Radius = Camera->LensRadius*RandomUnitInDisk();
    v3 Offset = Camera->u*Radius.x + Camera->v*Radius.y;
    Result.Origin = Camera->Origin + Offset;
    Result.Direction = Camera->LowerLeft + st->E[0]*Camera->Horizontal + st->E[1]*Camera->Vertical - Camera->Origin - Offset;

    return Result;
}
