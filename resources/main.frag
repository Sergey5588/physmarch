#version 460 core
uniform vec2 Resolution;
uniform vec3 Orientation;
uniform vec3 Cam_pos;
uniform int Iterations = 256;
uniform int Shadow_rays = 100;
out vec4 FragColor;
#define M_PI 3.1415926535897932384626433832795
// types:
// 0 - sphere
// 1 - box
// 2 - prism
// 3 - etc

// operations
// 0 - base

const float MAX_DIST = 10000;
const float min_dist = 0.00001;

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


const Material materials[] = {
    Material(0, vec4(1,0,1,1), false),
    Material(0.5, vec4(0,1,0,1), false),
    Material(0, vec4(0,0.7,0,1), false)
};

const Object scene[] = {
    Object(0, 0, vec3(0,2,0), vec4(1), 1),
    Object(1, 0, vec3(0,0,0), vec4(1),0),
    //Object(5, 0, vec3(0), vec4(0), 0),
    Object(6, 0, vec3(0, -1, 0), vec4(0), 2)
};

int nearest;
// borrowed from https://www.shadertoy.com/view/33S3Rh
float specular(vec3 camera_pos, vec3 point, vec3 lightDir, vec3 normal, float rougness) {
    float a_coeff_2 = dot(normalize(normalize(camera_pos-point)-lightDir), normal);
    a_coeff_2 *= a_coeff_2;
    float specular = exp(-(1.0-a_coeff_2)/(a_coeff_2*rougness))/(M_PI*rougness*a_coeff_2*a_coeff_2); // cool phycics based specular
    specular = max(specular, 0.0);

    return specular;
}

float sphere(vec3 pos, vec3 ray, float radius) {
    return length(pos-ray)-radius;
}

float box(vec3 pos, vec3 ray, vec3 sizes) {
    
    vec3 q = abs(pos - ray) - sizes;
    return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
    

}

float TriPrism( vec3 pos, vec3 ray, vec2 h)
{
    vec3 p = ray - pos;
    vec3 q = abs(p);
    return max(q.z-h.y,max(q.x*0.866025+p.y*0.5,-p.y)-h.x*0.5);
}

float torus(  vec3 pos, vec3 ray, vec2 t )
{
    vec3 p = ray - pos;
    vec2 q = vec2(length(p.xz)-t.x,p.y);
    return length(q)-t.y;
}


float plane(vec3 ray, vec4 N){
    
    return dot(ray,N.xyz) + N.w;
}

float bulb(vec3 p )
{
    //p = p/20.0;
    vec3 w = p;
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
        w = p + pow(r,8.0) * vec3( sin(b)*sin(a), cos(b), sin(b)*cos(a) );
        
      
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

float getDist(vec3 ray, Object obj) {
    switch (obj.type) {
        case 0:
            return sphere(obj.pos, ray, obj.args.x);
        case 1:
            return box(obj.pos, ray, obj.args.xyz);
        case 5:
            return bulb(ray);
        case 6:
            return ray.y - obj.pos.y;
    
    }
    
    
    
    return 0.0;
}

float sdfMap(vec3 ray)
{
    //this is faster
    float sc = min(sphere(vec3(0,0,0), ray, 1), box(vec3(0,2,0), ray, vec3(1,1,1)));
    sc = min(sc, ray.y +1);
    return sc;
    // than this :(
    if(scene.length() == 1) return getDist(ray, scene[0]);

    for(int i = 1; i < scene.length(); i++) {
        float curr = getDist(ray, scene[i]);
        if(i==1) {
            float last = getDist(ray, scene[i-1]);


            if(last>curr) {
                nearest = i;
                sc = curr;
            } else {
                nearest = i-1;
                sc = last;
            }
        } else {
            

            if(sc>curr) {
                nearest = i;
                sc = curr;
            }
        }

    }
    // float sc = min(sphere(1, vec3(0,0,3),ray),box(vec3(2,2,2), vec3(0,0,-1), ray));
    // sc = min(sc, TriPrism(vec3(0,5,0),vec2(1,1), ray));
    
    return sc;
    
    //return sphere(100, vec3(0,200,300), ray);
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
    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, Orientation));
    vec3 localUp = normalize(cross(-right, Orientation));
    vec3 focalPoint = vec3(Resolution.x/2, Resolution.y/2, -400.0);
    vec3 ray = gl_FragCoord.xyz - focalPoint;
    
    vec3 raydir = normalize(ray);
    raydir = right * raydir.x + localUp * raydir.y + Orientation * raydir.z;
    ray = Cam_pos;
    
    float dist = 0.0;

    if (sdfMap(Cam_pos) < 0) {
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

    vec3 obj_color = (materials[scene[nearest].material_id].color.rgb)   * ((max(dot(normal(ray), sun_dir), 0.0)) * coef + 0.3);
    vec4 clr = vec4(obj_color + specular(Cam_pos, ray, -sun_dir, normal(ray), materials[scene[nearest].material_id].roughness), 1.0);
    if(dist >= 0.001) {
        if(ray.y > 0) {
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
    //FragColor = clr;
}
