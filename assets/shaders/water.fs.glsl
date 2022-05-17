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
uniform float uWaveMovement;

const float WAVE_STRENGTH = 0.01;
// ================================


// ============= CODE =============
void main()
{
    vec2 ndc = (fs_in.positionClipspace.xy / fs_in.positionClipspace.w) / 2.0 + 0.5;
    vec2 refractUV = vec2(ndc.x, ndc.y);
    vec2 reflectUV = vec2(ndc.x, -ndc.y);

    vec2 dudvCoords1 = fs_in.uv + vec2(uWaveMovement, uWaveMovement);
    vec2 dudvCoords2 = vec2(-fs_in.uv.x, fs_in.uv.y + 0.1) + vec2(uWaveMovement * 0.5, uWaveMovement);
    vec2 distortion1 = (texture(uDUDV, dudvCoords1).rg * 2.0 - 1.0) * WAVE_STRENGTH;
    vec2 distortion2 = (texture(uDUDV, dudvCoords2).rg * 2.0 - 1.0) * WAVE_STRENGTH;
    refractUV += distortion1 + distortion2;
    reflectUV += distortion1 + distortion2;

    vec4 refractColor = texture(uTextureRefraction, refractUV);
    vec4 reflectColor = texture(uTextureReflection, reflectUV);

    vec3 cameraDir = normalize(uCameraPosition - fs_in.positionModelspace);
    float fresnel = max(dot(cameraDir, normalize(fs_in.normal)), 0.0);
    fresnel = pow(fresnel, 2.0);

    vec3 blueTint = vec3(0.0, 0.05, 0.15);
    fs_out_FragColor = mix(reflectColor, refractColor, fresnel) + vec4(blueTint, 1.0);
}
// ================================