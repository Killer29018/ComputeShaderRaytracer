#version 430

#define PI 3.14159

struct Shape
{
    vec3 position;
    vec3 size;
    vec3 colour;

    // ShapeType MatType MatExtra
    vec4 extraInfo;
};

layout (local_size_x = 16, local_size_y = 16) in;
layout (rgba32f, binding = 0) uniform image2D imgOutput;
layout (rgba32f, binding = 1) uniform image2D imgData;

layout (std430, binding = 2) buffer SSBO_SceneData
{
    Shape ssbo_SceneData[];
};

layout (std430, binding = 3) buffer SSBO_Data
{
    vec3 ssbo_CameraPos;
    vec3 ssbo_CameraLookAt;
    vec3 ssbo_CameraUp;
    vec3 ssbo_BackgroundColour;
    float ssbo_CameraViewDist;
    float ssbo_CameraFocusDist;
    float ssbo_CameraFOV;
    float ssbo_CameraAperture;
    uint ssbo_MaxDepth;
    float ssbo_AspectRatio;
};

uniform float u_SampleCount;

struct Ray
{
    vec3 origin;
    vec3 direction;
};

struct Material
{
    vec3 albedo;
    float extra;
};

struct HitRecord
{
    vec3 point;
    vec3 normal;
    bool frontFace;

    uint matType;
    Material mat;

    float t;
};

vec3 lowerLeftCorner;
vec3 horizontal;
vec3 vertical;
vec3 w, u, v;
float lensRadius;

uint seed;

vec3 getPixelColour(in float minT, in float maxT);
bool worldHit(in Ray r, in float minT, in float maxT, inout HitRecord rec);

void setFrontFace(inout HitRecord rec, in Ray ray, in vec3 normal);
vec3 rayAt(in Ray r, in float t);
void getRay(inout Ray r, float x, float y);

bool sphereHit(in Ray ray, in vec3 sphereCenter, in float sphereRadius, in float tMin, in float tMax, inout HitRecord rec);
bool xyRectHit(in Ray ray, in vec3 position, in vec3 size, in float tMin, in float tMax, inout HitRecord rec);
bool xzRectHit(in Ray ray, in vec3 position, in vec3 size, in float tMin, in float tMax, inout HitRecord rec);
bool yzRectHit(in Ray ray, in vec3 position, in vec3 size, in float tMin, in float tMax, inout HitRecord rec);

bool scatterLambertian(in Ray r, inout HitRecord rec, out vec3 attenuation, out Ray scattereed);
bool scatterMetal(in Ray r, inout HitRecord rec, out vec3 attenuation, out Ray scattereed);
bool scatterDielectric(in Ray r, inout HitRecord rec, out vec3 attenuation, out Ray scattered);
bool scatterDiffuseLight(in Ray r, inout HitRecord rec, out vec3 attenuation, out Ray scattered);

vec3 emittedDiffuseLight(in vec3 point);

uint hash(uint key);
float rand(inout uint seed);
vec2 randVec2(inout uint seed);
vec3 randVec3(inout uint seed);
vec3 randInUnitSphere(inout uint seed);
vec3 randInUnitDisc(inout uint seed);

void main()
{
    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);

    ivec2 invocation = ivec2(gl_LocalInvocationID.xy);
    ivec2 workGroupSize = ivec2(gl_WorkGroupSize.xy);

    ivec2 dims = imageSize(imgOutput);

    seed = hash(gl_GlobalInvocationID.x) ^ hash(gl_GlobalInvocationID.y * workGroupSize.y) ^ hash(uint(u_SampleCount));

    // Camera
    float theta = radians(ssbo_CameraFOV);
    float h = tan(theta/2.0);
    float viewportHeight = 2.0 * h;
    float viewportWidth = ssbo_AspectRatio * viewportHeight;

    w = normalize(ssbo_CameraPos - ssbo_CameraLookAt);
    u = normalize(cross(ssbo_CameraUp, w));
    v = cross(w, u);

    horizontal = ssbo_CameraFocusDist * viewportWidth * u;
    vertical = ssbo_CameraFocusDist * viewportHeight * v;
    lowerLeftCorner = ssbo_CameraPos - (horizontal/2.0) - (vertical/2.0) - (ssbo_CameraFocusDist*w);

    lensRadius = ssbo_CameraAperture / 2.0;

    // Get Pixel Colour
    float infinite = 1.0/0.0;
    vec3 finalColour = getPixelColour(0.0001, infinite);

    // Get previous summed total
    vec4 previousPx = imageLoad(imgData, pixelCoords);
    finalColour += previousPx.rgb;

    // Add Colour to the storage Image
    imageStore(imgData, pixelCoords, vec4(finalColour, 1.0));

    // Gamma Correction
    float gamma = 1.0 / float(u_SampleCount);
    finalColour.r = sqrt(gamma * finalColour.r);
    finalColour.g = sqrt(gamma * finalColour.g);
    finalColour.b = sqrt(gamma * finalColour.b);

    vec4 pixel = vec4(finalColour, 1.0);

    // pixel = vec4(s.colour.xyz, 1.0);

    // Store Pixel
    imageStore(imgOutput, pixelCoords, pixel);
}

