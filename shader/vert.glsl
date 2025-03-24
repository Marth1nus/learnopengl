#version 300 es
precision highp float;
precision mediump int;

uniform vec3 /*  */ iResolution;           // viewport resolution (in pixels)
uniform float /* */ iTime;                 // shader playback time (in seconds)
uniform float /* */ iTimeDelta;            // render time (in seconds)
uniform float /* */ iFrameRate;            // shader frame rate
uniform int /*   */ iFrame;                // shader playback frame
uniform vec4 /*  */ iMouse;                // mouse pixel coords. xy: current (if MLB down), zw: click
uniform vec4 /*  */ iDate;                 // (year, month, day, time in seconds)

in vec2 vertex_position;

void main()
{
  vec2 pos = vertex_position * min(0.9f, 1.3f - 1.0f / (iTime * 1.0f)) * (0.95f + sin(iTime * 0.5f) * 0.05f);
  pos += vec2(cos(-iTime), sin(-iTime)) * 0.05f;
  vec2 mouseOffset = (iMouse.xy / iResolution.xy) * 2.0f - 1.0f;
  pos += clamp(mouseOffset * vec2(-1, 1) - pos, -1.0f, 1.0f) * 0.1f;
  gl_Position = vec4(pos, 0, 1);
}