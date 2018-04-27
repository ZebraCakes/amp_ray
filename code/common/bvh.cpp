struct bvh_node
{
    bvh_node *Left;
    bvh_node *Right;
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