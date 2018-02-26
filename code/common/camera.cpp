#include "camera.h"

inline camera
DefaultCamera()
{
    camera Result;
    Result.LowerLeft = V3(-2.0, -1.0, -1.0);
    Result.Horizontal = V3(4.0, 0.0, 0.0);
    Result.Vertical = V3(0.0, 2.0, 0.0);
    Result.Origin = V3(0.0, 0.0, 0.0);

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
