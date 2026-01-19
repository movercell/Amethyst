#version 460 core
out vec4 FragColor;

void main()
{
    const float near = 0.1f;
    const float far = 100.0f;
    float linearDepth = (2.0 * near * far) / (far + near - gl_FragCoord.z * (far - near));
    linearDepth /= 100.0f;
    FragColor = vec4(0.3f * linearDepth, 0.3f * linearDepth, 0.7f * linearDepth, 1.0f);
    //FragColor = vec4(0.3f, 0.3f, 0.7f, 1.0f);
}

