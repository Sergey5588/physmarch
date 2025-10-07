#version 300 es
//#version 450 core
precision highp float;
precision highp int;

uniform vec2 Resolution;
uniform vec3 Orientation;
uniform vec3 Cam_pos;
uniform int Iterations;
uniform int Shadow_rays;
//uniform vec3 Positions[]= {vec3(0,2,0), vec3(0,0,0), vec3(0, -1, 0), vec3(0,2,0), vec3(0,0,0), vec3(0, -1, 0)};
uniform vec3 Positions[6];
//uniform vec4 Arguments[]= {vec4(1),vec4(1),vec4(1, 1, 0, 0), vec4(1),vec4(0, 1, 0, 0),vec4(1, 1, 0, 0)};
uniform vec4 Arguments[6];
//uniform int lengths[] = {1, 1, 1, 1, 1, 1};
uniform int lengths[6];
out vec4 FragColor;
#define M_PI 3.1415926535897932384626433832795

#define T_SPHERE 0
#define T_BOX 1
#define T_PRISM 2
#define T_TORUS 3
#define T_PLANE 4
#define T_BULB 5

#define O_BASE 0

#define PROCESS_OBJECT_TYPE(type, func) \
    for (int left = lengths[type]; left > 0; object_id++) {\
        min_dist = min(min_dist, func(scene[object_id].pos, ray, scene[object_id].args));\
        left--;\
    }

#define PROCESS_MATERIAL_ID(type, func) \
    for (int left = lengths[type]; left > 0; object_id++) {\
        float dist = func(scene[object_id].pos, ray, scene[object_id].args);\
        if (min_dist > dist) {\
            min_dist = dist;\
            material = scene[object_id].material_id;\
        }\
        left--;\
    }


const float MAX_DIST = 3e38;
const float MIN_DIST = 0.00001;

vec3 sun_dir = normalize(vec3(1, 2, 3));

struct Material {
    float roughness;
    vec4 color;
    bool reflective;
};

struct Object {
    int type;
    int operation;
    vec3 pos;
    vec4 args;
    int material_id;
};

const int UBO_SIZE = 128;

//layout (std140, binding=0) uniform UBO {
layout (std140) uniform UBO {
    Object scene[UBO_SIZE];
};

// const Material materials[] = {
//     Material(0, vec4(1,0,1,1), false),
//     Material(0.5, vec4(0,1,0,1), false),
//     Material(0, vec4(0,0.7,0,1), false)
// };
Material materials[3];

// Object scene[] = {
//     Object(T_SPHERE, O_BASE, vec3(0,2,0), vec4(1), 1),
//     Object(T_BOX, O_BASE, vec3(0,0,0), vec4(1),0),
//     Object(T_PRISM, O_BASE, vec3(1, 0, 0), vec4(1, 1, 0, 0), 1),
//     Object(T_TORUS, O_BASE, vec3(0, 0, 1), vec4(0.5, 1, 0, 0), 1),
//     Object(T_PLANE, O_BASE, vec3(0, -1, 0), vec4(0, 1, 0, 0), 2),
//     Object(T_BULB, O_BASE, vec3(0), vec4(0), 1),
// };

// int lengths[6];
// borrowed from https://www.shadertoy.com/view/33S3Rh
float specular(vec3 camera_pos, vec3 point, vec3 lightDir, vec3 normal, float rougness) {
    float a_coeff_2 = dot(normalize(normalize(camera_pos-point)-lightDir), normal);
    a_coeff_2 *= a_coeff_2;
    float specular = exp(-(1.0-a_coeff_2)/(a_coeff_2*rougness))/(M_PI*rougness*a_coeff_2*a_coeff_2); // cool phycics based specular
    specular = max(specular, 0.0);

    return specular;
}

float sphere(vec3 pos, vec3 ray, vec4 args) {
    return length(pos-ray)-args.x;
}

float box(vec3 pos, vec3 ray, vec4 args) {
    
    vec3 q = abs(pos - ray) - args.xyz;
    return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
    

}

float TriPrism( vec3 pos, vec3 ray, vec4 h)
{
    vec3 p = ray - pos;
    vec3 q = abs(p);
    return max(q.z-h.y,max(q.x*0.866025+p.y*0.5,-p.y)-h.x*0.5);
}

float torus(vec3 pos, vec3 ray, vec4 t)
{
    vec3 p = ray - pos;
    vec2 q = vec2(length(p.xz)-t.x,p.y);
    return length(q)-t.y;
}


float plane(vec3 pos, vec3 ray, vec4 N){
    
    return dot(ray - pos,N.xyz) + N.w;
}

float bulb(vec3 pos, vec3 ray, vec4 args)
{
    //p = p/20.0;
    vec3 w = ray - pos;
    float m = dot(w,w);

    vec4 trap = vec4(abs(w),m);
    float dz = 1.0;
    
    for( int i=0; i<3; i++ ) {

        // trigonometric version (MUCH faster than polynomial)
        
        // dz = 8*z^7*dz
        dz = 8.0*pow(m,3.5)*dz + 1.0;
      
        // z = z^8+c
        float r = length(w);
        float b = 8.0*acos( w.y/r);
        float a = 8.0*atan( w.x, w.z );
        w = w + pow(r,8.0) * vec3( sin(b)*sin(a), cos(b), sin(b)*cos(a) );
        
      
        trap = min( trap, vec4(abs(w),m) );

        m = dot(w,w);
        if( m > 256.0 ) break;
    }


    // distance estimation (through the Hubbard-Douady potential)
    return 0.25*log(m)*sqrt(m)/dz;
}

