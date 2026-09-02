#version 330 core

#define uResolution iResolution.xy
#define uTime iTime
#define uMouse vec2(iMouse.x, iResolution.y - iMouse.y)

//my changes to make it work:
// oColor bc FragColor already defined and we cant conflict
out vec4 oColor;
uniform vec3  iResolution;
uniform float iTime;
uniform vec4  iMouse;


const vec2 clickPos[8] = vec2[8](vec2(0.0),vec2(0.0),vec2(0.0),vec2(0.0),vec2(0.0),vec2(0.0),vec2(0.0),vec2(0.0));
const float clickAge[8] = float[8](1e6,1e6,1e6,1e6,1e6,1e6,1e6,1e6);
#define time uTime
const float noiseScale = 11.0;
#define screenSize uResolution
#define sampleOffset vec2(0.0)
#define renderSize uResolution
const float cellSize = 1.0;

#define morphPhase (uTime*0.4)
#define spinPhase (uTime*0.05)
const float orbitRadius = 0.9;
#define scrollSpeed vec2(-0.15, 0.15)
const vec2 loopD = vec2(0.0, 0.0);
const float tileOn = 0.0;
const vec2 tileP = vec2(0.0, 0.0);
const vec2 tileScale = vec2(0.0, 0.0);
#define mousePos uMouse
const float hoverEnable = 1.0;
const float hoverTarget = 0.0;
const float hoverStrength = 0.5;
const float hoverCount = 2.0;
const float hoverRadius = 50.0;
const float hoverFalloff = 1.0;
const float hoverScatter = 0.0;
const float hoverDragOnly = 0.0;
const vec2 dragVec = vec2(0.0);
const float dragActive = 0.0;
const float clickEnable = 0.0;
const float clickTarget = 0.0;
const float clickStrength = 0.5;
const float clickCount = 2.0;
const float clickRadius = 80.0;
const float clickDecay = 1.0;
const float clickFalloff = 0.5;
const float jitter = 0.1;
const float minkowskiP = 3.4;
const float starPoints = 4.0;
const float starDepth = 0.1;
const float cellStretch = 1.9;
const float cellAngle = 0.0;
const float cellRadius = 0.4;
const float smoothK = 0.3;
const float edgeSharpen = 8.0;
const float ringCount = 2.0;
const float octaves = 2.0;
const float gain = 0.4;
const float polar = 0.0;

