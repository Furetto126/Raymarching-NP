#version 460 core

in vec2  FragCoord;
out vec4 FragColor;

const float PI       = 3.141592653589793238462643;
const float INFINITY = 3.402823e38;
const float EPSILON  = 1e-4;

// STRUCTS
// -------
struct Ray {
        vec3 origin;
        vec3 direction;
};

struct Sphere {
        vec3  origin;
        float radius;

        vec3  color;
        int   shouldRender;
};

struct Quad {
        vec3 v1;
        vec3 v2;
        vec3 v3;
        vec3 v4;
};

// SSBOs
// -----
layout (std430, binding = 0) readonly buffer SphereBuffer {
        int    numSpheres;
        Sphere spheres[];
};

// Uniforms
// --------
uniform float time;
uniform float frames;
uniform vec2  resolution;

// Animation
// ---------
uniform float startRandomPhase;
uniform float endRandomPhase;
uniform float startOrbitPhase;
uniform float endOrbitPhase;

uniform float FOV;
uniform vec3  cameraPosition;
uniform vec3  cameraDirection;
uniform vec3  cameraRight;
uniform mat4  inverseViewMatrix;

// Cubemap
uniform samplerCube skybox;

// Function definitions.
vec3 raycast   (inout Ray ray);
vec4 sdfScene  (vec3 p, vec3 dir);                      // Color, distance.
vec3 calcNormal(vec3 p);
vec3 shade     (vec3 p, vec3 diffuse);
vec3 glow      (int i);

// UTILS
// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

// RNGs
uint  PCG_Hash          (uint seed);
float randomFloat       (inout uint seed);
vec3  randomInUnitSphere(inout uint seed);

// Color spaces
vec3 rgb2hsv(vec3 c);
vec3 hsv2rgb(vec3 c);

vec3 environmentalLight(vec3 dir);
vec4 wall(vec3 p, float dst);
// -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
float smoothMin         ( float d1, float d2, float k );
vec2  smin              ( float a,  float b,  float k );
float smoothMax         ( float d1, float d2, float k );
float smoothIntersection( float d1, float d2, float k );

float sdPlane(vec3 p, vec3 n, float h) { return dot(p, n) + h; };

float dot2( in vec2 v ) { return dot(v,v); }
float dot2( in vec3 v ) { return dot(v,v); }
float ndot( in vec2 a, in vec2 b ) { return a.x*b.x - a.y*b.y; }

const float enviromentWeight = 1.0;

void main() {
        vec2 uv = gl_FragCoord.xy/resolution;
        vec3 col = vec3(0.0);

        float tanFOV             = 1.0 / tan(radians(FOV * 0.5));
        vec2  pixelNDC           = (2.0 * gl_FragCoord.xy - resolution) / resolution.y;
        vec3  rayTarget          = (inverseViewMatrix * vec4(pixelNDC, tanFOV, 1.0)).xyz;
        vec3  rayDirection       = normalize(rayTarget - (cameraPosition));

        Ray ray = Ray(cameraPosition, rayDirection);
        col = raycast(ray);

        // Output to screen. If out of random phase
        if (frames >= startRandomPhase && frames <= endRandomPhase) {
                float interval = endRandomPhase - startRandomPhase;
                float periods  = 5.0;

                col = pow(col, vec3(mix(1.0, 1.0, (frames - startRandomPhase) / interval)));
        }
        FragColor = vec4(col, 1.0);
}

vec3 raycast(inout Ray ray)
{
        float dst         = INFINITY;
        vec3  diffuseCol  = vec3(0.0);

        bool hasHit = false;
        const int maxSteps = 100;
        int i              = 0;

        while (i < maxSteps && dst > EPSILON) {
                // Find distance to closest object...
                vec4 colDst = sdfScene(ray.origin, ray.direction);
                diffuseCol  = colDst.xyz;
                dst         = colDst.w;

                // ...then travel along the ray direction for that distance
                float step = max(dst, 0.1);
                ray.origin += ray.direction * step;

                hasHit = dst < EPSILON;

                i++;
        }

        // Metallic effect, from https://www.shadertoy.com/view/ltVBWc
        vec3 normal = round(calcNormal(ray.origin) * 10.0) / 10.0;
        vec3 refl = reflect(ray.direction, normal);
        float fresnel = pow(1.0 - dot(normal, ray.direction), 5.0);

        vec3 albedo = vec3(0.6) + diffuseCol;
        vec3 reflectCol = texture(skybox, refl).rgb;

        vec3 color = mix(reflectCol, albedo * reflectCol, fresnel);
        vec3 bgColor = texture(skybox, ray.direction * vec3(1.0, 1.0, 1.0)).rgb;

        return hasHit ? color : bgColor;
}