vec3 getPixelColour(in float minT, in float maxT)
{
    Ray ray;

    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
    ivec2 dims = imageSize(imgOutput);

    vec3 finalColour = vec3(0.0);
    HitRecord rec;

    uint depth = ssbo_MaxDepth;
    Ray currentRay;

    float x = (float(pixelCoords.x) + (rand(seed) / 2.0)) / float(dims.x);
    float y = (float(pixelCoords.y) + (rand(seed) / 2.0)) / float(dims.y);

    getRay(currentRay, x, y);

    vec3 colour = vec3(1.0);
    vec3 emittedTotal = vec3(1.0);
    while (true)
    {
        if (depth <= 0)
        {
            colour = vec3(0.0);
            break;
        }

        HitRecord tempRec;

        if (worldHit(currentRay, minT, maxT, tempRec))
        {
            Ray scattered;
            vec3 attenuation;
            vec3 emitted = vec3(0.0);

            bool hit = false;
            switch (tempRec.matType)
            {
            case 0:
                hit = scatterLambertian(currentRay, tempRec, attenuation, scattered);
                break;
            case 1:
                hit = scatterMetal(currentRay, tempRec, attenuation, scattered);
                break;
            case 2:
                hit = scatterDielectric(currentRay, tempRec, attenuation, scattered);
                break;
            case 3:
                emitted = tempRec.mat.albedo;
                hit = scatterDiffuseLight(currentRay, tempRec, attenuation, scattered);
                break;
            }

            // colour = tempRec.mat.albedo;
            // break;

            if (hit)
            {
                currentRay = scattered;

                colour *= (emitted + attenuation);
            }
            else
            {
                colour *= emitted;
                depth = 0;
                break;
            }

            rec = tempRec;

        }
        else
        {
            colour *= ssbo_BackgroundColour;

            break;
        }

        depth--;
    }

    return colour;
}

bool worldHit(in Ray ray, in float minT, in float maxT, inout HitRecord rec)
{
    bool hitAnything = false;
    float closest = maxT;

    for (int i = 0; i < ssbo_SceneData.length(); i++)
    {
        bool hit = false;
        Shape shape = ssbo_SceneData[i];
        switch (uint(shape.extraInfo.x))
        {
        case 0: // Circle
            vec3 centre = shape.position;
            float radius = shape.size.x;

            hit = sphereHit(ray, centre, radius, minT, closest, rec);
            break;
        case 1: // XY Rect
            hit = xyRectHit(ray, shape.position, shape.size, minT, closest, rec);
            break;
        case 2: // XZ Rect
            hit = xzRectHit(ray, shape.position, shape.size, minT, closest, rec);
            break;
        case 3: // YZ Rect
            hit = yzRectHit(ray, shape.position, shape.size, minT, closest, rec);
            break;
        }

        if (hit)
        {
            vec3 colour = shape.colour;
            float extra = shape.extraInfo.z;
            uint materialType = uint(shape.extraInfo.y);

            Material mat;
            mat.albedo = colour;
            mat.extra = extra;

            rec.mat = mat;
            rec.matType = materialType;

            closest = rec.t;
            hitAnything = true;
        }
    }

    return hitAnything;
}

void setFrontFace(inout HitRecord rec, in Ray ray, in vec3 normal)
{
    rec.frontFace = dot(ray.direction, normal) < 0;
    rec.normal = rec.frontFace ? normal : -normal;
}

vec3 rayAt(in Ray r, in float t)
{
    return r.origin + (t * r.direction);
}

void getRay(inout Ray ray, float x, float y)
{
    vec3 rd = lensRadius * randInUnitDisc(seed);
    vec3 offset = u*rd.x + v*rd.y;

    ray.origin = ssbo_CameraPos + offset;
    ray.direction = lowerLeftCorner + (x*horizontal) + (y*vertical) - ssbo_CameraPos - offset;
}

bool sphereHit(Ray ray, vec3 sphereCenter, float sphereRadius, float tMin, float tMax, inout HitRecord rec)
{
    vec3 AC = ray.origin - sphereCenter;
    float a = dot(ray.direction, ray.direction);
    float b = dot(ray.direction, AC);
    float c = dot(AC, AC) - sphereRadius*sphereRadius;

    float discriminant = b*b - a*c;
    if (discriminant < 0.0) return false;

    float sqrtD = sqrt(discriminant);
    float tRoot = (-b - sqrtD) / a;
    if (tRoot < tMin || tRoot > tMax)
    {
        tRoot = (-b + sqrtD) / a;
        if (tRoot < tMin || tRoot > tMax)
            return false;
    }

    rec.t = tRoot;
    rec.point = rayAt(ray, tRoot);
    vec3 normal = (rec.point - sphereCenter) / sphereRadius;
    setFrontFace(rec, ray, normal);

    return true;
}

