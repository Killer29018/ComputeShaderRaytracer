#version 430

layout (local_size_x = 16, local_size_y = 16) in;
layout (rgba32f, binding = 0) uniform image2D imgOutput;

layout (std430, binding = 1) buffer SSBO_Data
{
    vec4 ssbo_SceneData[];
};

layout (std430, binding = 2) buffer SSBO_Camera
{
    vec3 ssbo_CameraPos;
    vec3 ssbo_CameraLookAt;
    vec3 ssbo_CameraUp;
    float ssbo_CameraFocusDist;
    float ssbo_CameraFOV;
};

uniform int u_ImageWidth;
uniform int u_ImageHeight;

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

void main()
{
    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);

    ivec2 invocation = ivec2(gl_LocalInvocationID.xy);
    ivec2 workGroupSize = ivec2(gl_WorkGroupSize.xy);

    ivec2 dims = imageSize(imgOutput);

    // Shader Viewport X, Y
    float x = (float(pixelCoords.x) / (dims.x));
    float y = (float(pixelCoords.y) / (dims.y));

    // Shapes, Shape data, Material Data
    const ivec4 header = ivec4(ssbo_SceneData[0]);

    const int shapeDataOffset = header.x + 1;
    const int shapeMaterialOffset = header.x + header.y + 1;

    vec3 horizontal = vec3(u_ViewportWidth, 0, 0);
    vec3 vertical = vec3(0.0, u_ViewportHeight, 0);
    vec3 lowerLeftCorner = ssbo_CameraPos - (horizontal/2.0) - (vertical/2.0) - vec3(0, 0, 1.0);

    Ray ray;
    // ray.origin = ssbo_CameraPos + vec3(x * shaderViewportX, y * shaderViewportY, 0.0);
    ray.origin = ssbo_CameraPos;

    // ray.origin = vec3(x * maxX, y * maxY, 0.0);
    ray.direction = lowerLeftCorner + (x*horizontal) + (y*vertical) - ssbo_CameraPos;

    vec3 unitDirection = normalize(ray.direction);
    float t = 0.5 * (unitDirection.y + 1.0);
    vec3 pixelColour = (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);

    // pixelColour = vec3(t, 0, 0);
    // pixelColour = vec3(ssbo_CameraUp);

    HitRecord rec;
    rec.colour = pixelColour;
    float minT = 0.001;
    float maxT = 10000;
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

            vec3 N = normalize(rayAt(ray, tempRec.t) - vec3(0, 0, -1));
            tempRec.colour = 0.5 * vec3(N.x + 1, N.y + 1, N.z + 1);

            rec = tempRec;
        }
    }

    vec4 pixel = vec4(rec.colour, 1.0);

    imageStore(imgOutput, pixelCoords, pixel);
}