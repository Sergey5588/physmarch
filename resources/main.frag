#version 460 core
uniform vec2 Resolution;
uniform vec3 Orientation;
uniform vec3 Cam_pos;
uniform int Iterations = 100;
out vec4 FragColor;

const int ray_depth = 10;
float box(vec3 sizes, vec3 pos, vec3 ray) {
    
    vec3 q = abs(pos - ray) - sizes;
    return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
    

}

float bulb(vec3 p )
{
    //p = p/20.0;
    vec3 w = p;
    float m = dot(w,w);

    vec4 trap = vec4(abs(w),m);
	float dz = 1.0;
    
	for( int i=0; i<3; i++ )
    {

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
		if( m > 256.0 )
            break;
    }


    // distance estimation (through the Hubbard-Douady potential)
    return 0.25*log(m)*sqrt(m)/dz;
}

vec3 repeat(vec3 p, vec3 c) {
    return mod(p + 0.5 * c, c) - 0.5 * c;
}



// correct way to repeat space every s units



float sphere(float radius, vec3 pos, vec3 ray) {
    return length(pos-ray)-radius;
}
float testSDFComplitation(vec3 ray)
{
    
    //return max(-sphere(150, vec3(0,0,200), ray), box(vec3(100,100,100), vec3(0,0,300),ray));
    //return min(sphere(150, vec3(0,200,200), ray), box(vec3(100,100,100), vec3(0,0,300),ray));
    return min(box(vec3(1,1,1), vec3(0,0,3),ray), bulb(ray));
    
    //return sphere(100, vec3(0,200,300), ray);
}
vec3 normal(vec3 point) {
    vec2 e = vec2(.0001, 0); // x smol, y none
    float dist = testSDFComplitation(point);
    // Find normal as tangent of distance function
    return normalize(dist - vec3(
        testSDFComplitation(point - e.xyy),
        testSDFComplitation(point - e.yxy),
        testSDFComplitation(point - e.yyx)
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
    float min_dist = 0.0001;
    float dist = 0.0;

    if (testSDFComplitation(Cam_pos) < 0) {
        raydir *= -1.0;
    }

    
    vec3 current_raydir = raydir;
    int ref_count = 0;
    for (int i = 0; i< Iterations; ++i) {
        //ray = repeat(ray, vec3(1000.0, 1000.0, 1000.0));
        dist = testSDFComplitation(ray);
        // if (box(vec3(1,1,1), vec3(0,0,3),ray) < min_dist) {
        //     current_raydir = reflect(current_raydir, normal(ray));
        // }
        

        // if (box(vec3(1,1,1), vec3(0,0,3),ray) <  bulb(ray) && dist <= min_dist) {
        //     for(int j = 0; j < ray_depth; ++j) {

        //         dist = testSDFComplitation(ray);
        //         current_raydir = reflect(current_raydir, normal(ray));
                
                
        //     }
            
        // }
        
        ray += current_raydir * dist;
        
        
        
    }
    
    //FragColor = vec4(gl_FragCoord.xy/Resolution.xy,1,1);
    vec4 clr = vec4(normal(ray) / 2.0f + 0.5f, 1.0);
    if(dist >= 0.001) {
        if(ray.y > 0) {
            FragColor = vec4(0.5,0.5,0.5,1);
        }else{
            FragColor = vec4(0,0.6,0, 1);
        }
    } else {
        FragColor = clr;
    }
    
}