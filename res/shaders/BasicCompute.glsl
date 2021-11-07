#version 430

layout (local_size_x = 16, local_size_y = 16) in;
layout (rgba32f, binding = 0) uniform image2D imgOutput;

layout (location = 1) uniform int u_ViewportWidth;
layout (location = 2) uniform int u_ViewportHeight;

layout (location = 3) uniform int u_TargetWidth;
layout (location = 4) uniform float u_AspectRatio;

layout (location = 5) uniform vec3 u_CameraPos;
layout (location = 6) uniform vec3 u_CameraLookAt;

struct Ray
{
    vec3 origin;
    vec3 direction;
};

bool sphereHit(Ray ray, vec3 sphereCenter, float sphereRadius)
{
    vec3 AC = ray.origin - sphereCenter;
    float a = dot(ray.direction, ray.direction);
    float b = dot(ray.direction, AC);
    float c = dot(AC, AC) - sphereRadius*sphereRadius;

    float discriminant = b*b - a*c;
    if (discriminant < 0.0) return false;

    float sqrtD = sqrt(discriminant);
    float tRoot = (-b - sqrtD) / a;
    if (tRoot < 0.0)
    {
        tRoot = (-b + sqrtD) / a;
        if (tRoot < 0.0)
            return false;
    }

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

    Ray ray;
    ray.origin = vec3(x * maxX, y * maxY, 0.0);
    ray.direction = vec3(0.0, 0.0, -1.0);

    vec3 sphereCenter = vec3(0.0, 0.0, -10.0);
    float sphereRadius = 0.5;

    vec3 unitDirection = normalize(ray.direction);
    float t = 0.5 * (unitDirection.y + 1.0);
    vec3 pixelColour = (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);

    vec4 pixel = vec4(pixelColour, 1.0);
    bool hit = sphereHit(ray, sphereCenter, sphereRadius);
    if (hit)
        pixel = vec4(1.0, 0.0, 0.0, 1.0);

    imageStore(imgOutput, pixelCoords, pixel);
}