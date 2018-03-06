#include "camera.h"

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

inline ray
RayAtPoint(camera *Camera, v2 *uv)
{
    ray Result;

    Result.Origin = Camera->Origin;
    Result.Direction = Camera->LowerLeft + uv->u*Camera->Horizontal + uv->v*Camera->Vertical - Camera->Origin;

    return Result;
}
