#version 460 core
out vec4 FragColor;
in VertexData {
    vec3 Position;
    vec3 Normal;
    vec2 UV;
};

vec4 GenerateTemporaryTexture() {
    uint isOther = 0u;
    if (UV.x >= 0.5f)
        isOther = 1u;
    if (UV.y >= 0.5f)
        isOther += 2u;
    vec3 ret = vec3(1.0f);
    return isOther != 0u && isOther != 3u ? vec4(ret.xyz, 1.0f) : vec4((ret * 0.5f).xyz, 1.0f);
}

void main()
{
    const float near = 1.0f;
    const float far = 32768.0f;
    float linearDepth = (2.0 * near * far) / (far + near - gl_FragCoord.z * (far - near));
    linearDepth /= 100.0f;
    FragColor = GenerateTemporaryTexture() * vec4(0.3f * linearDepth, 0.3f * linearDepth, 0.7f * linearDepth, 1.0f);
}

