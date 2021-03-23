#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 color;

out vec2 TexCoord;
out vec3 fragColor;
out float fragDistance;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float hole_radius;
uniform float hole_depth;
uniform vec3 hole_center;
uniform vec3 cameraPos;
uniform int program_switcher;

void main()
{
    switch(program_switcher){
        case 0:
            TexCoord = vec2(aTexCoord.x, aTexCoord.y);
            gl_Position = vec4(aPos, 1.0f);
            break;

        case 1:
            vec4 world_pos = model*vec4(aPos, 1.0f);

            gl_Position = projection * view * world_pos;
            gl_PointSize = max(1.0f, 10.0f -0.25f*length(gl_Position));
            fragDistance = length(gl_Position);

            fragColor = color;

            /*
            if(distance(world_pos.xyz, hole_center) <= hole_radius && world_pos.y <= bed_level){
                world_pos.y -= hole_depth - (distance(world_pos.xyz, hole_center)/hole_radius) * hole_depth;

                vec3 camera_to_point_ray = world_pos.xyz - cameraPos;
                float modifier = ( bed_level - world_pos.y)/camera_to_point_ray.y;
                ////world_pos.y + x*camera_to_point.y = -bed_level => x = (-bed_level -wolrd_pos.y)/camera_to_point.y;
                vec3 intersection_point = world_pos.xyz + camera_to_point_ray*modifier;
                if(distance(intersection_point, hole_center) > hole_radius){
                    gl_Position = vec4(-2,-2,-2,1);
                    return;
                }

                fragColor = vec3( distance(world_pos.xyz, hole_center) / (hole_radius), 0.1f,0.1f);
                gl_Position = projection * view * world_pos;

                gl_PointSize = max(5.0f, 5.0f -0.15f*length(gl_Position));
                return;
            }
            */

            TexCoord = vec2(aTexCoord.x, aTexCoord.y);
            break;
    }

}
