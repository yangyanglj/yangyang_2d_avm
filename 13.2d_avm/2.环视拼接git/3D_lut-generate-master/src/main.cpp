#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

//#include "render.h"
//#include "ldc_config.h"
//#include "lens_distortion_correction.h"
//#include "lens_distortion_correction.cxx"

#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))

#define POINTS_WIDTH (1080)
#define POINTS_HEIGHT (1080)
#define POINTS_SUBX 4
#define POINTS_SUBY 4

#define QUADRANTS 4
#define QUADRANT_WIDTH (POINTS_WIDTH/(POINTS_SUBX * 2)+1)
#define QUADRANT_HEIGHT (POINTS_HEIGHT/(POINTS_SUBY * 2)+1)
#define QUADRANT_SIZE (QUADRANT_WIDTH * QUADRANT_HEIGHT)

#define QUADBASED
#define NUM_CAMERAS 4
#define CALMAT_T_SHIFT 10 //平移数值 缩放
#define CALMAT_R_SHIFT 30 //旋转数值 缩放   ，计算优化过程

#define PI 3.14159265

#if !defined(SRV_LDC_LENS_BIN_FILE)
#define SRV_LDC_LENS_BIN_FILE "./LENS.BIN"
#endif
#if !defined(SRV_CALMAT_BIN_FILE)
#define SRV_CALMAT_BIN_FILE "./CALMAT.BIN"
#endif

#define SRV_STATIC_LUT_BIN_FILE "./srv_lut.bin"
#define SRV_STATIC_INDICES "./srv_indices.bin"
#define SRV_STATIC_BLEND_LUT_BIN_FILE "./srv_blend_lut.bin"

#define OVERLAP_REGION_ANGULAR_WIDTH_DEGREES 10.0f

#define GL_TEXCOORD_DATATYPE GLshort
#define GL_VERTEX_DATATYPE GLshort
#define GL_BLEND_DATATYPE unsigned char

/*data type*/
#if LDC_LIB_DATA_TYPE==2
typedef Int32 dtype;
#elif LDC_LIB_DATA_TYPE==1
typedef double dtype;
#elif LDC_LIB_DATA_TYPE==0
typedef float        dtype;
#endif

typedef short        GLshort;

typedef int        Int32;//?????????????????????????????????????
typedef unsigned int uint32_t;
/*status flag*/
typedef enum {
	LDC_STATUS_OK,
	LDC_STATUS_FAIL
} LDC_status;

typedef struct {
	unsigned int *buffer;
	unsigned int length;
} t_index_buffer;

typedef struct {
	dtype distCenterX; 
	dtype distCenterY; 
	dtype distFocalLength;
	dtype distFocalLengthInv;
	dtype *lut_d2u;
	Int32 lut_d2u_indMax;
	dtype lut_d2u_step;
	dtype lut_d2u_stepInv;
	dtype *lut_u2d;
	Int32 lut_u2d_indMax;
	dtype lut_u2d_step;
	dtype lut_u2d_stepInv;
} LensDistortionCorrection;


typedef struct {
	unsigned int *buffer;
	unsigned int length;
} t_index_buffer;

#define MAX_INDEX_BUFFERS 2
t_index_buffer index_buffers[MAX_INDEX_BUFFERS];

float temp_lochor;
float temp_locver;
int32_t calmat[3*4*4];
uint16_t distortionCenters[2 * NUM_CAMERAS];
uint16_t focalLength[NUM_CAMERAS];
uint16_t camWidth;
uint16_t camHeight;

LensDistortionCorrection ldc[NUM_CAMERAS];

glm::mat4 mRT[NUM_CAMERAS];
glm::mat4 mK[NUM_CAMERAS];
glm::mat4 mP[NUM_CAMERAS]; //[P] = [K][RT]
glm::vec3 vTc[NUM_CAMERAS];
glm::vec3 vTw[NUM_CAMERAS];
glm::mat4 mRw[NUM_CAMERAS];
glm::mat4 mRTw[NUM_CAMERAS];

typedef struct _srv_lut_t
{
	GL_VERTEX_DATATYPE x, y, z;
	GL_TEXCOORD_DATATYPE u1, v1, u2, v2;
#ifdef SRV_INTERLEAVED_BLEND
	GL_BLEND_DATATYPE blend1, blend2;
#endif
} srv_lut_t;


typedef struct _srv_blend_lut_t
{
	GL_BLEND_DATATYPE blend1, blend2;
} srv_blend_lut_t;


typedef struct
{
   void * LUT3D;
   void * blendLUT3D;
   void * PALUT3D;
   void * BoxLUT;
   void * BoxPose3D;
   
   int screen_width;
   int screen_height;

   uint32_t cam_width;
   uint32_t cam_height;
   uint32_t cam_bpp;

} render_state_t;

#define LDC_LIB_DATA_TYPE 0 //0: float, (1:double currently not supported since ldc_lensParameters does only support float format)
#define LDC_LIB_U2D_LUT_TYPE 2 //0: theta->rd/ru, 1: ru->rd/ru, 2: theta->rd
#define LDC_LIB_D2U_LUT_TYPE 0 //0: rdSquared->ru/rd
#define LDC_LIB_INTERPOLATION_METHOD 2 //0: 'previous', 1: 'nearest', 2:'linear'

#define LDC_MAX_NUM_CAMERAS			(6)    //maximum number of cameras allowed
#define LDC_U2D_TABLE_MAX_LENGTH	(1024) //maximum u2d table length allowed
#define LDC_D2U_TABLE_MAX_LENGTH	(1024) //maximum d2d table length allowed

typedef struct
{
	Int32 ldcLUT_numCams;
	/**< Num of cameras */
	Int32 ldcLUT_distortionCenters[2*LDC_MAX_NUM_CAMERAS];
	/**< Num of Lens Distortion centres */
	float ldcLUT_focalLength;
	/**< Lens focal length */

	Int32 U2D_type;
	/**< Lens Undistort to Distort type (must match macro LDC_LIB_U2D_LUT_TYPE)*/
	Int32 ldcLUT_U2D_length;
	/**< Lens Undistort to Distort length */
	float ldcLUT_U2D_step;
	/**< Lens Undistort to Distort step */
	float ldcLUT_U2D_table[LDC_U2D_TABLE_MAX_LENGTH];
	/**< Lens Undistort to Distort table */

	Int32 D2U_type;
	/**< Lens Distort to Undistort type (must match macro LDC_LIB_D2U_LUT_TYPE)*/
	Int32 ldcLUT_D2U_length;
	/**< Lens Distort to Undistort length */
	float ldcLUT_D2U_step;
	/**< Lens Distort to Undistort step */
	float ldcLUT_D2U_table[LDC_D2U_TABLE_MAX_LENGTH];
	/**< Lens Distort to Undistort table */
}ldc_lensParameters;


