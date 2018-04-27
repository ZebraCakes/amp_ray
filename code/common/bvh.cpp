struct bvh_node
{
    void *Left;
    void *Right;

    hitable_type LeftType;
    hitable_type RightType;
    rect3 AABB;
};

internal b32
HitNode(bvh_node *Node, ray *Ray, r32 tMin, r32 tMax, hit_record *Record)
{
    b32 Result = false;
    if(FastRayBoxIntersection(&Node->AABB, Ray, tMin, tMax))
    {
        hit_record LeftRecord, RightRecord;
        b32 HitLeft = HitNode(Node->Left, Ray, tMin, tMax, &LeftRecord);
        b32 HitRight = HitNode(Node->Right, Ray, tMin, tMax, &RightRecord);

        if(HitLeft && HitRight)
        {
            if(LeftRecord.t < RightRecord.t)
            {
                *Record = LeftRecord;
            }
            else
            {
                *Record = RightRecord;
            }
            Result = true;
        }
        else if(HitLeft)
        {
            Result = true;
            *Record = LeftRecord;
        }
        else if(HitRight)
        {
            Result = true;
            *Record = RightRecord;
        }
    }

    return Result;
}

internal i32
CompareXAxis()

//TODO(Adam):  Accept more than just spheres.  How to do this in C?
internal bvh_node
ConstructBVH(hitable *root, i32 Count, r32 StartTime, r32 EndTime)
{
    bvh_node Result;
    int SplitAxis = i32(3*RandomCanonical());
    switch(SplitAxis)
    {
        case 0:
        {
            //TODO(Adam):  roll our own QuickSort for "fun"
            qsort(&root, Count, sizeof(sphere), CompareXAxis);
        }break;
        case 1:
        {
            qsort(&root, Count, sizeof(sphere), CompareYAxis);
        }break;
        default:
        {
            qsort(&root, Count, sizeof(sphere), CompareZAxis);
        }break;
    }

    if(Count == 1)
    {
        Result.Left = Result.Right = root;
    }
    else if(Count == 2)
    {
        Result.Left = root;
        Result.Right = root + 1;
    }
    else
    {
        Result.Left = ()
    }
}