vec3 repeat(vec3 p, vec3 c) {
    return mod(p + 0.5 * c, c) - 0.5 * c;
}



float angleBetween(vec3 A, vec3 B) {
    float dotAB = dot(A, B);
    float lenA = length(A);
    float lenB = length(B);
    float cosTheta = dotAB / (lenA * lenB);
    return cosTheta; // cos
}


//with max-ts help
float sdfMap(vec3 ray)
{
    materials[0] = Material(0.0, vec4(1,0,1,1), false);
    materials[1] = Material(0.5, vec4(0,1,0,1), false);
    materials[2] = Material(0.0, vec4(0,0.7,0,1), false);
    
    int object_id = 0;
    float min_dist = MAX_DIST+1.0;

    PROCESS_OBJECT_TYPE(T_SPHERE, sphere)
    PROCESS_OBJECT_TYPE(T_BOX, box)
    PROCESS_OBJECT_TYPE(T_PRISM, TriPrism)
    PROCESS_OBJECT_TYPE(T_TORUS, torus)
    PROCESS_OBJECT_TYPE(T_PLANE, plane)
    PROCESS_OBJECT_TYPE(T_BULB, bulb)
    return min_dist;
}
//with max-ts help
int getMaterial(vec3 ray) {
    
    float min_dist = MAX_DIST + 1.0;
    int object_id = 0;
    int material = 0;

    PROCESS_MATERIAL_ID(T_SPHERE, sphere)
    PROCESS_MATERIAL_ID(T_BOX, box)
    PROCESS_MATERIAL_ID(T_PRISM, TriPrism)
    PROCESS_MATERIAL_ID(T_TORUS, torus)
    PROCESS_MATERIAL_ID(T_PLANE, plane)
    PROCESS_MATERIAL_ID(T_BULB, bulb)
    return material;
}


vec3 normal(vec3 point) {
    vec2 e = vec2(.0001, 0); // x smol, y none
    float dist = sdfMap(point);
    // Find normal as tangent of distance function
    return normalize(dist - vec3(
        sdfMap(point - e.xyy),
        sdfMap(point - e.yxy),
        sdfMap(point - e.yyx)
    ));
}



void main() {
    
    
    // for(int i = 0; i< 6; i++) {
        
    //     scene[i].pos = Positions[i];
    //     scene[i].args = Arguments[i];
    // }
    
    // lengths[T_SPHERE] = 1;
    // lengths[T_BOX] = 1;
    // lengths[T_PRISM] = 1;
    // lengths[T_TORUS] = 1;
    // lengths[T_PLANE] = 1;
    // lengths[T_BULB] = 1;
    

    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, Orientation));
    vec3 localUp = normalize(cross(-right, Orientation));
    vec3 focalPoint = vec3(Resolution.x/2.0, Resolution.y/2.0, -400.0);
    vec3 ray = gl_FragCoord.xyz - focalPoint;
    
    vec3 raydir = normalize(ray);
    raydir = right * raydir.x + localUp * raydir.y + Orientation * raydir.z;
    ray = Cam_pos;
    
    float dist = 0.0;

    if (sdfMap(Cam_pos) < 0.0) {
        raydir *= -1.0;
    }

    
    vec3 current_raydir = raydir;
    for (int i = 0; i< Iterations; ++i) {
        // ray = repeat(ray, vec3(1000.0, 1000.0, 1000.0));
        // if (box(vec3(1,1,1), vec3(0,0,3),ray) < min_dist) {
        //     
        // }
        
        if (dist > MAX_DIST) break;
        dist = sdfMap(ray);
        
        // if (sphere(1, vec3(0,0,3),ray) <  box(vec3(2,2,2), vec3(0,0,-1), ray) && sphere(1, vec3(0,0,3),ray) < torus(vec3(0,4,0), vec2(2,1),ray) && dist < min_dist) {
            
        //     current_raydir = reflect(current_raydir, normal(ray));
        //     float boost = min_dist*angleBetween(current_raydir, normal(ray))*(1+min_dist);
        //     dist+=(boost);
        // //}
        // } else if (dist > MAX_DIST) break;
        
        ray += current_raydir * dist;
        
    }

    vec3 shadow_ray = ray + sun_dir * 0.02;
    float shadow_dist = 0.1;
    for (int i = 0; i < Shadow_rays; i++) {
        shadow_dist = sdfMap(shadow_ray);
        shadow_ray += shadow_dist * (sun_dir);
    }
    
    //FragColor = vec4(gl_FragCoord.xy/Resolution.xy,1,1);
    float coef = 1.0;

    if(length(shadow_ray - ray) < 99.0) {
        coef = 0.0;
    }
    int material = getMaterial(ray);

    vec3 obj_color = (materials[material].color.rgb)   * ((max(dot(normal(ray), sun_dir), 0.0)) * coef + 0.3);
    vec4 clr = vec4(obj_color + specular(Cam_pos, ray, -sun_dir, normal(ray), materials[material].roughness), 1.0);
    if(dist >= 0.001) {
        if(ray.y > 0.0) {
            if (dot(raydir, sun_dir) > 0.99) {
                FragColor = vec4(1.0);
            }
            else {
                FragColor = vec4(0.06, 0.67, 0.69, 1.0);
            }
        } else{
            FragColor = vec4(0.06, 0.53, 0.0, 1.0);
        }
        
    } else {
        FragColor = clr;
    }
    // FragColor = vec4(scene[0].pos, 1.0);

}