LDC_status LDC_Init(LensDistortionCorrection* ldc,
						   dtype distCenterX, dtype distCenterY, dtype distFocalLength,
						   dtype *lut_d2u, Int32 lut_d2u_length, dtype lut_d2u_step,
						   dtype *lut_u2d, Int32 lut_u2d_length, dtype lut_u2d_step)
{
#if LDC_LIB_DATA_TYPE!=0 && LDC_LIB_DATA_TYPE!=1
	"LDC_LIB_DATA_TYPE must be 0 (float) or 1 (double) in lens_distortion_correction.h"
#endif
	/*FLOATING TYPE*/
	/*distortion center*/
	ldc->distCenterX = distCenterX;
	ldc->distCenterY = distCenterY;
	ldc->distFocalLength = distFocalLength;
	ldc->distFocalLengthInv = 1/ldc->distFocalLength;
	/*ldc look-up table parameters*/
	ldc->lut_d2u_indMax = lut_d2u_length-1;
	ldc->lut_d2u_step = lut_d2u_step;
	ldc->lut_u2d_indMax = lut_u2d_length - 1;
	ldc->lut_u2d_step = lut_u2d_step;

	ldc->lut_d2u_stepInv = 1/ldc->lut_d2u_step;
	ldc->lut_u2d_stepInv = 1/ldc->lut_u2d_step;


	/*ldc look-up table pointers*/
	ldc->lut_d2u = lut_d2u;
	ldc->lut_u2d = lut_u2d;

	return LDC_STATUS_OK;
}


#if LDC_LIB_DATA_TYPE<2 
dtype lut_lookup_floating(dtype *lut, dtype inval, Int32 indMax, dtype stepInv, LDC_status *status)
{
	*status = LDC_STATUS_OK;
	dtype ind = inval * stepInv;
	if (ind >= (dtype)indMax)
	{
		*status = LDC_STATUS_FAIL;
		return lut[indMax];
	}


#if LDC_LIB_INTERPOLATION_METHOD==0
	return lut[(Int32)ind];
#elif LDC_LIB_INTERPOLATION_METHOD==1
	return lut[(Int32)(ind + 0.5)];
#elif LDC_LIB_INTERPOLATION_METHOD==2
	Int32 N = (Int32)ind;
	dtype indMinN = ind - (dtype)N;
	return (1.0f - indMinN)*lut[N] + indMinN * lut[N + 1];
#endif
}
#endif


LDC_status LDC_UndistToDist(LensDistortionCorrection* ldc, dtype point_in[2], dtype point_out[2])
{
#if LDC_LIB_DATA_TYPE!=0 && LDC_LIB_DATA_TYPE!=1
	"LDC_LIB_DATA_TYPE must be 0 (float) or 1 (double) in lens_distortion_correction.h"
#endif
#if LDC_LIB_U2D_LUT_TYPE!=0 && LDC_LIB_U2D_LUT_TYPE!=1 && LDC_LIB_U2D_LUT_TYPE!=2
	"LDC_LIB_U2D_LUT_TYPE must be 0, 1 or 2"
#endif

	LDC_status status;
	dtype diffX, diffY;
	dtype lut_in_val;
	dtype lut_out_val;

	diffX = point_in[0] - ldc->distCenterX;
	diffY = point_in[1] - ldc->distCenterY;

#if LDC_LIB_U2D_LUT_TYPE == 0 || LDC_LIB_U2D_LUT_TYPE == 2
	dtype ru;
	#if LDC_LIB_DATA_TYPE==0 
		ru = sqrtf(diffX*diffX + diffY*diffY);
		lut_in_val = atanf(ru*ldc->distFocalLengthInv);
	#elif LDC_LIB_DATA_TYPE==1
		ru = sqrt(diffX*diffX + diffY*diffY);
		lut_in_val = atan(ru*ldc->distFocalLengthInv);
	#endif
#elif LDC_LIB_U2D_LUT_TYPE == 1
	#if LDC_LIB_DATA_TYPE==0 
		lut_in_val = sqrtf(diffX*diffX + diffY*diffY);
	#elif LDC_LIB_DATA_TYPE==1
		lut_in_val = sqrt(diffX*diffX + diffY*diffY);
	#endif
#endif

	lut_out_val = lut_lookup_floating(ldc->lut_u2d, lut_in_val, ldc->lut_u2d_indMax, ldc->lut_u2d_stepInv, &status);

#if LDC_LIB_U2D_LUT_TYPE == 0 || LDC_LIB_U2D_LUT_TYPE == 1
	point_out[0] = diffX * lut_out_val + ldc->distCenterX;
	point_out[1] = diffY * lut_out_val + ldc->distCenterY;
#elif LDC_LIB_U2D_LUT_TYPE == 2
	if (ru==0)
	{
		point_out[0] = ldc->distCenterX;
		point_out[1] = ldc->distCenterY;
	}
	else
	{
		point_out[0] = lut_out_val * diffX / ru + ldc->distCenterX;
		point_out[1] = lut_out_val * diffY / ru + ldc->distCenterY;
	}
#endif

	return status;
}
/*===============================================================================
*
* Name:        svProjectiveTransform_float()
*
* Description: Transform a given world point according to camera co-ordinates
*
*
* Input:
*   sv:        Handle for the SV structure
*
* Returns:
*
* Effects:
*
/*===============================================================================*/
void svProjectiveTransform_float(uint32_t regmark, float *SV_origin, float X, float Y, float Z)
{
    float x_norm, y_norm;

    glm::vec4 vIn;
    glm::vec4 vOut = glm::vec4(0.0f);
    glm::vec4 vOut1 = glm::vec4(0.0f);
    vIn = glm::vec4(X, Y, Z, 1.0f);//齐次化

    vOut = mP[regmark] * vIn;
    if ( vOut.z < 0)
	    vOut.z *= (-1);
    x_norm = (vOut.x) / (vOut.z);  //归一化
    y_norm = (vOut.y) / (vOut.z);  //归一化
    temp_lochor = x_norm * 16;
    temp_locver = y_norm * 16;
}