const vec4 PALETTE[110] = vec4[110](
  vec4(0.594669,0.35145,0.0372267,1.0),
  vec4(0.506343,0.333877,0.161309,1.0),
  vec4(0.418017,0.316303,0.285391,1.0),
  vec4(0.32969,0.29873,0.409473,1.0),
  vec4(0.241364,0.281157,0.533555,1.0),
  vec4(0.253944,0.287113,0.520527,1.0),
  vec4(0.266523,0.29307,0.5075,1.0),
  vec4(0.279102,0.299026,0.494472,1.0),
  vec4(0.291682,0.304982,0.481445,1.0),
  vec4(0.304261,0.310939,0.468417,1.0),
  vec4(0.31684,0.316895,0.45539,1.0),
  vec4(0.32942,0.322852,0.442362,1.0),
  vec4(0.341999,0.328808,0.429334,1.0),
  vec4(0.354579,0.334764,0.416307,1.0),
  vec4(0.367158,0.340721,0.403279,1.0),
  vec4(0.379737,0.346677,0.390252,1.0),
  vec4(0.392317,0.352634,0.377224,1.0),
  vec4(0.404896,0.35859,0.364197,1.0),
  vec4(0.417476,0.364546,0.351169,1.0),
  vec4(0.430055,0.370503,0.338142,1.0),
  vec4(0.442634,0.376459,0.325114,1.0),
  vec4(0.455214,0.382416,0.312086,1.0),
  vec4(0.467793,0.388372,0.299059,1.0),
  vec4(0.480372,0.394328,0.286031,1.0),
  vec4(0.492952,0.400285,0.273004,1.0),
  vec4(0.505531,0.406241,0.259976,1.0),
  vec4(0.518111,0.412198,0.246949,1.0),
  vec4(0.53069,0.418154,0.233921,1.0),
  vec4(0.543269,0.424111,0.220894,1.0),
  vec4(0.555849,0.430067,0.207866,1.0),
  vec4(0.568428,0.436023,0.194838,1.0),
  vec4(0.581008,0.44198,0.181811,1.0),
  vec4(0.593587,0.447936,0.168783,1.0),
  vec4(0.606166,0.453893,0.155756,1.0),
  vec4(0.618746,0.459849,0.142728,1.0),
  vec4(0.631325,0.465805,0.129701,1.0),
  vec4(0.643905,0.471762,0.116673,1.0),
  vec4(0.656484,0.477718,0.103645,1.0),
  vec4(0.669063,0.483675,0.0906179,1.0),
  vec4(0.681643,0.489631,0.0775903,1.0),
  vec4(0.694222,0.495587,0.0645628,1.0),
  vec4(0.706801,0.501544,0.0515352,1.0),
  vec4(0.709559,0.505235,0.0518871,1.0),
  vec4(0.712316,0.508926,0.0522389,1.0),
  vec4(0.715074,0.512616,0.0525908,1.0),
  vec4(0.717831,0.516307,0.0529426,1.0),
  vec4(0.720588,0.519998,0.0532945,1.0),
  vec4(0.723346,0.523689,0.0536463,1.0),
  vec4(0.726103,0.52738,0.0539982,1.0),
  vec4(0.72886,0.531071,0.05435,1.0),
  vec4(0.731618,0.534761,0.0547019,1.0),
  vec4(0.734375,0.538452,0.0550537,1.0),
  vec4(0.737132,0.542143,0.0554056,1.0),
  vec4(0.73989,0.545834,0.0557574,1.0),
  vec4(0.742647,0.549525,0.0561093,1.0),
  vec4(0.745404,0.553215,0.0564611,1.0),
  vec4(0.748162,0.556906,0.056813,1.0),
  vec4(0.750919,0.560597,0.0571648,1.0),
  vec4(0.753676,0.564288,0.0575167,1.0),
  vec4(0.756434,0.567979,0.0578685,1.0),
  vec4(0.759191,0.57167,0.0582204,1.0),
  vec4(0.761949,0.57536,0.0585722,1.0),
  vec4(0.764706,0.579051,0.0589241,1.0),
  vec4(0.767463,0.582742,0.0592759,1.0),
  vec4(0.770221,0.586433,0.0596278,1.0),
  vec4(0.772978,0.590124,0.0599796,1.0),
  vec4(0.775735,0.593815,0.0603315,1.0),
  vec4(0.778493,0.597505,0.0606833,1.0),
  vec4(0.780331,0.599966,0.0609179,1.0),
  vec4(0.78125,0.601196,0.0610352,1.0),
  vec4(0.78125,0.601196,0.0610352,1.0),
  vec4(0.78125,0.601196,0.0610352,1.0),
  vec4(0.78125,0.601196,0.0610352,1.0),
  vec4(0.783275,0.604889,0.0697293,1.0),
  vec4(0.787326,0.612274,0.0871175,1.0),
  vec4(0.793403,0.623352,0.1132,1.0),
  vec4(0.799479,0.63443,0.139282,1.0),
  vec4(0.805556,0.645508,0.165365,1.0),
  vec4(0.811632,0.656586,0.191447,1.0),
  vec4(0.817708,0.667664,0.217529,1.0),
  vec4(0.823785,0.678741,0.243612,1.0),
  vec4(0.829861,0.689819,0.269694,1.0),
  vec4(0.835938,0.700897,0.295776,1.0),
  vec4(0.842014,0.711975,0.321859,1.0),
  vec4(0.84809,0.723053,0.347941,1.0),
  vec4(0.854167,0.734131,0.374023,1.0),
  vec4(0.860243,0.745209,0.400106,1.0),
  vec4(0.866319,0.756287,0.426188,1.0),
  vec4(0.872396,0.767365,0.452271,1.0),
  vec4(0.878472,0.778442,0.478353,1.0),
  vec4(0.884549,0.78952,0.504435,1.0),
  vec4(0.890625,0.800598,0.530518,1.0),
  vec4(0.896701,0.811676,0.5566,1.0),
  vec4(0.902778,0.822754,0.582682,1.0),
  vec4(0.908854,0.833832,0.608765,1.0),
  vec4(0.914931,0.84491,0.634847,1.0),
  vec4(0.921007,0.855988,0.660929,1.0),
  vec4(0.927083,0.867065,0.687012,1.0),
  vec4(0.93316,0.878143,0.713094,1.0),
  vec4(0.939236,0.889221,0.739176,1.0),
  vec4(0.945313,0.900299,0.765259,1.0),
  vec4(0.951389,0.911377,0.791341,1.0),
  vec4(0.957465,0.922455,0.817424,1.0),
  vec4(0.963542,0.933533,0.843506,1.0),
  vec4(0.969618,0.944611,0.869588,1.0),
  vec4(0.975694,0.955688,0.895671,1.0),
  vec4(0.981771,0.966766,0.921753,1.0),
  vec4(0.987847,0.977844,0.947835,1.0),
  vec4(0.993924,0.988922,0.973918,1.0),
  vec4(0.997975,0.996307,0.991306,1.0)
);
vec4 paletteLookup(float x){
  int i = clamp(int(clamp(x,0.0,1.0)*256.0),0,255);
  return PALETTE[clamp(int(float(i)/255.0*110.0),0,109)];
}

