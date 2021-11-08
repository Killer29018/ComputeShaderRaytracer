#version 430

layout (local_size_x = 16, local_size_y = 16) in;
layout (rgba32f, binding = 0) uniform image2D imgOutput;

layout (std430, binding = 1) buffer SceneData
{
    vec4 ssbo_SceneData[];
};

uniform int u_ViewportWidth;
uniform int u_ViewportHeight;

uniform int u_TargetWidth;
uniform float u_AspectRatio;

uniform vec3 u_CameraPos;
uniform vec3 u_CameraLookAt;

struct Ray
{
    vec3 origin;
    vec3 direction;
};

struct hitRecord
{
    float t;
    vec3 colour;
};

bool sphereHit(Ray ray, vec3 sphereCenter, float sphereRadius, float tMin, float tMax, vec3 colour, inout hitRecord rec)
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

void main()
{
    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);

    ivec2 invocation = ivec2(gl_LocalInvocationID.xy);
    ivec2 workGroupSize = ivec2(gl_WorkGroupSize.xy);

    float maxX = u_TargetWidth;
    float maxY = maxX / u_AspectRatio;

    ivec2 dims = imageSize(imgOutput);

    float x = float(pixelCoords.x * 2 - dims.x) / dims.x;
    float y = float(pixelCoords.y * 2 - dims.y) / dims.y;

    // Shapes, Shape data, Material
    const ivec4 header = ivec4(ssbo_SceneData[0]);

    const int shapeDataOffset = header.x + 1;
    const int shapeMaterialOffset = header.x + header.y + 1;

    Ray ray;
    ray.origin = vec3(x * maxX, y * maxY, 0.0);
    ray.direction = vec3(0.0, 0.0, -1.0);

    vec3 sphereCenter = vec3(0.0, 0.0, -10.0);
    float sphereRadius = 0.5;

    vec3 unitDirection = normalize(ray.direction);
    float t = 0.5 * (unitDirection.y + 1.0);
    vec3 pixelColour = (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);

    hitRecord rec;
    rec.colour = pixelColour;
    float minT = 0.001;
    float maxT = 10000;
    for (int i = 1; i < header.x + 1; i++)
    {
        hitRecord tempRec;

        bool hit = false;
        ivec4 shape = ivec4(ssbo_SceneData[i]);
        if (shape.x == 0)
        {
            vec3 centre = ssbo_SceneData[shapeDataOffset + shape.y].xyz;
            float radius = ssbo_SceneData[shapeDataOffset + shape.y].w;
            vec3 colour = ssbo_SceneData[shapeMaterialOffset + shape.z].xyz;

            hit = sphereHit(ray, centre, radius, minT, maxT, colour, tempRec);
        }
        // bool hit = sphereHit(ray, ssbo_SceneData[i].xyz, ssbo_SceneData[i].w, minT, maxT, vec3(1.0, 0.0, 0.0), tempRec);
        if (hit)
        {
            maxT = tempRec.t;
            rec = tempRec;
        }
    }

    vec4 pixel = vec4(rec.colour, 1.0);

    imageStore(imgOutput, pixelCoords, pixel);
}