/*===============================================================================
*
* Name:        svFisheyeTransform_distort_LDC()
*
* Description: Transform a given point according to the given fisheye parameters
*
*
* Input:
*   sv:        Handle for the SV structure
*
* Returns:
*
* Effects:
*
===============================================================================*/
void svFisheyeTransform_distort_LDC(uint32_t regmark)
{
    dtype point_u[2];
    dtype point_d[2];

    point_u[0] = (dtype)temp_lochor / (dtype)16;
    point_u[1] = (dtype)temp_locver / (dtype)16;

    // Convert pixel location from undistorted to distorted location
    Int32 status = LDC_UndistToDist(&ldc[regmark], point_u, point_d);

    if (status == LDC_STATUS_FAIL)
    {
        printf("WARNING: LDC stepped out of bounds");
    }

    temp_locver = (float)(point_d[1] * 16);
    temp_lochor = (float)(point_d[0] * 16);

    // Repeat pixels if we try to access a pixel outside the camera input image
    if (temp_locver < 0)
        temp_locver = 0;
    if (temp_lochor < 0)
        temp_lochor = 0;
    if (temp_locver >((camHeight - 2) << 4))
        temp_locver = ((camHeight- 2) << 4);
    if (temp_lochor >((camWidth - 2) << 4))
        temp_lochor = ((camWidth - 2) << 4);
}

void lut_add_entry(srv_lut_t *entry,
		float X, float Y, float Z,
		float step_size, float *SV_origin,
		uint32_t cam_id1, uint32_t cam_id2,
		float cornerx, float cornery)
{
    float angle_corner_degrees;
    float angle_point_degrees;
    float angle_offset_degrees; // Angle offset of point from corner line

    entry->x = round(X/step_size); // X co-ordinate
    entry->y = round(Y/step_size); // Y co-ordinate
    entry->z = Z;

    svProjectiveTransform_float(cam_id1, SV_origin, X, Y, Z);
    svFisheyeTransform_distort_LDC(cam_id1);  /* AYK */

    entry->u1 = temp_locver;
    entry->v1 = temp_lochor;

    svProjectiveTransform_float(cam_id2, SV_origin, X, Y, Z);
    svFisheyeTransform_distort_LDC(cam_id2);  /* AYK */

    entry->u2 = temp_locver;
    entry->v2 = temp_lochor;

#ifdef SRV_INTERLEAVED_BLEND
    angle_corner_degrees = atan2(cornery,cornerx) * 180.0/PI;
    if(entry->y == (GL_VERTEX_DATATYPE)0)
    {
        if(entry->x >= 0)
	{
		angle_point_degrees = 0;
	}
	else
	{
		angle_point_degrees = 180;
	}
    }
    else
    {
	angle_point_degrees = atan2((float)entry->y,(float)entry->x) * 180.0/PI;
    }
    angle_offset_degrees = angle_point_degrees - angle_corner_degrees;

    if(angle_offset_degrees > 180.0)
    {
	angle_offset_degrees -= 360.0;
    }
    if(fabsf(angle_offset_degrees) < OVERLAP_REGION_ANGULAR_WIDTH_DEGREES)
    {
        entry->blend1 = 128 - int(127.0 * angle_offset_degrees/OVERLAP_REGION_ANGULAR_WIDTH_DEGREES);
	entry->blend2 = 255 - entry->blend1;
    }
    else if((angle_offset_degrees >= 0.0))
    {
	entry->blend1 = 0;
	entry->blend2 = 255;
    }
    else
    {
	entry->blend1 = 255;
	entry->blend2 = 0;
    }
#endif //SRV_INTERLEAVED_BLEND
}

void blend_lut_add_entry(srv_blend_lut_t *entry, float X, float Y,float cornerx, float cornery)
{
    float angle_corner_degrees;
    float angle_point_degrees;
    float angle_offset_degrees; // Angle offset of point from corner line
    GL_VERTEX_DATATYPE x, y;

    x = round(X); // X co-ordinate
    y = round(Y); // Y co-ordinate

    angle_corner_degrees = atan2(cornery,cornerx) * 180.0/PI;
    if(y == (GL_VERTEX_DATATYPE)0)
    {
        if(x >= 0)
	{
		angle_point_degrees = 0;
	}
	else
	{
		angle_point_degrees = 180;
	}
    }
    else
    {
	angle_point_degrees = atan2((float)y,(float)x) * 180.0/PI;
    }
    angle_offset_degrees = angle_point_degrees - angle_corner_degrees;

    if(angle_offset_degrees > 180.0)
    {
	angle_offset_degrees -= 360.0;
    }
    if(fabsf(angle_offset_degrees) < OVERLAP_REGION_ANGULAR_WIDTH_DEGREES)
    {
        entry->blend1 = 128 - int(127.0 * angle_offset_degrees/OVERLAP_REGION_ANGULAR_WIDTH_DEGREES);
	entry->blend2 = 255 - entry->blend1;
    }
    else if((angle_offset_degrees >= 0.0))
    {
	entry->blend1 = 0;
	entry->blend2 = 255;
    }
    else
    {
	entry->blend1 = 255;
	entry->blend2 = 0;
    }
}

float X1max = 1500, X1min = 1000, Y1max = 500, Y1min = 450, Z1 = 100;
float X2max = -1000, X2min = -1400, Y2max = -300, Y2min = -400, Z2 = 50;