bool xyRectHit(in Ray ray, in vec3 position, in vec3 size, in float tMin, in float tMax, inout HitRecord rec)
{
    float k = position.z;

    float t = (k - ray.origin.z) / ray.direction.z;

    if (t < tMin || t > tMax)
        return false;

    float x = ray.origin.x + t*ray.direction.x;
    float y = ray.origin.y + t*ray.direction.y;

    if (x < position.x || x > size.x || y < position.y || y > size.y)
        return false;

    rec.t = t;
    rec.point = rayAt(ray, t);
    vec3 normal = vec3(0, 0, 1);
    setFrontFace(rec, ray, normal);

    return true;
}

bool xzRectHit(in Ray ray, in vec3 position, in vec3 size, in float tMin, in float tMax, inout HitRecord rec)
{
    float k = position.y;

    float t = (k - ray.origin.y) / ray.direction.y;

    if (t < tMin || t > tMax)
        return false;

    float x = ray.origin.x + t*ray.direction.x;
    float z = ray.origin.z + t*ray.direction.z;

    if (x < position.x || x > size.x || z < position.z || z > size.z)
        return false;

    rec.t = t;
    rec.point = rayAt(ray, t);
    vec3 normal = vec3(0, 1, 0);
    setFrontFace(rec, ray, normal);

    return true;
}

bool yzRectHit(in Ray ray, in vec3 position, in vec3 size, in float tMin, in float tMax, inout HitRecord rec)
{
    float k = position.x;

    float t = (k - ray.origin.x) / ray.direction.x;

    if (t < tMin || t > tMax)
        return false;

    float y = ray.origin.y + t*ray.direction.y;
    float z = ray.origin.z + t*ray.direction.z;

    if (y < position.y || y > size.y || z < position.z || z > size.z)
        return false;

    rec.t = t;
    rec.point = rayAt(ray, t);
    vec3 normal = vec3(1, 0, 0);
    setFrontFace(rec, ray, normal);

    return true;
}

bool scatterLambertian(in Ray ray, inout HitRecord rec, out vec3 attenuation, out Ray scattered)
{
    vec3 scatterDirection = rec.normal + normalize(randInUnitSphere(seed));

    float s = 1e-8;
    if (abs(scatterDirection.x) < s && abs(scatterDirection.y) < s && abs(scatterDirection.z) < s)
    {
        scatterDirection = rec.normal;
    }

    scattered.origin = rec.point;
    scattered.direction = scatterDirection;

    attenuation = rec.mat.albedo;
    return true;
}

bool scatterMetal(in Ray ray, inout HitRecord rec, out vec3 attenuation, out Ray scattered)
{
    vec3 reflected = reflect(normalize(ray.direction), rec.normal);
    scattered.origin = rec.point;
    scattered.direction = reflected + rec.mat.extra*randInUnitSphere(seed);

    attenuation = rec.mat.albedo;
    return dot(scattered.direction, rec.normal) > 0;
}

float reflectance(float cosine, float refIdx)
{
    float r0 = (1-refIdx) / (1+refIdx);
    r0 = r0*r0;
    return r0 +(1-r0)*pow((1-cosine), 5);
}

bool scatterDielectric(in Ray ray, inout HitRecord rec, out vec3 attenuation, out Ray scattered)
{
    attenuation = vec3(1.0);

    float refractionRatio = rec.frontFace ? (1.0/rec.mat.extra) : rec.mat.extra;

    vec3 unitDirection = normalize(ray.direction);
    float cosTheta = min(dot(-unitDirection, rec.normal), 1.0);
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);

    bool cannotRefract = refractionRatio * sinTheta > 1.0;
    vec3 direction;
    if (cannotRefract || reflectance(cosTheta, refractionRatio) > rand(seed)) direction = reflect(unitDirection, rec.normal);
    else direction = refract(unitDirection, rec.normal, refractionRatio);

    scattered.origin = rec.point;
    scattered.direction = direction;
    return true;
}

bool scatterDiffuseLight(in Ray ray, inout HitRecord rec, out vec3 attenuation, out Ray scattered)
{
    return false;
}

uint hash(uint key)
{
    uint h = 0;
    for (int i=0; i < 4; ++i) {
        h += (key >> (i * 8)) & 0xFF;
        h += h << 10;
        h ^= h >> 6;
    }
    h += h << 3;
    h ^= h >> 11;
    h += h << 15;
    return h;
}

float rand(inout uint seed)
{
    seed = 0xadb4a92d * seed + 1;

    uint m = (seed >> 9) | 0x40000000u;

    float f = uintBitsToFloat(m);   // Range [2:4]
    return (f / 2.0) - 1.0;
}

vec2 randVec2(inout uint seed)
{
    return vec2(rand(seed), rand(seed));
}

vec3 randVec3(inout uint seed)
{
    return vec3(rand(seed), rand(seed), rand(seed));
}

vec3 randInUnitSphere(inout uint seed)
{
    float x = rand(seed) * 2.0 - 1.0;
    float y = rand(seed) * 2.0 - 1.0;
    float z = rand(seed) * 2.0 - 1.0;
    return normalize(vec3(x, y, z));
}

vec3 randInUnitDisc(inout uint seed)
{
    float r = sqrt(rand(seed));
    float theta = rand(seed) * 2 * PI;

    return vec3(r * cos(theta), r * sin(theta), 0.0);
}