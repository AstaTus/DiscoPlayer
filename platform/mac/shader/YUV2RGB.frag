varying vec2 texture_Out;
uniform sampler2D tex_y;
uniform sampler2D tex_u;
uniform sampler2D tex_v;
void main(void)
{
    vec3 YUV;
    vec3 RGB;
    YUV.x = texture2D(tex_y, texture_Out).r;
    YUV.y = texture2D(tex_u, texture_Out).r - 0.5;
    YUV.z = texture2D(tex_v, texture_Out).r - 0.5;
    RGB = mat3(1.0, 1.0, 1.0,
        0.0, -0.39465, 2.03211,
        1.13983, -0.58060, 0.0) * YUV;
    gl_FragColor = vec4(RGB, 1.0);
}