int srv_generate_lut (uint32_t width, uint32_t height,
		uint32_t camw, uint32_t camh,
		uint32_t subx, uint32_t suby, srv_lut_t *lut)
{
    uint16_t i,j,j_offset,i_offset;
    float X,Y,Z;
    uint32_t count=0;
    float maxZ=0;

    #ifndef QUADBASED
    uint16_t ind_i,ind_j;
    //uint32_t img_count=0;
    #else
    uint32_t quad_count1=0, quad_count2=0, quad_count3=0, quad_count4=0;
    uint32_t no_of_elems = 9;
    #endif

    float rel_dist_bowl=0;

    // TBD User parameter .. needs to go to alg_plugin
    // For rectangular base
    int32_t offsetXleft = -400;
    int32_t offsetXright = 400;
    //uint32_t offsetY = 150;
    int32_t offsetYfront = -400;
    int32_t offsetYback = 400;
    int32_t offsetXmin;
    int32_t offsetYmin;

    int32_t doll = 72;
    int32_t jeep = 14;

    uint32_t max_dist=0;

    // Grid cell size in real-world measrements
    float step_size = 5; // in mm
    float SV_origin[2] = {0,0};
    float FC_loc[2] = {0,0};
    float BC_loc[2] = {0,0};
    float cam_distance=0;
    float rshift;
    float tshift;
    srv_lut_t *lut_region;

    // Calmat bin file
    FILE *f_calmatbin;
    //LDC binary file
    FILE *f_ldcbin;
    ldc_lensParameters lp;
    size_t bytes_read;


    rshift = (float)pow(2, CALMAT_R_SHIFT);
    tshift = (float)pow(2, CALMAT_T_SHIFT);

    camHeight = (uint16_t)camh;
    camWidth = (uint16_t)camw;

    //Calibration matrix
    f_calmatbin = fopen(SRV_CALMAT_BIN_FILE, "rb");
    if(!f_calmatbin)
    {
	    fprintf(stderr, "Fatal: Unable to open calibtable binary file: %s\n",
			    SRV_CALMAT_BIN_FILE);
	    return -1;
    }

    // FIXME: This is not the right way. We need to read the header
    fseek(f_calmatbin, 32 * 4, 0);
    bytes_read = fread((void *)&calmat, sizeof(int32_t), 3*4*4, f_calmatbin);
    if(bytes_read <= 0)
    {
	    fprintf(stderr, "Fatal: Cannot read calibration table from: %s\n",
			    SRV_CALMAT_BIN_FILE);
	    fclose(f_calmatbin);
	    return -1;
    }

    for(int i =0;i<48;i++)
    {
    printf("calmat %d is %d\n",i,calmat[i]);
    }
    // Don't need the file anymore
    fclose(f_calmatbin);


    //计算四个相机在标定场地中的世界坐标，计算四个相机包络中心的世界坐标，数值单位为mm
    for(count=0;count<NUM_CAMERAS;count++)
    {
	float xc, yc, zc;
        // -R't to calculate origin (>>20 for both t and R because they are multiplied)

        //SV_origin[0] += - ( ( sv->persmat[0+12*count]*((sv->persmat[9+12*count])>>20) + sv->persmat[1+12*count]*((sv->persmat[10+12*count])>>20) + sv->persmat[2+12*count]*((sv->persmat[11+12*count])>>20) )>>20 )/4;
        //SV_origin[1] += - ( ( sv->persmat[3+12*count]*((sv->persmat[9+12*count])>>20) + sv->persmat[4+12*count]*((sv->persmat[10+12*count])>>20) + sv->persmat[5+12*count]*((sv->persmat[11+12*count])>>20) )>>20 )/4;

        // Typecast to floats

        xc = -(((float)(calmat[0 + 12 * count]) / pow(2, CALMAT_R_SHIFT)) * ((float)(calmat[9 + 12 * count]) / pow(2, CALMAT_T_SHIFT))\
             + ((float)(calmat[1 + 12 * count]) / pow(2, CALMAT_R_SHIFT)) * ((float)(calmat[10 + 12 * count]) / pow(2, CALMAT_T_SHIFT))\
             + ((float)(calmat[2 + 12 * count]) / pow(2, CALMAT_R_SHIFT)) * ((float)(calmat[11 + 12 * count]) / pow(2, CALMAT_T_SHIFT)));

        SV_origin[0] += xc/NUM_CAMERAS;

        yc = -(((float)(calmat[3 + 12 * count]) / pow(2, CALMAT_R_SHIFT)) * ((float)(calmat[9 + 12 * count]) / pow(2, CALMAT_T_SHIFT))\
                               + ((float)(calmat[4 + 12 * count]) / pow(2, CALMAT_R_SHIFT)) * ((float)(calmat[10 + 12 * count]) / pow(2, CALMAT_T_SHIFT))\
                               + ((float)(calmat[5 + 12 * count]) / pow(2, CALMAT_R_SHIFT)) * ((float)(calmat[11 + 12 * count]) / pow(2, CALMAT_T_SHIFT)));

        SV_origin[1] += yc/NUM_CAMERAS;

        zc = -(((float)(calmat[6 + 12 * count]) / pow(2, CALMAT_R_SHIFT)) * ((float)(calmat[9 + 12 * count]) / pow(2, CALMAT_T_SHIFT))\
                               + ((float)(calmat[7 + 12 * count]) / pow(2, CALMAT_R_SHIFT)) * ((float)(calmat[10 + 12 * count]) / pow(2, CALMAT_T_SHIFT))\
                               + ((float)(calmat[8 + 12 * count]) / pow(2, CALMAT_R_SHIFT)) * ((float)(calmat[11 + 12 * count]) / pow(2, CALMAT_T_SHIFT)));


        printf("cal camera %d position is %f %f %f\n",i,xc,yc,zc);

        vTc[count] = glm::vec3(xc, yc, zc);

        printf("SV_Origin :%f\t%f\n", SV_origin[0], SV_origin[1]);
    }

    //Load LDC parameters from lens binary
    f_ldcbin = fopen(SRV_LDC_LENS_BIN_FILE, "rb");
    if(!f_ldcbin)
    {
	    fprintf(stderr, "Fatal: Unable to open lens parameters binary file: %s\n",
			    SRV_LDC_LENS_BIN_FILE);
	    return -1;
    }
    bytes_read = fread((void *)&lp, sizeof(ldc_lensParameters), 1, f_ldcbin);
    if(bytes_read <= 0)
    {
	    fprintf(stderr, "Fatal: Cannot read lens parameters from: %s\n",
			    SRV_LDC_LENS_BIN_FILE);
            fclose(f_ldcbin);
	    return -1;
    }
    
    printf("ldc_lut_num_cams is %d\n",lp.ldcLUT_numCams);

    // Don't need the file anymore
    fclose(f_ldcbin);

    for(j = 0; j < lp.ldcLUT_numCams; j++)
    {
	    focalLength[j] = lp.ldcLUT_focalLength;
	    distortionCenters[2*j] = (uint16_t)lp.ldcLUT_distortionCenters[2*j + 1];//相机畸变中心-x
	    distortionCenters[2*j + 1] = (uint16_t)lp.ldcLUT_distortionCenters[2*j];//相机畸变中心-y
	    //Initialize LDC
	    LDC_Init(&ldc[j], (dtype)lp.ldcLUT_distortionCenters[2 * j], (dtype)lp.ldcLUT_distortionCenters[2 * j + 1], lp.ldcLUT_focalLength,
			    NULL, 0, 0,
			    lp.ldcLUT_U2D_table, lp.ldcLUT_U2D_length, lp.ldcLUT_U2D_step);
	    mRT[j] = glm::mat4((float)(calmat[j * 12 + 0])/rshift, (float)(calmat[j * 12 + 1])/rshift, (float)(calmat[j * 12 + 2])/rshift, 0.0f,
			               (float)(calmat[j * 12 + 3])/rshift, (float)(calmat[j * 12 + 4])/rshift, (float)(calmat[j * 12 + 5])/rshift, 0.0f,
			               (float)(calmat[j * 12 + 6])/rshift, (float)(calmat[j * 12 + 7])/rshift, (float)(calmat[j * 12 + 8])/rshift, 0.0f,
			               (float)(calmat[j * 12 + 9])/tshift, (float)(calmat[j * 12 + 10])/tshift, (float)(calmat[j * 12 + 11])/tshift, 1.0f);
                           
	    mRT[j] = glm::translate(mRT[j], glm::vec3((float) SV_origin[0], (float)SV_origin[1], 0.0f));
	    mK[j] = glm::mat4((float)(focalLength[j]), 0.0f, 0.0f, 0.0f,
			       0.0f, (float)(focalLength[j]), 0.0f, 0.0f,
			       (float)distortionCenters[2 * j + 1], (float)distortionCenters[2 * j], 1.0f, 0.0f,
			       0.0f, 0.0f, 0.0f, 1.0f);
	    mP[j] = mK[j] * mRT[j];

    }

    float * mat_data;
    for(int i=0;i<4;i++)
    {
        mat_data = &mP[0][0][0];
        printf("mp %d is :\n",i);
        for(int j= 0;j<16;j=j+4)
        {
        printf(" %f %f %f %f \n",mat_data[j],mat_data[j+1],mat_data[j+2],mat_data[j+3]);
        }
    }

    for(int i=0;i<4;i++)
    {
        mat_data = &mK[0][0][0];
        printf("mK %d is :\n",i);
        for(int j= 0;j<16;j=j+4)
        {
        printf(" %f %f %f %f \n",mat_data[j],mat_data[j+1],mat_data[j+2],mat_data[j+3]);
        }
    }


    for(int i=0;i<4;i++)
    {
        mat_data = &mRT[0][0][0];
        printf("mRT %d is :\n",i);
        for(int j= 0;j<16;j=j+4)
        {
        printf(" %f %f %f %f \n",mat_data[j],mat_data[j+1],mat_data[j+2],mat_data[j+3]);
        }
    }

	printf("Initialized mRT\n");

    // Calculate step_size from calmat (scale correction)

        // Front camera
        count =0;
        // Typecast to floats
        FC_loc[0] = -(((float)(calmat[0 + 12 * count]) / pow(2, CALMAT_R_SHIFT)) * ((float)(calmat[9 + 12 * count]) / pow(2, CALMAT_T_SHIFT))\
                               + ((float)(calmat[1 + 12 * count]) / pow(2, CALMAT_R_SHIFT)) * ((float)(calmat[10 + 12 * count]) / pow(2, CALMAT_T_SHIFT))\
                               + ((float)(calmat[2 + 12 * count]) / pow(2, CALMAT_R_SHIFT)) * ((float)(calmat[11 + 12 * count]) / pow(2, CALMAT_T_SHIFT)));

        FC_loc[1] = -(((float)(calmat[3 + 12 * count]) / pow(2, CALMAT_R_SHIFT)) * ((float)(calmat[9 + 12 * count]) / pow(2, CALMAT_T_SHIFT))\
                               + ((float)(calmat[4 + 12 * count]) / pow(2, CALMAT_R_SHIFT)) * ((float)(calmat[10 + 12 * count]) / pow(2, CALMAT_T_SHIFT))\
                               + ((float)(calmat[5 + 12 * count]) / pow(2, CALMAT_R_SHIFT)) * ((float)(calmat[11 + 12 * count]) / pow(2, CALMAT_T_SHIFT)));


        // Back camera
        count=2;
        // Typecast to floats
        BC_loc[0] = -(((float)(calmat[0 + 12 * count]) / pow(2, CALMAT_R_SHIFT)) * ((float)(calmat[9 + 12 * count]) / pow(2, CALMAT_T_SHIFT))\
                               + ((float)(calmat[1 + 12 * count]) / pow(2, CALMAT_R_SHIFT)) * ((float)(calmat[10 + 12 * count]) / pow(2, CALMAT_T_SHIFT))\
                               + ((float)(calmat[2 + 12 * count]) / pow(2, CALMAT_R_SHIFT)) * ((float)(calmat[11 + 12 * count]) / pow(2, CALMAT_T_SHIFT)));

        BC_loc[1] = -(((float)(calmat[3 + 12 * count]) / pow(2, CALMAT_R_SHIFT)) * ((float)(calmat[9 + 12 * count]) / pow(2, CALMAT_T_SHIFT))\
                               + ((float)(calmat[4 + 12 * count]) / pow(2, CALMAT_R_SHIFT)) * ((float)(calmat[10 + 12 * count]) / pow(2, CALMAT_T_SHIFT))\
                               + ((float)(calmat[5 + 12 * count]) / pow(2, CALMAT_R_SHIFT)) * ((float)(calmat[11 + 12 * count]) / pow(2, CALMAT_T_SHIFT)));


// Front to back camera distance gives scale of car

    cam_distance = sqrt( (FC_loc[0]-BC_loc[0])*(FC_loc[0]-BC_loc[0]) + (FC_loc[1]-BC_loc[1])*(FC_loc[1]-BC_loc[1]) );//前后摄像头距离

    printf("cam_distance is %f\n",cam_distance);
    // Step_size divides the length of the vehicle into hundred grid points. (100 can be user defined parameter)
    step_size = cam_distance/100;

    #ifdef FLATWORDVIEW
    maxZ = 0; //in mm
    #else
   // maxZ = 30 * 150*step_size; //in mm
   // Z最大值是 X,Y的1.5倍
    maxZ = 150*step_size; //in mm
    #endif

    for(j = 0; j < NUM_CAMERAS; j++)
    {
	    glm::mat4 mTw;
	    mRw[j] = glm::mat4(glm::mat3(glm::transpose(mRT[j])));
	    vTw[j] = (vTc[j] - glm::vec3((float)SV_origin[0], (float)SV_origin[1], 0.0f))/(float)step_size;
	    mTw = glm::translate(glm::mat4(1.0f),vTw[j]);
	    mRTw[j] = mTw * mRw[j];
    }

    //打印 mRTw   
    for(int i=0;i<4;i++)
    {
        mat_data = &mRTw[0][0][0];
        printf("mRTw %d is :\n",i);
        for(int j= 0;j<16;j=j+4)
        {
        printf(" %f %f %f %f \n",mat_data[j],mat_data[j+1],mat_data[j+2],mat_data[j+3]);
        }
    }

    // Max dist set to diagonal
    //max_dist = sqrt( ((height-offsetX)*(height-offsetX)) + ((width-offsetY)*(width-offsetY)) );
    max_dist = sqrt( ((height/2)*(height/2)) + ((width/2)*(width/2)) );

    /*

    view id 1 is Dominant Image
    view id 2 is Secondary Image


                                    view0
                        |-----------------------|
                        | Quadrant1 | Quadrant2 |
                    view3   |-----------------------| view1
                                | Quadrant4|  Quadrant3 |
                        |-----------------------|
                                view2


    GALUT format:

    For each output grid vertex the followind are stored in the array:

    0 - O/p X co-ordinate
    1 - O/p Y co-ordinate
    2 - O/p Z co-ordinate
    3 - Image 1 x co-ordinate (Dominant view)
    4 - Image 1 y co-ordinate (Dominant view)
    5 - Image 2 x co-ordinate
    6 - Image 2 y co-ordinate
    7 - View id 1 (Dominant view)
    8 - View id 2


    */

    quad_count1 = 0;//0*no_of_elems *( (1+width/2/subx) * (1+height/2/suby));
    quad_count2 = 0;//1*no_of_elems *( (1+width/2/subx) * (1+height/2/suby));
    quad_count3 = 0;//2*no_of_elems *( (1+width/2/subx) * (1+height/2/suby));
    quad_count4 = 0;//3*no_of_elems *( (1+width/2/subx) * (1+height/2/suby));

    offsetXmin = min(offsetXright, offsetXleft);
    offsetYmin = min(offsetYfront, offsetYback);

    rel_dist_bowl = 0;


    for (i=0; i<height; i+=suby){
        for (j=0; j<width; j+=subx){

#ifdef QNX_BUILD
            j_offset = abs(int(j - width/2));
            i_offset = abs(int(i - height/2));
#else
            j_offset = abs(j - width/2);
            i_offset = abs(i - height/2);
#endif
            int32_t j_dist = j - width/2;
            int32_t i_dist = i - height/2;

            rel_dist_bowl = 0;


    //        offsetYfront = -(jeep * 10 + floor(cam_distance/2))/step_size;
    //        offsetYback  = (doll * 10 + floor(cam_distance/2))/step_size;

            // Bowl with rectangular base (rel_dist for bowl z height)
            // Max distance updatesd for non-uniform sampling
	    if(i_dist <= offsetYfront)
	    {
		    if(j_dist >= offsetXright)
		    {
			    rel_dist_bowl = sqrt((i_dist-offsetYfront)*(i_dist-offsetYfront) + (j_dist-offsetXright)*(j_dist-offsetXright));
			    rel_dist_bowl = (rel_dist_bowl/(max_dist-( sqrt(offsetYmin*offsetYmin+offsetXmin*offsetXmin))));
		    }
		    else if(j_dist <= offsetXleft)
		    {
			    rel_dist_bowl = sqrt((i_dist-offsetYfront)*(i_dist-offsetYfront) + (j_dist-offsetXleft)*(j_dist-offsetXleft));
			    rel_dist_bowl = (rel_dist_bowl/(max_dist-( sqrt(offsetYmin*offsetYmin+offsetXmin*offsetXmin))));

		    }
		    else
		    {
			    rel_dist_bowl = abs(i_dist-offsetYfront);
			    rel_dist_bowl = (rel_dist_bowl/(max_dist-( sqrt(offsetYmin*offsetYmin+offsetXmin*offsetXmin))));
		    }
	    }

	    else if(i_dist >= offsetYback)
	    {
		    if(j_dist >= offsetXright)
		    {
			    rel_dist_bowl = sqrt((i_dist-offsetYback)*(i_dist-offsetYback) + (j_dist-offsetXright)*(j_dist-offsetXright));
			    rel_dist_bowl = (rel_dist_bowl/(max_dist-( sqrt(offsetYmin*offsetYmin+offsetXmin*offsetXmin))));
		    }
		    else if(j_dist <= offsetXleft)
		    {
			    rel_dist_bowl = sqrt((i_dist-offsetYback)*(i_dist-offsetYback) + (j_dist-offsetXleft)*(j_dist-offsetXleft));
			    rel_dist_bowl = (rel_dist_bowl/(max_dist-( sqrt(offsetYmin*offsetYmin+offsetXmin*offsetXmin))));

		    }
		    else
		    {
			    rel_dist_bowl = abs(i_dist-offsetYback);
			    rel_dist_bowl = (rel_dist_bowl/(max_dist-( sqrt(offsetYmin*offsetYmin+offsetXmin*offsetXmin))));
		    }
	    }
	    else
	    {
		    if(j_dist >= offsetXright)
		    {
			    rel_dist_bowl = abs(j_dist-offsetXright);
			    rel_dist_bowl = (rel_dist_bowl/(max_dist-( sqrt(offsetYmin*offsetYmin+offsetXmin*offsetXmin))));
		    }
		    else if(j_dist <= offsetXleft)
		    {
			    rel_dist_bowl = abs(j_dist-offsetXleft);
			    rel_dist_bowl = (rel_dist_bowl/(max_dist-( sqrt(offsetYmin*offsetYmin+offsetXmin*offsetXmin))));
		    }
		    else
		    {
			    rel_dist_bowl = 0;
		    }

	    }



#if 0

            if(i_offset > offsetY && j_offset > offsetX)
            {
                  rel_dist_bowl = sqrt((i_offset-offsetY)*(i_offset-offsetY) + (j_offset-offsetX)*(j_offset-offsetX));
                  rel_dist_bowl = (rel_dist_bowl/(max_dist-( sqrt(offsetY*offsetY+offsetX*offsetX))));
            }
            else if (i_offset <= offsetY && j_offset > offsetX)
            {
                  rel_dist_bowl = (j_offset-offsetX);
                  rel_dist_bowl = (rel_dist_bowl/(max_dist-( sqrt(offsetY*offsetY+offsetX*offsetX))));
            }
            else if (i_offset > offsetY && j_offset <= offsetX)
            {
                  rel_dist_bowl = (i_offset-offsetY);
                  rel_dist_bowl = (rel_dist_bowl/(max_dist-( sqrt(offsetY*offsetY+offsetX*offsetX))));
            }
            else
            {
                  rel_dist_bowl=0;
            }
#endif
                // Calculate World points
                X = (float)(((float)j - (float)width/2.0) * (float)step_size);
                Y = (float)(((float)i - (float)height/2.0) * (float)step_size);
                Z = (float)(maxZ*((rel_dist_bowl)));
                //Z=0;

            Y=-Y;

#if 1
	    if((X <= X1max) && (X >= X1min) && (Y <= Y1max) && (Y >= Y1min))
	    {
		    Z = Z1;
	    }

	    if((X <= X2max) && (X >= X2min) && (Y <= Y2max) && (Y >= Y2min))
	    {
		    Z = Z2;
	    }
#endif

        // Quadrant 1
        if (j<=width/2 && i<=height/2) {
		lut_region = &lut[0 * QUADRANT_SIZE + quad_count1];
		lut_add_entry(lut_region, X, Y, Z,
			step_size, SV_origin,
			0, 3, 0 - ((float)width)/2, ((float)height)/2);
		quad_count1++;
	}

        // Quadrant 2
        if (j>=(width/2-subx) && i<=height/2) {
		lut_region = &lut[1 * QUADRANT_SIZE + quad_count2];
		lut_add_entry(lut_region, X, Y, Z,
			step_size, SV_origin,
			1, 0, ((float)width)/2, ((float)height)/2);
		quad_count2++;
	}

        // Quadrant 3
        if (j>=(width/2-subx) && i>=(height/2-suby)) {
		lut_region = &lut[2 * QUADRANT_SIZE + quad_count3];
		lut_add_entry(lut_region, X, Y, Z,
			step_size, SV_origin,
			2, 1, ((float)width)/2, 0 - ((float)height)/2);
		quad_count3++;
	}

        // Quadrant 4
        if (j<=width/2 && i>=(height/2-suby)){
		lut_region = &lut[3 * QUADRANT_SIZE + quad_count4];
		lut_add_entry(lut_region, X, Y, Z,
			step_size, SV_origin,
			3, 2, 0 - ((float)width)/2, 0 - ((float)height)/2);
		quad_count4++;
	}

        }

    }

    #ifdef PC_VERSION
        //Test
        fp = fopen("GAlingLUT3D.txt","w");
        count = ( ( (2+width/subsampleratio)*(2+height/subsampleratio) ) ) *9;
        for(index=0; index<count; index+=1){
                    fprintf(fp, " %d,", lut[index]);
        }
        fclose(fp);
    #endif
}


