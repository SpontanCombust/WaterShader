#version 330 core

// ============= OUT ==============
out vec4 fs_out_FragColor;
// ================================


// ============= IN ===============
in VS_OUT {
    vec3 positionModelspace;
	vec4 positionScreenspace;
	vec3 normal;
	// vec2 uv;
} fs_in;

uniform vec3 uCameraPosition;
uniform sampler2D uTextureRefraction;
uniform sampler2D uTextureReflection;
// ================================


// ============= CODE =============
void main()
{
    vec2 ndc = (fs_in.positionScreenspace.xy / fs_in.positionScreenspace.w) / 2.0 + 0.5;
    vec2 refractUV = vec2(ndc.x, ndc.y);
    vec2 reflectUV = vec2(ndc.x, -ndc.y);

    vec4 refractColor = texture(uTextureRefraction, refractUV);
    vec4 reflectColor = texture(uTextureReflection, reflectUV);

    vec3 cameraDir = normalize(uCameraPosition - fs_in.positionModelspace);
    float fresnel = max(dot(cameraDir, normalize(fs_in.normal)), 0.0);
    fresnel = pow(fresnel, 4.0);

    fs_out_FragColor = mix(reflectColor, refractColor, fresnel);
}
// ================================