float sdSphere( vec3 p, vec3 c, float r )
{
        return length(c-p)-r;
}

const float blendingCoefficient = 0.2;

const vec3  planeNormal = normalize(vec3(0.0, 1.0, 0.0));
const float planeHeight = 10.0;

vec3 repeatDomain(vec3 p)
{
        const vec3 spacing = vec3(20.0, 20.0, 10.0);

        vec3 shift = vec3(4.5, 7.0, 10.0);
        vec3 movedP = p + shift;
        return (movedP - spacing * round(movedP / spacing)) - shift;
}

vec3 displace(vec3 p, vec3 origin)
{
        vec3 dstFromCenter = clamp(vec3(p.x - origin.x, p.y - origin.y, 0.0) / origin, vec3(-1.0), vec3(1.0));
        return p + vec3(1.0, 0.0, 1.0) * sin(length(dstFromCenter) * 10.0 + time * 1.0) * 0.1;
}

float map(vec3 p)
{
        float dst = INFINITY;

        // Domain repetition.
        //if (frames >= startRandomPhase && frames <= endRandomPhase) {
               //p = repeatDomain(p);
        //}

        vec3 planeP = p;

        // Find min distance from all spheres.
        for (int i = 0; i < numSpheres; i++) {
                Sphere s = spheres[i];
                if (s.shouldRender == 0)
                        continue;

                // If not in random nor orbit phase
                if ((frames < startRandomPhase || frames > endRandomPhase) && (frames < startOrbitPhase || frames > endOrbitPhase)) {
                        p = displace(p, s.origin);
                } else {
                        float animationDuration = 120.0;
                        float progression = clamp((frames - startRandomPhase) / animationDuration, 0.0, 1.0);
                        p = mix(displace(p, s.origin), p, progression);
                }

                float currentDst = sdSphere(p, s.origin, s.radius);

                // If in orbit phase
                if (frames > startOrbitPhase && frames < endOrbitPhase) {
                        dst = min(dst, currentDst);
                } else {
                        // Smooth min.
                        vec2 result = smin(dst, currentDst, blendingCoefficient);
                        dst = result.x;
                }
        }

        /*float planeDst   = sdPlane(planeP, planeNormal, planeHeight);
        vec2  result     = smin(dst, planeDst, blendingCoefficient);
        dst = result.x;*/

        return dst;
}

// PASS RAY PLEASE
vec4 sdfScene(vec3 p, vec3 dir)
{
        float dst = INFINITY;
        vec3  blendedColor = vec3(0.0);

        vec3 planeP = p;

        // Domain repetition.
        //if (frames >= startRandomPhase && frames <= endRandomPhase) {
                //p = repeatDomain(p);
        //}

        vec3 displacedPSpheres;
        vec3 finalPos;

        // Find min distance from all spheres.
        for (int i = 0; i < numSpheres; i++) {
                Sphere s = spheres[i];
                if (s.shouldRender == 0)
                        continue;

                // If not in random nor orbit phase
                if ((frames < startRandomPhase || frames > endRandomPhase) && (frames < startOrbitPhase || frames > endOrbitPhase)) {
                        displacedPSpheres = displace(p, s.origin);
                } else {
                        float animationDuration = 120.0;
                        float progression = clamp((frames - startRandomPhase) / animationDuration, 0.0, 1.0);
                        displacedPSpheres = mix(displace(p, s.origin), p, progression);
                }

                float currentDst = sdSphere(displacedPSpheres, s.origin, s.radius);

                // If in orbit phase
                if (frames > startOrbitPhase && frames < endOrbitPhase) {
                        if (currentDst < dst) {
                                dst = currentDst;
                                blendedColor = s.color;
                        }
                } else {
                        // Smooth min.
                        vec2 result = smin(dst, currentDst, blendingCoefficient);
                        if (result.x < dst) finalPos = displacedPSpheres;
                        dst = result.x;
                        blendedColor = blendedColor == vec3(0.0) ? s.color : mix(blendedColor, s.color, result.y);
                }
        }

        // Plane
        /*vec3  planeColor = vec3(1.0, 1.0, 1.0);
        float planeDst   = sdPlane(planeP, planeNormal, planeHeight);
        vec2  result     = smin(dst, planeDst, blendingCoefficient);
        if (result.x < dst) finalPos = planeP;
        dst = result.x;
        blendedColor = mix(blendedColor, planeColor, result.y);*/

        return vec4(blendedColor, dst);
}