int srv_static_lut_import(uint32_t lut_size, srv_lut_t *LUT_TOPVIEW)
{
    FILE *f_static_lut_file;
    size_t bytes_read;

    //read static load_file
    f_static_lut_file = fopen("./srv_lut.bin","rb");

    if(!f_static_lut_file)
    {
        printf("statice lut file read failed!!!\n");
        return -1;
    }
    else
    {
        printf("open static lut finished !!!\n");
    }

    //bytes_read = fread((void*)&LUT_TOPVIEW, sizeof(_srv_lut_t), lut_size, f_static_lut_file);
    //_srv_lut_t *data_get;
    //LUT_TOPVIEW = (_srv_lut_t*)malloc(sizeof(_srv_lut_t)*72900);
    bytes_read = fread(LUT_TOPVIEW,sizeof(_srv_lut_t),72900,f_static_lut_file);

    if(bytes_read<=0)
    {
        printf("read static lut failed !!!\n");
        return -1;
    }
    else
    {
        for(int i =0;i<=10;i++)
        {
        printf("static data %d is %d %d %d %d %d %d %d",i,LUT_TOPVIEW[i].x,LUT_TOPVIEW[i].y,LUT_TOPVIEW[i].z,LUT_TOPVIEW[i].u1,LUT_TOPVIEW[i].v1,LUT_TOPVIEW[i].u2,LUT_TOPVIEW[i].v2);
        printf("\n");
        }
        return 1;
        fclose(f_static_lut_file);
    }

}


