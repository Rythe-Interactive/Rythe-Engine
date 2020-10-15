#pragma once

#define SV_START 0

/* 0  */  #define SV_TEXCOORD0 SV_START   
/* 1  */  #define SV_TEXCOORD1 SV_TEXCOORD0 + 1     
/* 2  */  #define SV_TEXCOORD2 SV_TEXCOORD1 + 1     
/* 3  */  #define SV_TEXCOORD3 SV_TEXCOORD2 + 1     

/* 4  */  #define SV_POSITION SV_TEXCOORD3 + 1          
/* 5  */  #define SV_NORMAL SV_POSITION + 1     
/* 6  */  #define SV_TANGENT SV_NORMAL + 1      
/* 7  */  #define SV_MODELMATRIX SV_TANGENT + 1 

/* 11 */  #define SV_ALBEDO SV_MODELMATRIX + 4      
/* 12 */  #define SV_NORMALHEIGHT SV_ALBEDO + 1   
/* 13 */  #define SV_MRDAO SV_NORMALHEIGHT + 1    
/* 14 */  #define SV_EMISSIVE SV_MRDAO + 1        
/* 15 */  #define SV_HEIGHTSCALE SV_EMISSIVE + 1  

/* 11 */  #define SV_MATERIAL SV_ALBEDO

/* 0  */  #define SV_VIEW SV_START
/* 4  */  #define SV_PROJECT SV_VIEW + 4
/* 8  */  #define SV_CAMPOSIDX SV_PROJECT + 4
/* 10 */  #define SV_VIEWDIR SV_CAMPOSIDX + 1

/* 0  */  #define SV_CAMERA SV_VIEW
