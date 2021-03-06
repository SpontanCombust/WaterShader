#version 330 core

// ============= OUT ==============
out vec4 fs_out_FragColor;
// ================================


// ============= IN ===============
in VS_OUT {
    vec3 positionModelspace;
	vec4 positionClipspace;
	vec3 normal;
	vec2 uv;
} fs_in;

uniform vec3 uCameraPosition;
uniform sampler2D uTextureRefraction;
uniform sampler2D uTextureReflection;
uniform sampler2D uDUDV;
uniform float uWaveStrength;
uniform float uWaveMovement;
uniform float uReflecivity;
uniform vec4 uTint;
// ================================


// ============= CODE =============
void main()
{
    vec2 ndc = (fs_in.positionClipspace.xy / fs_in.positionClipspace.w) / 2.0 + 0.5;
    vec2 refractUV = vec2(ndc.x, ndc.y);
    vec2 reflectUV = vec2(ndc.x, -ndc.y);

    vec2 dudvCoords1 = fs_in.uv + vec2(uWaveMovement, uWaveMovement);
    vec2 dudvCoords2 = vec2(-fs_in.uv.x, fs_in.uv.y + 0.1) + vec2(uWaveMovement * 0.5, uWaveMovement);
    vec2 distortion1 = (texture(uDUDV, dudvCoords1).rg * 2.0 - 1.0) * uWaveStrength;
    vec2 distortion2 = (texture(uDUDV, dudvCoords2).rg * 2.0 - 1.0) * uWaveStrength;
    
    refractUV += distortion1 + distortion2;
    reflectUV += distortion1 + distortion2;
    refractUV.x = clamp(refractUV.x, 0.001, 0.999);
    refractUV.y = clamp(refractUV.y, 0.001, 0.999);
    reflectUV.x = clamp(reflectUV.x, 0.001, 0.999);
    reflectUV.y = clamp(reflectUV.y, -0.999, -0.001);
    

    vec4 refractColor = texture(uTextureRefraction, refractUV);
    vec4 reflectColor = texture(uTextureReflection, reflectUV);

    vec3 cameraDir = normalize(uCameraPosition - fs_in.positionModelspace);
    float fresnel = max(dot(cameraDir, normalize(fs_in.normal)), 0.0);
    fresnel = pow(fresnel, uReflecivity);

    fs_out_FragColor = mix(reflectColor, refractColor, fresnel) + uTint;
}
// ================================