int srv_generate_blend_lut (uint32_t width, uint32_t height,
		uint32_t subx, uint32_t suby, srv_blend_lut_t *lut)
{
    uint16_t i,j;
    float X,Y;
    uint32_t count=0;

    #ifndef QUADBASED
    uint16_t ind_i,ind_j;
    //uint32_t img_count=0;
    #else
    uint32_t quad_count1=0, quad_count2=0, quad_count3=0, quad_count4=0;
    #endif

    srv_blend_lut_t *lut_region;

    /*

    view id 1 is Dominant Image
    view id 2 is Secondary Image


                                view0
                        |-----------------------|
                        | Quadrant1 | Quadrant2 |
                view3   |-----------------------| view1
                        | Quadrant4|  Quadrant3 |
                        |-----------------------|
                                view2


    Blend LUT format:

    For each output grid vertex the followind are stored in the array:

    0 - Blend 1
    1 - Blend 2


    */

    quad_count1 = 0;//0*no_of_elems *( (1+width/2/subx) * (1+height/2/suby));
    quad_count2 = 0;//1*no_of_elems *( (1+width/2/subx) * (1+height/2/suby));
    quad_count3 = 0;//2*no_of_elems *( (1+width/2/subx) * (1+height/2/suby));
    quad_count4 = 0;//3*no_of_elems *( (1+width/2/subx) * (1+height/2/suby));

    for (i=0; i<height; i+=suby){
        for (j=0; j<width; j+=subx){

		// Calculate World points
		X = (float)(((float)j - (float)width/2.0));
		Y = (float)(((float)i - (float)height/2.0));
		Y=-Y;

        // Quadrant 1
        if (j<=width/2 && i<=height/2) {
		lut_region = &lut[0 * QUADRANT_SIZE + quad_count1];
		blend_lut_add_entry(lut_region, X, Y,
			0 - ((float)width)/2, ((float)height)/2);
		quad_count1++;
	}
        // Quadrant 2
        if (j>=(width/2-subx) && i<=height/2) {
		lut_region = &lut[1 * QUADRANT_SIZE + quad_count2];
		blend_lut_add_entry(lut_region, X, Y,
			((float)width)/2, ((float)height)/2);
		quad_count2++;
	}

        // Quadrant 3
        if (j>=(width/2-subx) && i>=(height/2-suby)) {
		lut_region = &lut[2 * QUADRANT_SIZE + quad_count3];
		blend_lut_add_entry(lut_region, X, Y,
			((float)width)/2, 0 - ((float)height)/2);
		quad_count3++;
	}

        // Quadrant 4
        if (j<=width/2 && i>=(height/2-suby)){
		lut_region = &lut[3 * QUADRANT_SIZE + quad_count4];
		blend_lut_add_entry(lut_region, X, Y,
			0 - ((float)width)/2, 0 - ((float)height)/2);
		quad_count4++;
	}

        }

    }
}

