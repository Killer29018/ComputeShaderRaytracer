#version 430

layout (local_size_x = 16, local_size_y = 16) in;
layout (rgba32f, binding = 0) uniform image2D imgOutput;

layout (std430, binding = 1) buffer SSBO_SceneData
{
    vec4 ssbo_SceneData[];
};

layout (std430, binding = 2) buffer SSBO_Data
{
    vec3 ssbo_CameraPos;
    vec3 ssbo_CameraLookAt;
    vec3 ssbo_CameraUp;
    float ssbo_CameraViewDist;
    float ssbo_CameraFocusDist;
    float ssbo_CameraFOV;
    uint ssbo_Sampling;
};

uniform float u_ViewportWidth;
uniform float u_ViewportHeight;
uniform float u_AspectRatio;

struct Ray
{
    vec3 origin;
    vec3 direction;
};

struct HitRecord
{
    float t;
    vec3 colour;
};

ivec4 header;
uint shapeDataOffset;
uint shapeMaterialOffset;
vec3 lowerLeftCorner;
vec3 horizontal;
vec3 vertical;

vec3 getPixelColour(in float x, in float y, in float minT, in float maxT);

vec3 rayAt(in Ray r, in float t);
bool sphereHit(in Ray ray, in vec3 sphereCenter, in float sphereRadius, in float tMin, in float tMax, in vec3 colour, inout HitRecord rec);

void main()
{
    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);

    ivec2 invocation = ivec2(gl_LocalInvocationID.xy);
    ivec2 workGroupSize = ivec2(gl_WorkGroupSize.xy);

    ivec2 dims = imageSize(imgOutput);

    // Shader Viewport X, Y
    float x = (float(pixelCoords.x) / float(dims.x - 1));
    float y = (float(pixelCoords.y) / float(dims.y - 1));

    // Shapes, Shape data, Material Data
    header = ivec4(ssbo_SceneData[0]);
    shapeDataOffset = header.x + 1;
    shapeMaterialOffset = header.x + header.y + 1;

    // Camera
    float aspectRatio = 16.0/9.0;
    float viewportHeight = 2.0;
    float viewportWidth = aspectRatio * viewportHeight;

    horizontal = vec3(viewportWidth, 0, 0);
    vertical = vec3(0.0, viewportHeight, 0);
    lowerLeftCorner = ssbo_CameraPos - (horizontal/2.0) - (vertical/2.0) - vec3(0, 0, 1.0);

    // Ray
    // Ray ray;
    // ray.origin = ssbo_CameraPos;
    // ray.direction = lowerLeftCorner + (x*horizontal) + (y*vertical) - ssbo_CameraPos;

    // Get Y coordinate between 0 and 1

    // vec3 finalColour = vec3(0.0);
    // HitRecord rec;
    // rec.colour = vec3(0.0);
    // for (int s = 0; s < ssbo_Sampling; s++)
    // {
    //     rec.colour = vec3(0.0);

    //     float minT = 0.001;
    //     float maxT = 10000;

    //     bool hitAnything = false;

    //     for (int i = 1; i < header.x + 1; i++)
    //     {
    //         HitRecord tempRec;

    //         bool hit = false;
    //         ivec4 shape = ivec4(ssbo_SceneData[i]);
    //         if (shape.x == 0) // Circle
    //         {
    //             vec3 centre = ssbo_SceneData[shapeDataOffset + shape.y].xyz;
    //             float radius = ssbo_SceneData[shapeDataOffset + shape.y].w;
    //             vec3 colour = ssbo_SceneData[shapeMaterialOffset + shape.z].xyz;

    //             hit = sphereHit(ray, centre, radius, minT, maxT, colour, tempRec);
    //         }

    //         if (hit)
    //         {
    //             maxT = tempRec.t;

    //             if (ssbo_SceneData[shapeMaterialOffset + shape.z].w == 0)
    //             {
    //                 vec3 N = normalize(rayAt(ray, tempRec.t) - vec3(0, 0, -1));
    //                 tempRec.colour = 0.5 * vec3(N.x + 1, N.y + 1, N.z + 1);
    //             }

    //             rec = tempRec;

    //             hitAnything = true;
    //         }
    //         // else
    //         // {
    //         //     float t = (ray.direction.y + abs(lowerLeftCorner.y)) / (abs(lowerLeftCorner.y) * 2.0);
    //         //     rec.colour = (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
    //         // }
    //     }

    //     if (!hitAnything)
    //     {
    //         float t = (ray.direction.y + abs(lowerLeftCorner.y)) / (abs(lowerLeftCorner.y) * 2.0);
    //         rec.colour = (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
    //     }

    //     finalColour += rec.colour;
    // }

    vec3 finalColour = getPixelColour(x, y, 0.0001, 10000);

    finalColour *= (1.0 / float(ssbo_Sampling));

    vec4 pixel = vec4(finalColour, 1.0);
    // pixel = vec4(ray.direction, 1.0);

    imageStore(imgOutput, pixelCoords, pixel);
}

vec3 getPixelColour(in float x, in float y, in float minT, in float maxT)
{
    Ray ray;
    ray.origin = ssbo_CameraPos;
    ray.direction = lowerLeftCorner + (x*horizontal) + (y*vertical) - ssbo_CameraPos;

    vec3 finalColour = vec3(0.0);
    HitRecord rec;
    for (int s = 0; s < ssbo_Sampling; s++)
    {
        rec.colour = vec3(0.0);

        bool hitAnything = false;

        for (int i = 1; i < header.x + 1; i++)
        {
            HitRecord tempRec;

            bool hit = false;
            ivec4 shape = ivec4(ssbo_SceneData[i]);
            if (shape.x == 0) // Circle
            {
                vec3 centre = ssbo_SceneData[shapeDataOffset + shape.y].xyz;
                float radius = ssbo_SceneData[shapeDataOffset + shape.y].w;
                vec3 colour = ssbo_SceneData[shapeMaterialOffset + shape.z].xyz;

                hit = sphereHit(ray, centre, radius, minT, maxT, colour, tempRec);
            }

            if (hit)
            {
                maxT = tempRec.t;

                if (ssbo_SceneData[shapeMaterialOffset + shape.z].w == 0)
                {
                    // Get surface normals of sphere
                    vec3 N = normalize(rayAt(ray, tempRec.t) - vec3(0, 0, -1));
                    tempRec.colour = 0.5 * vec3(N.x + 1, N.y + 1, N.z + 1);
                }

                rec = tempRec;

                hitAnything = true;
            }
        }

        if (!hitAnything) // Background Colour
        {
            float t = (ray.direction.y + abs(lowerLeftCorner.y)) / (abs(lowerLeftCorner.y) * 2.0);
            rec.colour = (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
        }

        finalColour += rec.colour;
    }

    return finalColour;
}

vec3 rayAt(in Ray r, in float t)
{
    return r.origin + (t * r.direction);
}

bool sphereHit(Ray ray, vec3 sphereCenter, float sphereRadius, float tMin, float tMax, vec3 colour, inout HitRecord rec)
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
    rec.colour = colour;

    return true;
}