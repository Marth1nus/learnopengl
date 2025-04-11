#version 300 es
precision highp float;
precision mediump int;

out vec4 color;

uniform vec3 /*  */ iResolution; // viewport resolution (in pixels)
uniform float /* */ iTime;       // shader playback time (in seconds)
uniform float /* */ iTimeDelta;  // render time (in seconds)
uniform float /* */ iFrameRate;  // shader frame rate
uniform int /*   */ iFrame;      // shader playback frame
uniform vec4 /*  */ iMouse;      // mouse pixel coords. xy: current (if MLB down), zw: click
uniform vec4 /*  */ iDate;       // (year, month, day, time in seconds)

/*  From: https://www.shadertoy.com/view/3csSWB
    "Singularity" by @XorDev

    I'll come back to clean up the code later.
    Feel free to code golf!
    
    FabriceNeyret2: -19
    dean_the_coder: -12
    iq: -4
*/
void mainImage(out vec4 O, vec2 F)
{
  float i, j;
  vec2 r = iResolution.xy, p = (F + F - r) / r.y / .7f, d = vec2(-1, 1), q = 5.f * p - d, c = p * mat2(1, 1, d / (.1f + 5.f / dot(q, q))), v = c * mat2(cos(.5f * log(j = dot(c, c)) + iTime * .2f + vec4(0, 33, 11, 0))) * 5.f, s;
  for (; i++ < 9.f; s += 1.f + sin(v)) v += .7f * sin(v.yx * i + iTime) / i + .5f;
  i = length(sin(v / .3f) * .4f + c * (3.f + d));
  O = 1.f - exp(-exp(c.x * vec4(.6f, -.4f, -1, 0)) / s.xyyx / (2.f + i * i / 4.f - i) / (.5f + 3.5f * exp(.3f * c.y - j)) / (.03f + abs(length(p) - .7f)));
}

void main()
{
  // color = vec4(1.0f, 0.1f, 1.0f, 1.0f);
  mainImage(color, gl_FragCoord.xy);
}