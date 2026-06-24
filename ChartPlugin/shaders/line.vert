#version 440

layout(location = 0) in vec2 position;
layout(location = 1) in float distanceToCenter;

layout(location = 0) out float distance;

layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    vec4 color;
    float qt_Opacity;
};

out gl_PerVertex { vec4 gl_Position; };

void main()
{
    gl_Position = qt_Matrix * vec4(position, 0.0, 1.0);
    distance = distanceToCenter;
}