#define TAU 6.28318530718
#define MAX_OCTAVES 10
#define SPIN_RATE_MAX  1.5

#define MAX_CLICKS 8          

#define ZOOM_AMT     1.0
#define REPEL_AMT    0.3
#define SCATTER_FREQ 60.0

vec2 frameC(vec2 uv) {
    vec2 r = renderSize;
    return (uv - 0.5) * screenSize / r.y;
}

vec2 toPolar(vec2 uv) {
    vec2 c = frameC(uv);
    return vec2(atan(c.y, c.x) / TAU, length(c));
}

vec2 latticeOffset(vec2 cell) {
    float row = floor(cell.y);
    
    return vec2(mod(row, 2.0) * 0.5, 0.0);
}

vec2 hash2(vec2 p) {
    p = vec2(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3)));
    return fract(sin(p) * 43758.5453);
}

vec2 wrapCell(vec2 cell, float angN, vec2 d) {
    if (angN > 0.5) cell.x = mod(cell.x, angN);
    if (tileOn > 0.5) return mod(cell, d);
    
    return cell;
}

float metricDist(vec2 d, float ang) {
    float s = sin(ang), c = cos(ang);
    d = mat2(c, -s, s, c) * d;
    d.x *= cellStretch; d.y /= cellStretch;
    float p  = max(minkowskiP, 0.1);
    float md = pow(pow(abs(d.x), p) + pow(abs(d.y), p), 1.0 / p);
    return md * (1.0 + starDepth * cos(starPoints * atan(d.y, d.x)));
}

float cellSpin(vec2 cell) {
    float turns = cellAngle / 360.0;
    {
        float k = (hash2(cell + 53.0).x * 2.0 - 1.0) * SPIN_RATE_MAX;

        turns += spinPhase * k;
    }
    return -turns * TAU;
}

float smin(float a, float b, float k) {
    if (k <= 0.0) return min(a, b);
    float h = clamp(0.5 + 0.5 * (a - b) / k, 0.0, 1.0);
    return mix(a, b, h) - k * h * (1.0 - h);
}

void consider(float d, inout float f1, inout float f2) {
    if      (d < f1) f2 = f1;
    else if (d < f2) f2 = d;
    f1 = smin(f1, d, smoothK);
}

float comboValue(float f1, float f2) {
    float s = 1.0 / max(cellRadius, 0.001);
    f1 *= s; f2 *= s;
    float v;
    v = f1 / max(f2, 0.001);
    v = clamp(v, 0.0, 1.0);
    v = pow(v, edgeSharpen);
    v = 0.5 - 0.5 * cos(v * ringCount * TAU);
    return clamp(v, 0.0, 1.0);
}

float whash2(vec2 p) {
    p = fract(p * vec2(127.1, 311.7));
    p += dot(p, p.yx + 19.19);
    return fract((p.x + p.y) * 43.32);
}
float wnoise2(vec2 p) {
    vec2 i = floor(p), f = fract(p);
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(mix(whash2(i + vec2(0.0, 0.0)), whash2(i + vec2(1.0, 0.0)), u.x),
               mix(whash2(i + vec2(0.0, 1.0)), whash2(i + vec2(1.0, 1.0)), u.x), u.y);
}

vec2 warpUV(vec2 uv) {
    return uv;
}

vec2 rotateUV(vec2 uv) {
    return uv;
}

#define CELL_EMPTY_LO 4.0
#define CELL_EMPTY_HI 7.0
#define CELL_EMPTY_VALUE 1.0

float cellFinish(float f1, float f2, bool doCells) {
    float v = comboValue(f1, f2);
    if (doCells) v = mix(v, CELL_EMPTY_VALUE, smoothstep(CELL_EMPTY_LO, CELL_EMPTY_HI, f1));
    
    return v;
}

float cellOrbit(vec2 p, float angN, vec2 d, bool isBase) {
    
    vec2 ip = floor(p), fp = fract(p);
    float f1 = 8.0, f2 = 8.0;
    for (int j = -2; j <= 2; j++) {
        for (int i = -2; i <= 2; i++) {
            vec2 g    = vec2(float(i), float(j));
            vec2 cell = wrapCell(ip + g, angN, d);
            vec2 rnd  = hash2(cell);
            vec2 anim = 0.5 + orbitRadius * sin(morphPhase * TAU + rnd * TAU);
            vec2 pt   = g + mix(vec2(0.5), anim, jitter);
            pt += latticeOffset(cell) * (1.0 - jitter);
            
            float d = metricDist(pt - fp, cellSpin(cell));
            
            consider(d, f1, f2);
        }
    }
    
    return cellFinish(f1, f2, false);
}

