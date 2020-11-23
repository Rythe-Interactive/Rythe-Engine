#pragma once
#include <vector>
#include <string>

#define SV_START 0

/* 0  */  #define SV_TEXCOORD0      SV_START   
/* 1  */  #define SV_TEXCOORD1      SV_TEXCOORD0 + 1     
/* 2  */  #define SV_TEXCOORD2      SV_TEXCOORD1 + 1     
/* 3  */  #define SV_TEXCOORD3      SV_TEXCOORD2 + 1     

/* 4  */  #define SV_POSITION       SV_TEXCOORD3 + 1
/* 5  */  #define SV_COLOR          SV_POSITION + 1
/* 6  */  #define SV_NORMAL         SV_COLOR + 1     
/* 7  */  #define SV_TANGENT        SV_NORMAL + 1      
/* 8  */  #define SV_MODELMATRIX    SV_TANGENT + 1 

/* 12 */  #define SV_ALBEDO         SV_MODELMATRIX + 4      
/* 13 */  #define SV_NORMALHEIGHT   SV_ALBEDO + 1   
/* 14 */  #define SV_MRDAO          SV_NORMALHEIGHT + 1    
/* 15 */  #define SV_EMISSIVE       SV_MRDAO + 1        
/* 16 */  #define SV_HEIGHTSCALE    SV_EMISSIVE + 1  

/* 11 */  #define SV_MATERIAL       SV_ALBEDO

/* 0  */  #define SV_VIEW           SV_START
/* 1  */  #define SV_PROJECT        SV_VIEW + 1
/* 2  */  #define SV_CAMPOS         SV_PROJECT + 1
/* 3  */  #define SV_VIEWDIR        SV_CAMPOS + 1
/* 4  */  #define SV_CAMIDX         SV_VIEWDIR + 1

/* 0  */  #define SV_CAMERA         SV_VIEW

/* 5  */  #define SV_LIGHT_COUNT    SV_CAMIDX + 1
/* 0  */  #define SV_LIGHTS         SV_START

namespace legion::rendering::detail
{
    inline const std::vector<std::string>& get_default_defines()
    {
        static std::vector<std::string> defines;
        if (defines.empty())
        {
            defines.push_back("SV_TEXCOORD0=" +    std::to_string(SV_TEXCOORD0));
            defines.push_back("SV_TEXCOORD1=" +    std::to_string(SV_TEXCOORD1));
            defines.push_back("SV_TEXCOORD2=" +    std::to_string(SV_TEXCOORD2));
            defines.push_back("SV_TEXCOORD3=" +    std::to_string(SV_TEXCOORD3));
            defines.push_back("SV_POSITION=" +     std::to_string(SV_POSITION));
            defines.push_back("SV_COLOR=" +        std::to_string(SV_COLOR));
            defines.push_back("SV_NORMAL=" +       std::to_string(SV_NORMAL));
            defines.push_back("SV_TANGENT=" +      std::to_string(SV_TANGENT));
            defines.push_back("SV_MODELMATRIX=" +  std::to_string(SV_MODELMATRIX));

            defines.push_back("SV_ALBEDO=" +       std::to_string(SV_ALBEDO));
            defines.push_back("SV_NORMALHEIGHT=" + std::to_string(SV_NORMALHEIGHT));
            defines.push_back("SV_MRDAO=" +        std::to_string(SV_MRDAO));
            defines.push_back("SV_EMISSIVE=" +     std::to_string(SV_EMISSIVE));
            defines.push_back("SV_HEIGHTSCALE=" +  std::to_string(SV_HEIGHTSCALE));

            defines.push_back("SV_MATERIAL=" +     std::to_string(SV_MATERIAL));

            defines.push_back("SV_VIEW=" +         std::to_string(SV_VIEW));
            defines.push_back("SV_PROJECT=" +      std::to_string(SV_PROJECT));
            defines.push_back("SV_CAMPOS=" +       std::to_string(SV_CAMPOS));
            defines.push_back("SV_VIEWDIR=" +      std::to_string(SV_VIEWDIR));
            defines.push_back("SV_CAMIDX=" +       std::to_string(SV_CAMIDX));

            defines.push_back("SV_CAMERA=" +       std::to_string(SV_CAMERA));
            defines.push_back("SV_LIGHTS=" +       std::to_string(SV_LIGHTS));
            defines.push_back("SV_LIGHT_COUNT=" +  std::to_string(SV_LIGHT_COUNT));
        }

        return defines;
    }
}
