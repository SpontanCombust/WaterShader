#version 330 core

// ============ TYPES =============
struct Light
{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};



// ============= OUT ==============
out vec4 fs_out_FragColor;
// ================================


// ============= IN ===============
in VS_OUT {
	vec3 position;
	vec3 normal;
	vec2 uv;
} fs_in;

// uniform vec3 uCameraPosition;
uniform sampler2D uMaterialDiffuse;
uniform Light uLight;
// ================================


// ============= CODE =============
void main()
{
    float diffuseImpact = max(dot(fs_in.normal, -normalize(uLight.direction)), 0.0);

    vec3 ambient = uLight.ambient * texture(uMaterialDiffuse, fs_in.uv).rgb;
    vec3 diffuse = uLight.diffuse * texture(uMaterialDiffuse, fs_in.uv).rgb * diffuseImpact;

    fs_out_FragColor = vec4(ambient + diffuse, 1.0);
}
// ================================