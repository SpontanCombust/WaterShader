#version 330 core

// ============= OUT ==============
out vec4 fs_out_FragColor;
// ================================


// ============= IN ===============
in VS_OUT {
    vec3 texCoords;
} fs_in;

uniform samplerCube uSkybox;
// ================================


// ============= CODE =============
void main() {
    fs_out_FragColor = texture(uSkybox, fs_in.texCoords);
}
// ================================