int srv_static_blend_lut (uint32_t blend_lut_size,
		srv_blend_lut_t *blend_lut)
{

    FILE *f_static_blend_lut;
    size_t bytes_read;

    //read static load_file
    f_static_blend_lut = fopen(SRV_STATIC_BLEND_LUT_BIN_FILE,"rb");
    if(!f_static_blend_lut)
    {
        printf("statice blend lut file read failed!!!\n");
        return -1;
    }
    else
    {
        printf("open static blend lut finished !!!\n");
    }

    bytes_read = fread(blend_lut, sizeof(srv_blend_lut_t), blend_lut_size, f_static_blend_lut);
    if(bytes_read<=0)
    {
        printf("read static lut failed !!!\n");
        return -1;
    }
    else
    {
        for(int i =0;i<=10;i++)
        {
        printf("static blend data %d is %d %d\n",blend_lut[i].blend1,blend_lut[i].blend2); 
        }
        return 1;
        fclose(f_static_blend_lut);
    }

}

void generate_indices(t_index_buffer *index_buffer, unsigned int xlength, unsigned int ylength, unsigned int gap)
{
	unsigned int *buffer = index_buffer->buffer;
	unsigned int x, y, k=0;
	for (y=0; y<ylength-gap; y+=gap)
	{
		if(y>0)
			buffer[k++]=(unsigned int) (y*xlength);
		for (x=0; x<xlength; x+=gap)
		{
			buffer[k++]=(unsigned int) (y*xlength + x);
			buffer[k++]=(unsigned int) ((y+gap)*xlength + x);
		}
		if(y < ylength - 1 - gap)
			buffer[k++]=(unsigned int) ((y+gap)*xlength + (xlength -1));
	}
	index_buffer->length = k;
}


