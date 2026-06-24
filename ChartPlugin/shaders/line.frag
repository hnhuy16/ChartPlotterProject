#version 440

layout(location = 0) in float distance;
layout(location = 0) out vec4 fragColor;

layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    vec4 color;
    float qt_Opacity;
};

void main()
{
    float feather = max(fwidth(distance), 0.0001);
    float edge = smoothstep(1.0 - feather, 1.0, abs(distance));
    fragColor = color * qt_Opacity * (1.0 - edge);
}