float cellEval(vec2 p, float angN, vec2 d, bool isBase) {
    return cellOrbit(p, angN, d, isBase);
}

float cellFBM(vec2 p, float angN) {
    float v = 0.0, a = 0.5, norm = 0.0;
    vec2  d = (tileOn > 0.5) ? tileP : loopD;
    int oct = int(octaves + 0.5);
    for (int i = 0; i < MAX_OCTAVES; i++) {
        if (i >= oct) break;
        v    += a * cellEval(p, angN, d, i == 0);
        norm += a;
        p     = p * 2.0 + vec2(5.3, 1.7);
        a    *= gain;
        if (angN > 0.5) angN *= 2.0;

        d    *= 2.0;
    }
    return v / norm;
}

float inflMask(vec2 block, vec2 uv, vec2 center, float radius, float falloff, float scatter) {
    float d     = length(block - center);
    float inner = radius * (1.0 - falloff);
    float m     = 1.0 - smoothstep(inner, max(radius, inner + 0.5), d);
    if (scatter > 0.001) m *= mix(1.0, wnoise2(uv * SCATTER_FREQ), scatter);
    return m;
}

float clickRingMaskAt(vec2 block, vec2 uv, int i) {
    float life = clickAge[i] / max(clickDecay, 0.05);
    if (life >= 1.0) return 0.0;
    float thick = clickRadius * mix(0.06, 0.5, clickFalloff) + 0.5;
    float d     = length(block - clickPos[i]);
    float ringR = clickRadius * life;
    float m     = (1.0 - smoothstep(0.0, thick, abs(d - ringR))) * (1.0 - life);
    
    return m;
}

void route(float target, float strength, float mask, float count, vec2 uv, vec2 centerUV, vec2 smearPx,
           inout float valAdd, inout vec2 uvDisp) {
    float amp = strength * mask;
    if (target < 0.5) {
        valAdd += amp;
    } else if (target < 1.5) {
        uvDisp += (uv - centerUV) * amp * ZOOM_AMT;
    } else if (target < 2.5) {
        vec2 dir = (length(smearPx) > 1e-4) ? smearPx / screenSize : (uv - centerUV);
        float L = length(dir);

        if (L > 1e-5) uvDisp -= (dir / L) * amp * REPEL_AMT;
    } else if (target > 6.5) {
        float n = max(count, 1.0);
        float k = (target < 7.5) ? (1.0 / n) : n;
        uvDisp += (uv - centerUV) * (k - 1.0) * mask;
    }
}

vec4 shade(vec2 screen_coords) {
    vec2 block = floor((screen_coords + sampleOffset) / cellSize) * cellSize;
    vec2 uv0   = warpUV(block / screenSize);

    float valAdd = 0.0;
    vec2  uvDisp = vec2(0.0);
    float hgate  = hoverEnable * ((hoverDragOnly > 0.5) ? dragActive : 1.0);
    vec2  hsmear = (hoverDragOnly > 0.5) ? dragVec : vec2(0.0);
    float mH = hgate * inflMask(block, uv0, mousePos, hoverRadius, hoverFalloff, hoverScatter);
    route(hoverTarget, hoverStrength, mH, hoverCount, uv0, mousePos / screenSize, hsmear, valAdd, uvDisp);
    for (int i = 0; i < MAX_CLICKS; i++) {
        float mC = clickRingMaskAt(block, uv0, i) * clickEnable;
        route(clickTarget, clickStrength, mC, clickCount, uv0, clickPos[i] / screenSize, vec2(0.0),
              valAdd, uvDisp);
    }

    vec2 uv = rotateUV(uv0 + uvDisp);

    vec2  p;
    float angN = 0.0;
    if (polar > 0.5) {
        vec2 pc = toPolar(uv);
        angN = max(1.0, floor(noiseScale + 0.5));
        p = vec2((pc.x + 0.5) * angN, pc.y * noiseScale) + time * scrollSpeed;
    } else if (tileOn > 0.5) {
        
        p = uv * tileScale + time * scrollSpeed;
    } else {
        p = uv * noiseScale + time * scrollSpeed;
    }
    p.y *= 1.1547;

    float v = cellFBM(p, angN);
    v = clamp(v + valAdd, 0.0, 1.0);

    return paletteLookup(v);
}


void mainImage(out vec4 fragColor, in vec2 fragCoord){
  vec4 c = shade(vec2(fragCoord.x, iResolution.y - fragCoord.y));
  fragColor = c;
}

void main() {
    vec4 col;
    mainImage(col, gl_FragCoord.xy);
    oColor = col;
}
