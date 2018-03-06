#pragma once

struct camera
{
    v3 Origin;
    v3 LowerLeft;
    v3 Horizontal;
    v3 Vertical;
    r32 LensRadius;

    v3 u, v, w;
};