// From https://iquilezles.org/articles/normalsSDF/
vec3 calcNormal(vec3 p)
{
        const float h = 1e-5; // replace by an appropriate value
        const vec2 k = vec2(1,-1);
        return normalize(k.xyy * map( p + k.xyy * h ) +
                         k.yyx * map( p + k.yyx * h ) +
                         k.yxy * map( p + k.yxy * h ) +
                         k.xxx * map( p + k.xxx * h ) );
}

vec3 shade(vec3 p, vec3 diffuse)
{
        // Blinn-Phong lighting algorithm.
        // -------------------------------
        const vec3  lightColor    = vec3(1.0);
        const vec3  ambientColor  = vec3(0.1);
        const vec3  specularColor = vec3(1.0);
        const float shininess     = 100.0;
        const float lightStrength = 30.0;

        const vec3 lightPosition  = vec3(-5.0, 4.0, 3.0);

        vec3  lightDirection = lightPosition - p;
        float dstSquared     = dot(lightDirection, lightDirection);
        lightDirection       = normalize(lightDirection);

        vec3 normal = calcNormal(p);

        float lambertian = max(dot(lightDirection, normal), 0.0);
        float specular   = 0.0;

        if (lambertian > 0.0) {
                vec3 viewDir = normalize(cameraPosition - p);

                vec3 h          = normalize(lightDirection + viewDir);
                float specAngle = max(dot(h, normal), 0.0);
                specular        = pow(specAngle, shininess);
        }

        return ambientColor + (lightColor * lightStrength / dstSquared * ((diffuse * lambertian) + (specularColor * specular)));
}

vec3 glow(int i)
{
        // Higher gives more defined glow.
        const float definition   = 2.4;
        // At what amounts of ray marching cycles should the glow be the max? (tangent rays will iterate more cycles!)
        const int fullStrenghtAt = 25;

        return vec3(smoothstep(0.0, 1.0, pow(float(i) / float(fullStrenghtAt), definition)));
}

vec4 wall(vec3 p, float dst)
{
        const float wallDst = 10.0;
        if ((cameraPosition - p).z >= wallDst)
                return vec4(vec3(0.0, 1.0, 0.0), 1.0);
        else
                return vec4(0.0);
}

// Smooth min/max/xor
float smoothMin( float d1, float d2, float k )
{
        float h = clamp( 0.5 + 0.5*(d2-d1)/k, 0.0, 1.0 );
        return mix( d2, d1, h ) - k*h*(1.0-h);
}

// x = min, y = blend factor
vec2 smin( float a, float b, float k )
{
        float h = 1.0 - min( abs(a-b)/(6.0*k), 1.0 );
        float w = h*h*h;
        float m = w*0.5;
        float s = w*k;
        vec2 result = (a<b) ? vec2(a-s,m) : vec2(b-s,1.0-m);

        result.x = min(max(a, b), result.x);
        return result;
}

float smoothMax( float d1, float d2, float k )
{
        float h = clamp( 0.5 - 0.5*(d2+d1)/k, 0.0, 1.0 );
        return mix( d2, -d1, h ) + k*h*(1.0-h);
}

float smoothIntersection( float d1, float d2, float k )
{
        float h = clamp( 0.5 - 0.5*(d2-d1)/k, 0.0, 1.0 );
        return mix( d2, d1, h ) + k*h*(1.0-h);
}

// RNGs
// -----------------
uint PCG_Hash(uint seed)
{
        uint state = seed * 747796405u + 2891336453u;
        uint word  = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
        return (word >> 22u) ^ word;
}

float randomFloat(inout uint seed)
{
        seed = PCG_Hash(seed);
        return float(seed) / 4294967295.0;
}

vec3 randomInUnitSphere(inout uint seed)
{
        return normalize(
                vec3(randomFloat(seed) * 2.0f - 1.0f,
                randomFloat(seed) * 2.0f - 1.0f,
                randomFloat(seed) * 2.0f - 1.0f));
}

// Color spaces
vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

// Others
// ------
vec3 environmentalLight(vec3 dir)
{
        vec3 horizon = vec3(0.9, 1.0, 1.0);
        vec3 zenith  = vec3(0.37, 0.47, 0.61);
        vec3 ground  = vec3(0.41, 0.39, 0.37);

        float a        = pow(smoothstep(0.0, 0.7, dir.y), 0.35);
        vec3  gradient = mix(horizon, zenith, a);
        float b        = smoothstep(-0.01, 0.0, dir.y);
        return mix(ground, gradient, b) * enviromentWeight;
}