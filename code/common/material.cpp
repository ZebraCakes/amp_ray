

inline material
LambertianMaterial(v3 *Albedo)
{
    material Lambertian = {LAMBERTIAN, *Albedo};

    return Lambertian;
}

inline material
MetalMaterial(v3 *Albedo, r32 Perturbation)
{
    material Metal = { METAL, *Albedo, Perturbation };

    return Metal;
}

inline bool
Scatter(ray *Ray, hit_record *Record, v3 *Attenuation, ray *ScatteredRay)
{
    bool result = false;
    if (Record->Material.Type == LAMBERTIAN)
    {
        v3 Target = Record->Pos + Record->Normal + RandomVecInUnitSphere();
        ScatteredRay->Origin = Record->Pos;
        ScatteredRay->Direction = Target - Record->Pos;
        *Attenuation = Record->Material.Albedo;
        result = true;
    }
    else if (Record->Material.Type == METAL)
    {
        v3 ReflectedRay = Reflect(Normalize(Ray->Direction), Record->Normal);
        ScatteredRay->Origin =  Record->Pos;
        ScatteredRay->Direction = ReflectedRay + Record->Material.Perturbation*RandomVecInUnitSphere();
        *Attenuation = Record->Material.Albedo;
        result = Inner(ScatteredRay->Direction, Record->Normal) > 0;
    }

    return result;
}