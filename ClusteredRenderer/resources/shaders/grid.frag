in vec2 uv;

out vec4 FragColor;

// Helper fns
float log10(float x) {
  return log(x) / log(10.0);
}
float satf(float x) {
  return clamp(x, 0.0, 1.0);
}
vec2 satv(vec2 x) {
  return clamp(x, vec2(0.0), vec2(1.0));
}
float max2(vec2 v) {
  return max(v.x, v.y);
}
//

const float gridSize = 100.0;
const float gridCellSize = 0.025;

const vec4 gridColorThin = vec4(0.5, 0.5, 0.5, 1.0);
const vec4 gridColorThick = vec4(0.0, 0.0, 0.0, 1.0);

const float gridMinPixelsBetweenCells = 2.0;

void main()
{
    vec2 dudv = vec2(
      length(vec2(dFdx(uv.x), dFdy(uv.x))),
      length(vec2(dFdx(uv.y), dFdy(uv.y)))
    );

    float lodLevel = max(0.0, log10((length(dudv) * gridMinPixelsBetweenCells) / gridCellSize) + 1.0);
    float lodFade = fract(lodLevel);

    float lod0 = gridCellSize * pow(10.0, floor(lodLevel+0));
    float lod1 = gridCellSize * pow(10.0, floor(lodLevel+1));
    float lod2 = gridCellSize * pow(10.0, floor(lodLevel+2));

    dudv *= 4.0;

    float test = max2( vec2(1.0) - abs(satv(mod(uv, lod0) / dudv) * 2.0 - vec2(1.0f)));

    float lod0a = max2( vec2(1.0) - abs(satv(mod(uv, lod0) / dudv) * 2.0 - vec2(1.0f)));
    float lod1a = max2(vec2(1.0) - abs(satv(mod(uv, lod1) / dudv) * 2.0 - vec2(1.0f)));
    float lod2a = max2(vec2(1.0) - abs(satv(mod(uv, lod2) / dudv) * 2.0 - vec2(1.0f)));

    vec4 c = lod2a > 0.0 ? gridColorThick : lod1a > 0.0 ?
        mix(gridColorThick, gridColorThin, lodFade) :
        gridColorThin;

    float opacityFalloff = (1.0 - satf(length(uv) / gridSize));
    c.a *= lod2a > 0.0 ? lod2a : lod1a > 0.0 ?
        lod1a : (lod0a * (1.0-lodFade));
    c.a *= opacityFalloff;

    FragColor = c;
}   