int static_indices_import(t_index_buffer* index_buffer,size_t indice_length)
{
    FILE *f_static_indices;
    size_t bytes_read;
	index_buffer->length =indice_length;

    //read static load_file
    f_static_indices = fopen(SRV_STATIC_INDICES,"rb");
    if(!f_static_indices)
    {
        printf("statice blend lut file read failed!!!\n");
        return -1;
    }
    else
    {
        printf("open static blend lut finished !!!\n");
    }
    bytes_read = fread(index_buffer->buffer, sizeof(unsigned int), indice_length, f_static_indices);

    if(bytes_read<=0)
    {
        printf("read static lut failed !!!\n");
        return -1;
    }
    else
    {
        for(int i =0;i<=30;i=i+3)
        {
        printf("Triganle data %d is %d %d %d \n",i,index_buffer->buffer[i],index_buffer->buffer[i+1],index_buffer->buffer[i+2]); 
        }
        return 1;
        fclose(f_static_indices);
    }
}

int main(void)
{


    printf("generator srv_lut there!!!!!!\n");
    // srv_generate_blend_lut (uint32_t width, uint32_t height,
	//	uint32_t subx, uint32_t suby, srv_blend_lut_t *lut) 
    render_state_t srv_renderObj;
    render_state_t *pObj;
    pObj = &srv_renderObj;

    /* if(1)//pObj->LUT3D == NULL)
	{
    pObj ->LUT3D = malloc(sizeof(int16_t) * (POINTS_WIDTH/POINTS_SUBX) * (POINTS_HEIGHT/POINTS_SUBY) * 9 * 2);
    pObj ->cam_width =1280;
    pObj ->cam_height =720;
    //srv_generate_lut(
    //                 POINTS_WIDTH, 
    //                 POINTS_HEIGHT,
	//			       pObj->cam_width,
	//			       pObj->cam_height,
	//			       POINTS_SUBX,
	//			       POINTS_SUBY,
	//			       (srv_lut_t *)pObj->LUT3D
    //                 ); 


    //pObj ->LUT3D = malloc(sizeof(_srv_lut_t) * 72900);
    srv_static_lut_import(72900,(_srv_lut_t *)pObj->LUT3D);

    printf("generator srv_lut there!!!!!!\n");
    }
    if(1)//pObj->blendLUT3D == NULL)
	{
		// Generate the BlendLUT if it wasn't passed
		pObj->blendLUT3D = malloc(sizeof(int16_t) * (POINTS_WIDTH/POINTS_SUBX) * (POINTS_HEIGHT/POINTS_SUBY) * 9 * 2);
		srv_generate_blend_lut(POINTS_WIDTH, POINTS_HEIGHT,
				POINTS_SUBX,
				POINTS_SUBY,
				(srv_blend_lut_t *)pObj->blendLUT3D);

    printf("generator srv_lut_blend there!!!!!!\n");
	}

    t_index_buffer index_buffer;

    index_buffer.buffer = (unsigned int *)malloc(10 * 10* 3 * sizeof(unsigned int));

    generate_indices(&index_buffer,100, 100, 2);

    for(int i=0;i<200;i+=3)
    {
        printf("indices data is %d %d %d\n",index_buffer.buffer[i],index_buffer.buffer[i+1],index_buffer.buffer[i+2]);
    }*/
    	if(pObj->LUT3D == NULL)
	{
		// Generate the LUT if it wasn't passed
		/* pObj->LUT3D = malloc(sizeof(int16_t) * (POINTS_WIDTH/POINTS_SUBX) * (POINTS_HEIGHT/POINTS_SUBY) * 9 * 2);
		srv_generate_lut(POINTS_WIDTH, POINTS_HEIGHT,
				pObj->cam_width,
				pObj->cam_height,
				POINTS_SUBX,
				POINTS_SUBY,
				(srv_lut_t *)pObj->LUT3D); */
	    pObj->LUT3D = malloc(sizeof(int16_t) * QUADRANT_WIDTH * QUADRANT_HEIGHT * 7 * 4);
		srv_static_lut_import(QUADRANT_WIDTH * QUADRANT_HEIGHT*4, (srv_lut_t *)pObj->LUT3D);
	}

	if(pObj->blendLUT3D == NULL)
	{
		// Generate the BlendLUT if it wasn't passed
		/*pObj->blendLUT3D = malloc(sizeof(int16_t) * (POINTS_WIDTH/POINTS_SUBX) * (POINTS_HEIGHT/POINTS_SUBY) * 9 * 2);
		 srv_generate_blend_lut(POINTS_WIDTH, POINTS_HEIGHT,
				POINTS_SUBX,
				POINTS_SUBY,
				(srv_blend_lut_t *)pObj->blendLUT3D);*/
		pObj->blendLUT3D = malloc(sizeof(unsigned char) * QUADRANT_WIDTH * QUADRANT_HEIGHT * 2 * 4);
		srv_static_blend_lut(QUADRANT_WIDTH * QUADRANT_HEIGHT*4,(srv_blend_lut_t*)pObj->blendLUT3D);
	}

    for (int i = 0; i < MAX_INDEX_BUFFERS; i++)
	{
		index_buffers[i].buffer = (unsigned int *)malloc((QUADRANT_WIDTH-1) * (QUADRANT_HEIGHT-1) * 3 * 2*sizeof(unsigned int));
		static_indices_import((t_index_buffer*)&index_buffers[i],(QUADRANT_WIDTH-1) * (QUADRANT_HEIGHT-1) * 3* 2 );
	}
    return 0;
}