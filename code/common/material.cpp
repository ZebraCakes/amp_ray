

inline material
LambertianMaterial(v3 *Albedo)
{
    material Lambertian = {LAMBERTIAN, *Albedo, 0.0, 0.0};

    return Lambertian;
}

inline material
MetalMaterial(v3 *Albedo, r32 Perturbation)
{
    material Metal = { METAL, *Albedo, Perturbation, 0.0 };

    return Metal;
}

inline material
DialectricMaterial(r32 RefractionIndex)
{
    material Dialectric = { DIALECTRIC, V3(0.0, 0.0, 0.0), 0.0, RefractionIndex };

    return Dialectric;
}

internal bool
Refract(v3 *Vec, v3 *N, r32 niOvernt, v3 *RefractedVec)
{
    bool Result = false;

    v3 UV = Normalize(*Vec);
    r32 uvDotn = Dot(UV, *N);
    r32 discriminant = 1.0 - niOvernt*niOvernt*(1.0 - uvDotn*uvDotn);
    if (discriminant > 0)
    {
        *RefractedVec = niOvernt*(UV - *N*uvDotn) - *N*sqrt(discriminant);
        Result = true;
    }

    return Result;
}

inline r32
SchlickApprox(r32 Cosine, r32 RefractionIndex)
{
    r32 r0 = (1.0 - RefractionIndex) / (1.0 + RefractionIndex);
    r0 = r0*r0;
    r32 Result = r0 + (1.0 - r0)*pow((1.0 - Cosine), 5.0);

    return Result;
}

internal bool
Scatter(ray *Ray, hit_record *Record, v3 *Attenuation, ray *ScatteredRay)
{
    bool result = false;
    if (Record->Material.Type == LAMBERTIAN)
    {
        v3 Target = Record->Pos + Record->Normal + RandomVecInUnitSphere();
        ScatteredRay->Origin = Record->Pos;
        ScatteredRay->Direction = Target - Record->Pos;
        ScatteredRay->CastTime = Ray->CastTime;
        *Attenuation = Record->Material.Albedo;
        result = true;
    }
    else if (Record->Material.Type == METAL)
    {
        v3 ReflectedRay = Reflect(Normalize(Ray->Direction), Record->Normal);
        ScatteredRay->Origin =  Record->Pos;
        ScatteredRay->Direction = ReflectedRay + Record->Material.Perturbation*RandomVecInUnitSphere();
        ScatteredRay->CastTime = Ray->CastTime;
        *Attenuation = Record->Material.Albedo;
        result = Dot(ScatteredRay->Direction, Record->Normal) > 0;
    }
    else if (Record->Material.Type == DIALECTRIC)
    {
        v3 Normal;
        v3 ReflectedVec = Reflect(Ray->Direction, Record->Normal);

        r32 niOvernt;
        *Attenuation = V3(1.0, 1.0, 1.0);
        v3 Refracted;
        r32 ReflectProbability;
        r32 Cosine;
        if (Dot(Ray->Direction, Record->Normal) > 0)
        {
            Normal = -Record->Normal;
            niOvernt = Record->Material.RefractionIndex;
            Cosine = Record->Material.RefractionIndex * Dot(Ray->Direction, Record->Normal) / Length(Ray->Direction);
        }
        else
        {
            Normal = Record->Normal;
            niOvernt = 1.0 / Record->Material.RefractionIndex;
            Cosine = -Dot(Ray->Direction, Record->Normal) / Length(Ray->Direction);
        }

        ScatteredRay->Origin = Record->Pos;

        if (Refract(&Ray->Direction, &Normal, niOvernt, &Refracted))
        {
            ReflectProbability = SchlickApprox(Cosine, Record->Material.RefractionIndex);
        }
        else
        {
            ReflectProbability = 1.0;
        }

        if (RandomRealInRange(0.0, 1.0) < ReflectProbability)
        {
            ScatteredRay->Direction = ReflectedVec;
        }
        else
        {
            ScatteredRay->Direction = Refracted;
        }

        ScatteredRay->CastTime = Ray->CastTime;
        result = true;
    }
    return result;
}
