
/*
 *GUI_DEMO_AA.c
 *2019年1月16日下午8:41:57
 *
 */

#include <stdint.h>
#include <string.h>

#include "emXGUI.h"
#include "emXGUI_Arch.h"

#if 1
/*============================================================================*/
//定义控件ID
#define	ID_OK		0x1000

/*============================================================================*/
#define	LV_ANTIALIAS 1
#define LV_COLOR_DEPTH     16                     /*Color depth: 1/8/16/24*/


#define LV_MATH_MIN(a,b) (a<b?a:b)
#define LV_MATH_MAX(a,b) (a>b?a:b)
#define LV_MATH_ABS(x) ((x)>0?(x):(-(x)))

#define CIRCLE_AA_NON_LINEAR_OPA_THRESHOLD  5   /*Circle segment greater then this value will be anti-aliased by a non-linear (cos) opacity mapping*/

#define LINE_WIDTH_CORR_BASE 64
#define LINE_WIDTH_CORR_SHIFT 6

#define SHADOW_OPA_EXTRA_PRECISION      8       /*Calculate with 2^x bigger shadow opacity values to avoid rounding errors*/
#define SHADOW_BOTTOM_AA_EXTRA_RADIUS   3       /*Add extra radius with LV_SHADOW_BOTTOM to cover anti-aliased corners*/

#define LV_COLOR_BLACK   LV_COLOR_MAKE(0x00,0x00,0x00)
#define LV_COLOR_WHITE   LV_COLOR_MAKE(0xFF,0xFF,0xFF)
#define LV_COLOR_RED     LV_COLOR_MAKE(0xFF,0x00,0x00)
#define LV_COLOR_LIME    LV_COLOR_MAKE(0x00,0xFF,0x00)
#define LV_COLOR_BLUE    LV_COLOR_MAKE(0x00,0x00,0xFF)
#define LV_COLOR_YELLOW  LV_COLOR_MAKE(0xFF,0xFF,0x00)
#define LV_COLOR_CYAN    LV_COLOR_MAKE(0x00,0xFF,0xFF)
#define LV_COLOR_AQUA    LV_COLOR_CYAN
#define LV_COLOR_MAGENTA LV_COLOR_MAKE(0xFF,0x00,0xFF)
#define LV_COLOR_SILVER  LV_COLOR_MAKE(0xC0,0xC0,0xC0)
#define LV_COLOR_GRAY    LV_COLOR_MAKE(0x80,0x80,0x80)
#define LV_COLOR_MARRON  LV_COLOR_MAKE(0x80,0x00,0x00)
#define LV_COLOR_OLIVE   LV_COLOR_MAKE(0x80,0x80,0x00)
#define LV_COLOR_GREEN   LV_COLOR_MAKE(0x00,0x80,0x00)
#define LV_COLOR_PURPLE  LV_COLOR_MAKE(0x80,0x00,0x80)
#define LV_COLOR_TEAL    LV_COLOR_MAKE(0x00,0x80,0x80)
#define LV_COLOR_NAVY    LV_COLOR_MAKE(0x00,0x00,0x80)
#define LV_COLOR_ORANGE  LV_COLOR_MAKE(0xFF,0xA5,0x00)

#define LV_OPA_TRANSP    0
#define LV_OPA_0         0
#define LV_OPA_10        25
#define LV_OPA_20        51
#define LV_OPA_30        76
#define LV_OPA_40        102
#define LV_OPA_50        127
#define LV_OPA_60        153
#define LV_OPA_70        178
#define LV_OPA_80        204
#define LV_OPA_90        229
#define LV_OPA_100       255
#define LV_OPA_COVER     255

#define LV_CIRC_OCT1_X(p) (p.x)
#define LV_CIRC_OCT1_Y(p) (p.y)
#define LV_CIRC_OCT2_X(p) (p.y)
#define LV_CIRC_OCT2_Y(p) (p.x)
#define LV_CIRC_OCT3_X(p) (-p.y)
#define LV_CIRC_OCT3_Y(p) (p.x)
#define LV_CIRC_OCT4_X(p) (-p.x)
#define LV_CIRC_OCT4_Y(p) (p.y)
#define LV_CIRC_OCT5_X(p) (-p.x)
#define LV_CIRC_OCT5_Y(p) (-p.y)
#define LV_CIRC_OCT6_X(p) (-p.y)
#define LV_CIRC_OCT6_Y(p) (-p.x)
#define LV_CIRC_OCT7_X(p) (p.y)
#define LV_CIRC_OCT7_Y(p) (-p.x)
#define LV_CIRC_OCT8_X(p) (p.x)
#define LV_CIRC_OCT8_Y(p) (-p.y)

#define	true  1
#define	false 0

typedef union
{
    struct
    {
        uint16_t blue  :5;
        uint16_t green :6;
        uint16_t red   :5;
    };
    uint16_t full;
}lv_color16_t;

typedef	int bool;
typedef int16_t lv_coord_t;
typedef uint8_t lv_opa_t;
typedef lv_color16_t lv_color_t;

typedef struct
{
    lv_coord_t x;
    lv_coord_t y;
}lv_point_t;

typedef struct
{
    lv_coord_t x1;
    lv_coord_t y1;
    lv_coord_t x2;
    lv_coord_t y2;
}lv_area_t;


/*Border types (Use 'OR'ed values)*/
typedef enum
{
    LV_BORDER_NONE =   0x00,
    LV_BORDER_BOTTOM = 0x01,
    LV_BORDER_TOP =    0x02,
    LV_BORDER_LEFT =   0x04,
    LV_BORDER_RIGHT =  0x08,
    LV_BORDER_FULL =   0x0F,
}lv_border_part_t;

/*Shadow types*/
typedef enum
{
    LV_SHADOW_BOTTOM = 0,
    LV_SHADOW_FULL,
}lv_shadow_type_t;

typedef struct
{
    uint8_t glass :1;   /*1: Do not inherit this style*/

    struct {
        lv_color_t main_color;
        lv_color_t grad_color;
        lv_coord_t radius;
        lv_opa_t opa;

        struct {
            lv_color_t color;
            lv_coord_t width;
            lv_border_part_t part;
            lv_opa_t opa;
        }border;

        struct {
            lv_color_t color;
            lv_coord_t width;
            uint8_t type;
        }shadow;

        struct {
            lv_coord_t ver;
            lv_coord_t hor;
            lv_coord_t inner;
        }padding;

        uint8_t empty :1;   /*Transparent background (border still drawn)*/
    }body;


    struct {
        lv_color_t color;
 //       const lv_font_t * font;
        lv_coord_t letter_space;
        lv_coord_t line_space;
        lv_opa_t opa;
    }text;

    struct {
        lv_color_t color;
        lv_opa_t intense;
        lv_opa_t opa;
    }image;

    struct {
        lv_color_t color;
        lv_coord_t width;
        lv_opa_t opa;
    }line;
}lv_style_t;

typedef struct
{
    lv_area_t area;
    lv_color_t *buf;
}lv_vdb_t;

/*============================================================================*/

#define	VLCD_W 320
#define VLCD_H 240

static lv_vdb_t vdb;
//static u16 vlcd_fb[VLCD_W*VLCD_H];
static u16 *vlcd_fb=NULL;

lv_vdb_t* lv_vdb_get(void)
{
	vdb.buf =(lv_color_t*)vlcd_fb;
	vdb.area.x1 =0;
	vdb.area.y1 =0;
	vdb.area.x2 =(VLCD_W-1);
	vdb.area.y2 =(VLCD_H-1);
	return &vdb;
}

/*============================================================================*/

/**
 * Initialize an area
 * @param area_p pointer to an area
 * @param x1 left coordinate of the area
 * @param y1 top coordinate of the area
 * @param x2 right coordinate of the area
 * @param y2 bottom coordinate of the area
 */
void lv_area_set(lv_area_t * area_p, lv_coord_t x1, lv_coord_t y1, lv_coord_t x2, lv_coord_t y2);

/**
 * Copy an area
 * @param dest pointer to the destination area
 * @param src pointer to the source area
 */
inline static void lv_area_copy(lv_area_t * dest, const lv_area_t * src)
{
    memcpy(dest, src, sizeof(lv_area_t));
}

/*
* Initialize an area
* @param area_p pointer to an area
* @param x1 left coordinate of the area
* @param y1 top coordinate of the area
* @param x2 right coordinate of the area
* @param y2 bottom coordinate of the area
*/
void lv_area_set(lv_area_t * area_p, lv_coord_t x1, lv_coord_t y1, lv_coord_t x2, lv_coord_t y2)
{
   area_p->x1 = x1;
   area_p->y1 = y1;
   area_p->x2 = x2;
   area_p->y2 = y2;
}

/**
 * Get the width of an area
 * @param area_p pointer to an area
 * @return the width of the area (if x1 == x2 -> width = 1)
 */
static inline lv_coord_t lv_area_get_width(const lv_area_t * area_p)
{
    return area_p->x2 - area_p->x1 + 1;
}

/**
 * Get the height of an area
 * @param area_p pointer to an area
 * @return the height of the area (if y1 == y2 -> height = 1)
 */
static inline lv_coord_t lv_area_get_height(const lv_area_t * area_p)
{
    return area_p->y2 - area_p->y1 + 1;
}

/**
 * Get the common parts of two areas
 * @param res_p pointer to an area, the result will be stored her
 * @param a1_p pointer to the first area
 * @param a2_p pointer to the second area
 * @return false: the two area has NO common parts, res_p is invalid
 */
bool lv_area_union(lv_area_t * res_p, const lv_area_t * a1_p, const lv_area_t * a2_p)
{
    /* Get the smaller area from 'a1_p' and 'a2_p' */
    res_p->x1 = LV_MATH_MAX(a1_p->x1, a2_p->x1);
    res_p->y1 = LV_MATH_MAX(a1_p->y1, a2_p->y1);
    res_p->x2 = LV_MATH_MIN(a1_p->x2, a2_p->x2);
    res_p->y2 = LV_MATH_MIN(a1_p->y2, a2_p->y2);

    /*If x1 or y1 greater then x2 or y2 then the areas union is empty*/
    bool union_ok = true;
    if((res_p->x1 > res_p->x2) ||
       (res_p->y1 > res_p->y2))
    {
        union_ok = false;
    }

    return union_ok;
}

static inline lv_color_t lv_color_mix(lv_color_t c1, lv_color_t c2, uint8_t mix)
{
    lv_color_t ret;
#if LV_COLOR_DEPTH != 1
    ret.red =   (uint16_t)((uint16_t) c1.red * mix + (c2.red * (255 - mix))) >> 8;
    ret.green = (uint16_t)((uint16_t) c1.green * mix + (c2.green * (255 - mix))) >> 8;
    ret.blue =  (uint16_t)((uint16_t) c1.blue * mix + (c2.blue * (255 - mix))) >> 8;
# if LV_COLOR_DEPTH == 24
    ret.alpha = 0xFF;
# endif
#else
    ret.full = mix > LV_OPA_50 ? c1.full : c2.full;
#endif

    return ret;
}
/**
 *
 * @param mem_area coordinates of 'mem' memory area
 * @param mem a memory address. Considered to a rectangular window according to 'mem_area'
 * @param fill_area coordinates of an area to fill. Relative to 'mem_area'.
 * @param color fill color
 * @param opa opacity (0, LV_OPA_TRANSP: transparent ... 255, LV_OPA_COVER, fully cover)
 */
static void sw_color_fill(lv_area_t * mem_area, lv_color_t * mem, const lv_area_t * fill_area, lv_color_t color, lv_opa_t opa)
{

    /*Set all row in vdb to the given color*/
    lv_coord_t row;
    lv_coord_t col;
    lv_coord_t mem_width = lv_area_get_width(mem_area);

    /*Run simpler function without opacity*/
    if(opa == LV_OPA_COVER)
    {
        /*Fill the first row with 'color'*/
        for(col = fill_area->x1; col <= fill_area->x2; col++)
        {
            mem[col] = color;
        }

        /*Copy the first row to all other rows*/
        lv_color_t * mem_first = &mem[fill_area->x1];
        lv_coord_t copy_size =  (fill_area->x2 - fill_area->x1 + 1) * sizeof(lv_color_t);
        mem += mem_width;

        for(row = fill_area->y1 + 1; row <= fill_area->y2; row++)
        {
            memcpy(&mem[fill_area->x1], mem_first, copy_size);
            mem += mem_width;
        }
    }
    /*Calculate with alpha too*/
    else
    {
        lv_color_t bg_tmp ={.full=0,};//LV_COLOR_BLACK;
        lv_color_t opa_tmp = lv_color_mix(color, bg_tmp, opa);
        for(row = fill_area->y1; row <= fill_area->y2; row++)
        {
            for(col = fill_area->x1; col <= fill_area->x2; col++)
            {
                /*If the bg color changed recalculate the result color*/
                if(mem[col].full != bg_tmp.full)
                {
                    bg_tmp = mem[col];
                    opa_tmp = lv_color_mix(color, bg_tmp, opa);
                }
                mem[col] = opa_tmp;
            }
            mem += mem_width;
        }
    }
}
/**
 * Put a pixel in the Virtual Display Buffer
 * @param x pixel x coordinate
 * @param y pixel y coordinate
 * @param mask_p fill only on this mask (truncated to VDB area)
 * @param color pixel color
 * @param opa opacity of the area (0..255)
 */
void lv_vpx(lv_coord_t x, lv_coord_t y, const lv_area_t * mask_p, lv_color_t color, lv_opa_t opa)
{
    lv_vdb_t * vdb_p = lv_vdb_get();

    /*Pixel out of the mask*/
    if(x < mask_p->x1 || x > mask_p->x2 ||
       y < mask_p->y1 || y > mask_p->y2) {
        return;
    }

    uint32_t vdb_width = lv_area_get_width(&vdb_p->area);

    /*Make the coordinates relative to VDB*/
    x-=vdb_p->area.x1;
    y-=vdb_p->area.y1;
    lv_color_t * vdb_px_p = vdb_p->buf + y * vdb_width + x;
    if(opa == LV_OPA_COVER) {
        *vdb_px_p = color;
    }
    else {
        *vdb_px_p = lv_color_mix(color,*vdb_px_p, opa);
    }

}


/**
 * Fill an area in the Virtual Display Buffer
 * @param cords_p coordinates of the area to fill
 * @param mask_p fill only o this mask  (truncated to VDB area)
 * @param color fill color
 * @param opa opacity of the area (0..255)
 */
void lv_vfill(const lv_area_t * cords_p, const lv_area_t * mask_p,lv_color_t color, lv_opa_t opa)
{
    lv_area_t res_a;
    bool union_ok;
    lv_vdb_t * vdb_p = lv_vdb_get();

    /*Get the union of cord and mask*/
    /* The mask is already truncated to the vdb size
     * in 'lv_refr_area_with_vdb' function */
    union_ok = lv_area_union(&res_a, cords_p, mask_p);

    /*If there are common part of the three area then draw to the vdb*/
    if(union_ok == false) return;

    lv_area_t vdb_rel_a;   /*Stores relative coordinates on vdb*/
    vdb_rel_a.x1 = res_a.x1 - vdb_p->area.x1;
    vdb_rel_a.y1 = res_a.y1 - vdb_p->area.y1;
    vdb_rel_a.x2 = res_a.x2 - vdb_p->area.x1;
    vdb_rel_a.y2 = res_a.y2 - vdb_p->area.y1;

    lv_color_t * vdb_buf_tmp = vdb_p->buf;
    uint32_t vdb_width = lv_area_get_width(&vdb_p->area);
    /*Move the vdb_tmp to the first row*/
    vdb_buf_tmp += vdb_width * vdb_rel_a.y1;


#if USE_LV_GPU
    static lv_color_t color_array_tmp[LV_HOR_RES];       /*Used by 'lv_disp_mem_blend'*/
    static lv_coord_t last_width = -1;

    lv_coord_t w = lv_area_get_width(&vdb_rel_a);
    /*Don't use hw. acc. for every small fill (because of the init overhead)*/
    if(w < VFILL_HW_ACC_SIZE_LIMIT) {
        sw_color_fill(&vdb_p->area, vdb_buf_tmp, &vdb_rel_a, color, opa);
    }
    /*Not opaque fill*/
    else if(opa == LV_OPA_COVER) {
        /*Use hw fill if present*/
        if(lv_disp_is_mem_fill_supported()) {
            lv_coord_t row;
            for(row = vdb_rel_a.y1;row <= vdb_rel_a.y2; row++) {
                lv_disp_mem_fill(&vdb_buf_tmp[vdb_rel_a.x1], w, color);
                vdb_buf_tmp += vdb_width;
            }
        }
        /*Use hw blend if present and the area is not too small*/
        else if(lv_area_get_height(&vdb_rel_a) > VFILL_HW_ACC_SIZE_LIMIT &&
                lv_disp_is_mem_blend_supported())
        {
            /*Fill a  one line sized buffer with a color and blend this later*/
            if(color_array_tmp[0].full != color.full || last_width != w) {
                uint16_t i;
                for(i = 0; i < w; i++) {
                    color_array_tmp[i].full = color.full;
                }
                last_width = w;
            }

            /*Blend the filled line to every line VDB line-by-line*/
            lv_coord_t row;
            for(row = vdb_rel_a.y1;row <= vdb_rel_a.y2; row++) {
                lv_disp_mem_blend(&vdb_buf_tmp[vdb_rel_a.x1], color_array_tmp, w, opa);
                vdb_buf_tmp += vdb_width;
            }

        }
        /*Else use sw fill if no better option*/
        else {
            sw_color_fill(&vdb_p->area, vdb_buf_tmp, &vdb_rel_a, color, opa);
        }

    }
    /*Fill with opacity*/
    else {
    	/*Use hw blend if present*/
        if(lv_disp_is_mem_blend_supported()) {
            if(color_array_tmp[0].full != color.full || last_width != w) {
                uint16_t i;
                for(i = 0; i < w; i++) {
                    color_array_tmp[i].full = color.full;
                }

                last_width = w;
            }
            lv_coord_t row;
            for(row = vdb_rel_a.y1;row <= vdb_rel_a.y2; row++) {
                lv_disp_mem_blend(&vdb_buf_tmp[vdb_rel_a.x1], color_array_tmp, w, opa);
                vdb_buf_tmp += vdb_width;
            }

        }
        /*Use sw fill with opa if no better option*/
        else {
            sw_color_fill(&vdb_p->area, vdb_buf_tmp, &vdb_rel_a, color, opa);
        }

    }
#else
    sw_color_fill(&vdb_p->area, vdb_buf_tmp, &vdb_rel_a, color, opa);
#endif
}

static void (*px_fp)(lv_coord_t x, lv_coord_t y, const lv_area_t * mask, lv_color_t color, lv_opa_t opa) = lv_vpx;
static void (*fill_fp)(const lv_area_t * coords, const lv_area_t * mask, lv_color_t color, lv_opa_t opa) =  lv_vfill;
//static void (*letter_fp)(const lv_point_t * pos_p, const lv_area_t * mask, const lv_font_t * font_p, uint32_t letter, lv_color_t color, lv_opa_t opa) = lv_vletter;


/*============================================================================*/
#if LV_ANTIALIAS != 0
static lv_opa_t antialias_get_opa(lv_coord_t seg, lv_coord_t px_id, lv_opa_t line_opa)
{
    /* How to calculate the opacity of pixels on the edges which makes the anti-aliasing?
     * For example we have a line like this (y = -0.5 * x):
     *
     *  | _ _
     *    * * |
     *
     * Anti-aliased pixels come to the '*' characters
     * Calculate what percentage of the pixels should be covered if real line (not rasterized) would be drawn:
     * 1. A real line should start on (0;0) and end on (2;1)
     * 2. So the line intersection coordinates on the first pixel: (0;0) (1;0.5) -> 25% covered pixel in average
     * 3. For the second pixel: (1;0.5) (2;1) -> 75% covered pixel in average
     * 4. The equation: (px_id * 2 + 1) / (segment_width * 2)
     *                   segment_width: the line segment which is being anti-aliased (was 2 in the example)
     *                   px_id: pixel ID from 0 to  (segment_width - 1)
     *                   result: [0..1] coverage of the pixel
     */

    /*Accelerate the common segment sizes to avoid division*/
    static const  lv_opa_t seg1[1] = {128};
    static const  lv_opa_t seg2[2] = {64, 192};
    static const  lv_opa_t seg3[3] = {42, 128, 212};
    static const  lv_opa_t seg4[4] = {32, 96, 159, 223};
    static const  lv_opa_t seg5[5] = {26,  76, 128, 178, 230};
    static const  lv_opa_t seg6[6] = {21,  64, 106, 148, 191, 234};
    static const  lv_opa_t seg7[7] = {18,  55, 91, 128, 164, 200, 237};
    static const  lv_opa_t seg8[8] = {16,  48, 80, 112, 143, 175, 207, 239};

    static const lv_opa_t * seg_map[] = {seg1, seg2, seg3, seg4,
                                         seg5, seg6, seg7, seg8};

    if(seg == 0)
    {
    	return LV_OPA_TRANSP;
    }
    else if(seg < 8)
    {
    	return (uint32_t)((uint32_t)seg_map[seg - 1][px_id] * line_opa) >> 8;
    }
    else
    {
        return ((px_id * 2 + 1) * line_opa) / (2 * seg);
    }

}

/**
 * Approximate the opacity for anti-aliasing.
 * Used  the first segment of a circle which is the longest and have the most non-linearity (cos)
 * @param seg
 * @param px_id
 * @param line_opa
 * @return
 */
static lv_opa_t antialias_get_opa_circ(lv_coord_t seg, lv_coord_t px_id, lv_opa_t opa)
{
    static const  lv_opa_t opa_map[8] = {250, 242, 221, 196, 163, 122, 74, 18};

    if(seg == 0)
    {
    	return LV_OPA_TRANSP;
    }
    else if(seg == 1)
    {
    	return LV_OPA_80;
    }
    else
    {

        uint8_t id = (uint32_t) ((uint32_t)px_id * (sizeof(opa_map) - 1)) / (seg - 1);
        return (uint32_t) ((uint32_t) opa_map[id] * opa) >> 8;

    }

}

#endif

static uint16_t lv_draw_cont_radius_corr(uint16_t r, lv_coord_t w, lv_coord_t h)
{
	if(r >= (w >> 1)){
		r = (w >> 1);
		if(r != 0) r--;
	}
	if(r >= (h >> 1)) {
		r = (h >> 1);
		if(r != 0) r--;
	}

    if(r > 0) r -= LV_ANTIALIAS;

	return r;
}

/**
 * Initialize the circle drawing
 * @param c pointer to a point. The coordinates will be calculated here
 * @param tmp point to a variable. It will store temporary data
 * @param radius radius of the circle
 */
void lv_circ_init(lv_point_t * c, lv_coord_t * tmp, lv_coord_t radius)
{
    c->x = radius;
    c->y = 0;
    *tmp = 1 - radius;
}

/**
 * Test the circle drawing is ready or not
 * @param c same as in circ_init
 * @return true if the circle is not ready yet
 */
bool lv_circ_cont(lv_point_t * c)
{
    return c->y <= c->x ? true : false;
}

/**
 * Get the next point from the circle
 * @param c same as in circ_init. The next point stored here.
 * @param tmp same as in circ_init.
 */
void lv_circ_next(lv_point_t * c, lv_coord_t * tmp)
{
    c->y++;

    if (*tmp <= 0) {
       (*tmp) += 2 * c->y + 1;   // Change in decision criterion for y -> y+1
    } else {
      c->x--;
      (*tmp) += 2 * (c->y - c->x) + 1;   // Change for y -> y+1, x -> x-1
    }
}

/*============================================================================*/
/**
 * Draw a line
 * @param p1 first point of the line
 * @param p2 second point of the line
 * @param maskthe line will be drawn only on this area
 * @param lines_p pointer to a line style
 */
void lv_draw_line(const lv_point_t * point1, const lv_point_t * point2, const lv_area_t * mask,
                  const lv_style_t * style)
{

    if(style->line.width == 0)
    {
    	return;
    }

    if(point1->x == point2->x && point1->y == point2->y)
    {
    	return;
    }

    lv_point_t p1;
    lv_point_t p2;

    /*Be sure always x1 < x2*/
    if(point1->x < point2->x)
    {
        p1.x = point1->x;
        p1.y = point1->y;
        p2.x = point2->x;
        p2.y = point2->y;
    }
    else
    {
        p1.x = point2->x;
        p1.y = point2->y;
        p2.x = point1->x;
        p2.y = point1->y;
    }

    lv_coord_t dx = LV_MATH_ABS(p2.x - p1.x);
    lv_coord_t sx = p1.x < p2.x ? 1 : -1;
    lv_coord_t dy = LV_MATH_ABS(p2.y - p1.y);
    lv_coord_t sy = p1.y < p2.y ? 1 : -1;
    lv_coord_t err = (dx > dy ? dx : -dy) / 2;
    lv_coord_t e2;
    bool hor = dx > dy ? true : false;	/*Rather horizontal or vertical*/
    lv_coord_t last_x = p1.x;
    lv_coord_t last_y = p1.y;
    lv_point_t act_point;
    act_point.x = p1.x;
    act_point.y = p1.y;


    lv_coord_t width;
    uint16_t wcor;
    uint16_t width_half = 0;
    uint16_t width_1 = 0;
    static const uint8_t width_corr_array[] = {
            64, 64, 64, 64, 64, 64, 64, 64, 64, 65, 65, 65, 65, 65, 66, 66, 66, 66, 66,
            67, 67, 67, 68, 68, 68, 69, 69, 69, 70, 70, 71, 71, 72, 72, 72, 73, 73, 74,
            74, 75, 75, 76, 77, 77, 78, 78, 79, 79, 80, 81, 81, 82, 82, 83, 84, 84, 85,
            86, 86, 87, 88, 88, 89, 90, 91,
    };

    if(hor == false)
    {
        wcor = (dx * LINE_WIDTH_CORR_BASE) / dy;
    }
    else
    {
        wcor = (dy * LINE_WIDTH_CORR_BASE) / dx;
    }


    width = style->line.width - 1;

#if LV_ANTIALIAS != 0
    bool aa_invert = false;
    aa_invert = p1.y < p2.y ? false : true;      /*Direction of opacity increase on the edges*/
    if(p1.x != p2.x && p1.y != p2.y) width--;    /*Because of anti aliasing (no anti aliasing on hor. and ver. lines)*/
#endif

    /*Make the correction on lie width*/
    if(width > 0)
    {
        width = (width * width_corr_array[wcor]);
        width = width >> LINE_WIDTH_CORR_SHIFT;
        width_half = width >> 1;
        width_1 = width & 0x1 ? 1 : 0;
    }

    /*Special case draw a horizontal line*/
    if(p1.y == p2.y ) {
        lv_area_t act_area;
        act_area.x1 = p1.x;
        act_area.x2 = p2.x;
        act_area.y1 = p1.y - width_half - width_1;
        act_area.y2 = p2.y + width_half ;

        lv_area_t draw_area;
        draw_area.x1 = LV_MATH_MIN(act_area.x1, act_area.x2);
        draw_area.x2 = LV_MATH_MAX(act_area.x1, act_area.x2);
        draw_area.y1 = LV_MATH_MIN(act_area.y1, act_area.y2);
        draw_area.y2 = LV_MATH_MAX(act_area.y1, act_area.y2);
        fill_fp(&draw_area, mask, style->line.color, style->line.opa);

        return;
    }

    /*Special case draw a vertical line*/
    if(p1.x == p2.x ) {
        lv_area_t act_area;
        act_area.x1 = p1.x - width_half;
        act_area.x2 = p2.x + width_half + width_1;
        act_area.y1 = p1.y;
        act_area.y2 = p2.y;

        lv_area_t draw_area;
        draw_area.x1 = LV_MATH_MIN(act_area.x1, act_area.x2);
        draw_area.x2 = LV_MATH_MAX(act_area.x1, act_area.x2);
        draw_area.y1 = LV_MATH_MIN(act_area.y1, act_area.y2);
        draw_area.y2 = LV_MATH_MAX(act_area.y1, act_area.y2);
        fill_fp(&draw_area, mask, style->line.color, style->line.opa);
        return;
    }


    while(1)
    {
        if(hor == true && last_y != act_point.y)
        {
            lv_area_t act_area;
            lv_area_t draw_area;
            act_area.x1 = last_x;
            act_area.x2 = act_point.x - sx;
            act_area.y1 = last_y - width_half ;
            act_area.y2 = act_point.y - sy  + width_half + width_1;
            last_y = act_point.y;
            last_x = act_point.x;
            draw_area.x1 = LV_MATH_MIN(act_area.x1, act_area.x2);
            draw_area.x2 = LV_MATH_MAX(act_area.x1, act_area.x2);
            draw_area.y1 = LV_MATH_MIN(act_area.y1, act_area.y2);
            draw_area.y2 = LV_MATH_MAX(act_area.y1, act_area.y2);
            if(width >= 0)
            {
                fill_fp(&draw_area, mask, style->line.color, style->line.opa);
            }

#if LV_ANTIALIAS != 0
            lv_coord_t seg_w = lv_area_get_width(&draw_area);  /*Segment width*/
            lv_point_t aa_p1;
            lv_point_t aa_p2;

            aa_p1.x = draw_area.x1;
            aa_p1.y = draw_area.y1 - 1;

            aa_p2.x = draw_area.x1;
            aa_p2.y = draw_area.y1 + width + 1;

            lv_coord_t i;
            for(i = 0; i  < seg_w; i++)
            {
                lv_opa_t aa_opa = antialias_get_opa(seg_w, i, style->line.opa);

                px_fp(aa_p1.x + i, aa_p1.y, mask, style->line.color, aa_invert ? aa_opa : style->line.opa - aa_opa);
                px_fp(aa_p2.x + i, aa_p2.y, mask, style->line.color, aa_invert ? style->line.opa - aa_opa :  aa_opa);
            }
#endif
        }
        if (hor == false && last_x != act_point.x)
        {
            lv_area_t act_area;
            lv_area_t draw_area;
            act_area.x1 = last_x - width_half;
            act_area.x2 = act_point.x  - sx + width_half + width_1;
            act_area.y1 = last_y ;
            act_area.y2 = act_point.y - sy;
            last_y = act_point.y;
            last_x = act_point.x;

            draw_area.x1 = LV_MATH_MIN(act_area.x1, act_area.x2);
            draw_area.x2 = LV_MATH_MAX(act_area.x1, act_area.x2);
            draw_area.y1 = LV_MATH_MIN(act_area.y1, act_area.y2);
            draw_area.y2 = LV_MATH_MAX(act_area.y1, act_area.y2);
            if(width >= 0)
            {
                fill_fp(&draw_area, mask, style->line.color, style->line.opa);
            }

#if LV_ANTIALIAS != 0
            lv_coord_t seg_h = lv_area_get_height(&draw_area);  /*Segment height*/
            lv_point_t aa_p1;
            lv_point_t aa_p2;

            aa_p1.x = draw_area.x1 - 1;
            aa_p1.y = draw_area.y1;

            aa_p2.x = draw_area.x1 + width + 1;
            aa_p2.y = draw_area.y1;

            lv_coord_t i;
            for(i = 0; i  < seg_h; i++)
            {
                lv_opa_t aa_opa = antialias_get_opa(seg_h, i, style->line.opa);

                px_fp(aa_p1.x, aa_p1.y + i, mask, style->line.color, aa_invert ? aa_opa : style->line.opa - aa_opa);
                px_fp(aa_p2.x, aa_p2.y + i, mask, style->line.color, aa_invert ? style->line.opa - aa_opa :  aa_opa);
            }
#endif
        }

        /*Calc. the next point of the line*/
        if (act_point.x == p2.x && act_point.y == p2.y) break;
        e2 = err;
        if (e2 >-dx) {
            err -= dy;
            act_point.x += sx;
        }
        if (e2 < dy) {
            err += dx;
            act_point.y += sy;
        }
    }

    /*Draw the last part of the line*/
    if(hor == true) {
        lv_area_t act_area;
        lv_area_t draw_area;
        act_area.x1 = last_x;
        act_area.x2 = act_point.x;
        act_area.y1 = last_y - width_half ;
        act_area.y2 = act_point.y + width_half + width_1;

        draw_area.x1 = LV_MATH_MIN(act_area.x1, act_area.x2);
        draw_area.x2 = LV_MATH_MAX(act_area.x1, act_area.x2);
        draw_area.y1 = LV_MATH_MIN(act_area.y1, act_area.y2);
        draw_area.y2 = LV_MATH_MAX(act_area.y1, act_area.y2);
        if(width >= 0) {
            fill_fp(&draw_area, mask, style->line.color, style->line.opa);
        }

#if LV_ANTIALIAS != 0
        lv_coord_t seg_w = lv_area_get_width(&draw_area);  /*Segment width*/
        lv_point_t aa_p1;
        lv_point_t aa_p2;

        aa_p1.x = draw_area.x1;
        aa_p1.y = draw_area.y1 - 1;

        aa_p2.x = draw_area.x1;
        aa_p2.y = draw_area.y1 + width + 1;

        lv_coord_t i;
        for(i = 0; i  < seg_w; i++) {
            lv_opa_t aa_opa = antialias_get_opa(seg_w, i, style->line.opa);

            px_fp(aa_p1.x + i, aa_p1.y, mask, style->line.color, aa_invert ? aa_opa : style->line.opa - aa_opa);
            px_fp(aa_p2.x + i, aa_p2.y, mask, style->line.color, aa_invert ? style->line.opa - aa_opa :  aa_opa);
        }
#endif

    }
    if (hor == false) {
        lv_area_t act_area;
        lv_area_t draw_area;
        act_area.x1 = last_x - width_half;
        act_area.x2 = act_point.x + width_half + width_1;
        act_area.y1 = last_y;
        act_area.y2 = act_point.y;

        draw_area.x1 = LV_MATH_MIN(act_area.x1, act_area.x2);
        draw_area.x2 = LV_MATH_MAX(act_area.x1, act_area.x2);
        draw_area.y1 = LV_MATH_MIN(act_area.y1, act_area.y2);
        draw_area.y2 = LV_MATH_MAX(act_area.y1, act_area.y2);
        if(width >= 0) {
            fill_fp(&draw_area, mask, style->line.color, style->line.opa);
        }

#if LV_ANTIALIAS != 0
        lv_coord_t seg_h = lv_area_get_height(&draw_area);  /*Segment height*/
        lv_point_t aa_p1;
        lv_point_t aa_p2;

        aa_p1.x = draw_area.x1 - 1;
        aa_p1.y = draw_area.y1;

        aa_p2.x = draw_area.x1 + width + 1;
        aa_p2.y = draw_area.y1;

        lv_coord_t i;
        for(i = 0; i  < seg_h; i++) {
            lv_opa_t aa_opa = antialias_get_opa(seg_h, i, style->line.opa);

            px_fp(aa_p1.x, aa_p1.y + i, mask, style->line.color, aa_invert ? aa_opa : style->line.opa - aa_opa);
            px_fp(aa_p2.x, aa_p2.y + i, mask, style->line.color, aa_invert ? style->line.opa - aa_opa :  aa_opa);
        }
#endif
    }
}


/**
 * Draw the middle part (rectangular) of a rectangle
 * @param coords the coordinates of the original rectangle
 * @param mask the rectangle will be drawn only  on this area
 * @param rects_p pointer to a rectangle style
 */
static void lv_draw_rect_main_mid(const lv_area_t * coords, const lv_area_t * mask, const lv_style_t * style)
{
    uint16_t radius = style->body.radius;

    lv_color_t mcolor = style->body.main_color;
    lv_color_t gcolor = style->body.grad_color;
    uint8_t mix;
    lv_opa_t opa = style->body.opa;
    lv_coord_t height = lv_area_get_height(coords);
    lv_coord_t width = lv_area_get_width(coords);

    radius = lv_draw_cont_radius_corr(radius, width, height);

    /*If the radius is too big then there is no body*/
    if(radius > height / 2)  return;

	lv_area_t work_area;
	work_area.x1 = coords->x1;
	work_area.x2 = coords->x2;

    if(mcolor.full == gcolor.full) {
    	work_area.y1 = coords->y1 + radius;
    	work_area.y2 = coords->y2 - radius;

        if(style->body.radius != 0) {
#if LV_ANTIALIAS
            work_area.y1 += 2;
            work_area.y2 -= 2;
#else
            work_area.y1 += 1;
            work_area.y2 -= 1;
#endif
        }

		fill_fp(&work_area, mask, mcolor, opa);
    } else {
		lv_coord_t row;
		lv_coord_t row_start = coords->y1 + radius;
		lv_coord_t row_end = coords->y2 - radius;
		lv_color_t act_color;

        if(style->body.radius != 0) {
#if LV_ANTIALIAS
		    row_start += 2;
            row_end -= 2;
#else
            row_start += 1;
            row_end -= 1;
#endif
		}
        if(row_start < 0) row_start = 0;

		for(row = row_start; row <= row_end; row ++)
		{
			work_area.y1 = row;
			work_area.y2 = row;
			mix = (uint32_t)((uint32_t)(coords->y2 - work_area.y1) * 255) / height;
			act_color = lv_color_mix(mcolor, gcolor, mix);

			fill_fp(&work_area, mask, act_color, opa);
		}
    }
}

/**
 * Draw the top and bottom parts (corners) of a rectangle
 * @param coords the coordinates of the original rectangle
 * @param mask the rectangle will be drawn only  on this area
 * @param rects_p pointer to a rectangle style
 */
static void lv_draw_rect_main_corner(const lv_area_t * coords, const lv_area_t * mask, const lv_style_t * style)
{
    uint16_t radius = style->body.radius;

    lv_color_t mcolor = style->body.main_color;
    lv_color_t gcolor = style->body.grad_color;
    lv_color_t act_color;
    lv_opa_t opa = style->body.opa;
    uint8_t mix;
    lv_coord_t height = lv_area_get_height(coords);
    lv_coord_t width = lv_area_get_width(coords);

    radius = lv_draw_cont_radius_corr(radius, width, height);

    lv_point_t lt_origo;   /*Left  Top    origo*/
    lv_point_t lb_origo;   /*Left  Bottom origo*/
    lv_point_t rt_origo;   /*Right Top    origo*/
    lv_point_t rb_origo;   /*Left  Bottom origo*/

    lt_origo.x = coords->x1 + radius + LV_ANTIALIAS;
    lt_origo.y = coords->y1 + radius + LV_ANTIALIAS;

    lb_origo.x = coords->x1 + radius + LV_ANTIALIAS;
    lb_origo.y = coords->y2 - radius - LV_ANTIALIAS;

    rt_origo.x = coords->x2 - radius - LV_ANTIALIAS;
    rt_origo.y = coords->y1 + radius + LV_ANTIALIAS;

    rb_origo.x = coords->x2 - radius - LV_ANTIALIAS;
    rb_origo.y = coords->y2 - radius - LV_ANTIALIAS;

    lv_area_t edge_top_area;
    lv_area_t mid_top_area;
    lv_area_t mid_bot_area;
    lv_area_t edge_bot_area;

    lv_point_t cir;
    lv_coord_t cir_tmp;
    lv_circ_init(&cir, &cir_tmp, radius);

    /*Init the areas*/
    lv_area_set(&mid_bot_area,  lb_origo.x + LV_CIRC_OCT4_X(cir),
                             lb_origo.y + LV_CIRC_OCT4_Y(cir),
                             rb_origo.x + LV_CIRC_OCT1_X(cir),
                             rb_origo.y + LV_CIRC_OCT1_Y(cir));

    lv_area_set(&edge_bot_area, lb_origo.x + LV_CIRC_OCT3_X(cir),
                             lb_origo.y + LV_CIRC_OCT3_Y(cir),
                             rb_origo.x + LV_CIRC_OCT2_X(cir),
                             rb_origo.y + LV_CIRC_OCT2_Y(cir));

    lv_area_set(&mid_top_area,  lt_origo.x + LV_CIRC_OCT5_X(cir),
                             lt_origo.y + LV_CIRC_OCT5_Y(cir),
                             rt_origo.x + LV_CIRC_OCT8_X(cir),
                             rt_origo.y + LV_CIRC_OCT8_Y(cir));

    lv_area_set(&edge_top_area, lt_origo.x + LV_CIRC_OCT6_X(cir),
                             lt_origo.y + LV_CIRC_OCT6_Y(cir),
                             rt_origo.x + LV_CIRC_OCT7_X(cir),
                             rt_origo.y + LV_CIRC_OCT7_Y(cir));
#if LV_ANTIALIAS
    /*Store some internal states for anti-aliasing*/
    lv_coord_t out_y_seg_start = 0;
    lv_coord_t out_y_seg_end = 0;
    lv_coord_t out_x_last = radius;

    lv_color_t aa_color_hor_top;
    lv_color_t aa_color_hor_bottom;
    lv_color_t aa_color_ver;
#endif

    while(lv_circ_cont(&cir)) {
#if LV_ANTIALIAS != 0
        /*New step in y on the outter circle*/
        if(out_x_last != cir.x) {
            out_y_seg_end = cir.y;
            lv_coord_t seg_size = out_y_seg_end - out_y_seg_start;
            lv_point_t aa_p;

            aa_p.x = out_x_last;
            aa_p.y = out_y_seg_start;

            mix = (uint32_t)((uint32_t)(radius - out_x_last) * 255) / height;
            aa_color_hor_top = lv_color_mix(gcolor, mcolor, mix);
            aa_color_hor_bottom = lv_color_mix(mcolor, gcolor, mix);

            lv_coord_t i;
            for(i = 0; i  < seg_size; i++) {
                lv_opa_t aa_opa;
                if(seg_size > CIRCLE_AA_NON_LINEAR_OPA_THRESHOLD) {    /*Use non-linear opa mapping on the first segment*/
                    aa_opa = antialias_get_opa_circ(seg_size, i, style->body.opa);
                } else {
                    aa_opa = opa - antialias_get_opa(seg_size, i, style->body.opa);
                }

                px_fp(rb_origo.x + LV_CIRC_OCT2_X(aa_p) + i, rb_origo.y + LV_CIRC_OCT2_Y(aa_p) + 1, mask, aa_color_hor_bottom, aa_opa);
                px_fp(lb_origo.x + LV_CIRC_OCT3_X(aa_p) - i, lb_origo.y + LV_CIRC_OCT3_Y(aa_p) + 1, mask, aa_color_hor_bottom, aa_opa);
                px_fp(lt_origo.x + LV_CIRC_OCT6_X(aa_p) - i, lt_origo.y + LV_CIRC_OCT6_Y(aa_p) - 1, mask, aa_color_hor_top, aa_opa);
                px_fp(rt_origo.x + LV_CIRC_OCT7_X(aa_p) + i, rt_origo.y + LV_CIRC_OCT7_Y(aa_p) - 1, mask, aa_color_hor_top, aa_opa);

                mix = (uint32_t)((uint32_t)(radius - out_y_seg_start + i) * 255) / height;
                aa_color_ver = lv_color_mix(mcolor, gcolor, mix);
                px_fp(rb_origo.x + LV_CIRC_OCT1_X(aa_p) + 1, rb_origo.y + LV_CIRC_OCT1_Y(aa_p) + i, mask, aa_color_ver, aa_opa);
                px_fp(lb_origo.x + LV_CIRC_OCT4_X(aa_p) - 1, lb_origo.y + LV_CIRC_OCT4_Y(aa_p) + i, mask, aa_color_ver, aa_opa);

                aa_color_ver = lv_color_mix(gcolor, mcolor, mix);
                px_fp(lt_origo.x + LV_CIRC_OCT5_X(aa_p) - 1, lt_origo.y + LV_CIRC_OCT5_Y(aa_p) - i, mask, aa_color_ver, aa_opa);
                px_fp(rt_origo.x + LV_CIRC_OCT8_X(aa_p) + 1, rt_origo.y + LV_CIRC_OCT8_Y(aa_p) - i, mask, aa_color_ver, aa_opa);
            }

            out_x_last = cir.x;
            out_y_seg_start = out_y_seg_end;
        }
#endif
        uint8_t edge_top_refr = 0;
        uint8_t mid_top_refr = 0;
        uint8_t mid_bot_refr = 0;
        uint8_t edge_bot_refr = 0;

        /* If a new row coming draw the previous
         * The y coordinate can remain the same so wait for a new*/
        if(mid_bot_area.y1 != LV_CIRC_OCT4_Y(cir) + lb_origo.y ) mid_bot_refr = 1;

        if(edge_bot_area.y1 != LV_CIRC_OCT2_Y(cir) + lb_origo.y) edge_bot_refr = 1;

        if(mid_top_area.y1 != LV_CIRC_OCT8_Y(cir) + lt_origo.y) mid_top_refr = 1;

        if(edge_top_area.y1 != LV_CIRC_OCT7_Y(cir) + lt_origo.y) edge_top_refr = 1;

        /*Draw the areas which are not disabled*/
        if(edge_top_refr != 0){
            if(mcolor.full == gcolor.full) act_color = mcolor;
            else {
                mix = (uint32_t)((uint32_t)(coords->y2 - edge_top_area.y1)  * 255) / height;
                act_color = lv_color_mix(mcolor, gcolor, mix);
            }
            fill_fp(&edge_top_area, mask, act_color, opa);
        }

        if(mid_top_refr != 0) {
            if(mcolor.full == gcolor.full) act_color = mcolor;
            else {
                mix = (uint32_t)((uint32_t)(coords->y2 - mid_top_area.y1) * 255) / height;
                act_color = lv_color_mix(mcolor, gcolor, mix);
            }
            fill_fp(&mid_top_area, mask, act_color, opa);
        }

        if(mid_bot_refr != 0) {
            if(mcolor.full == gcolor.full) act_color = mcolor;
            else {
                mix = (uint32_t)((uint32_t)(coords->y2 - mid_bot_area.y1) * 255) / height;
                act_color = lv_color_mix(mcolor, gcolor, mix);
            }
            fill_fp(&mid_bot_area, mask, act_color, opa);
        }

        if(edge_bot_refr != 0) {

            if(mcolor.full == gcolor.full) act_color = mcolor;
            else {
                mix = (uint32_t)((uint32_t)(coords->y2 - edge_bot_area.y1) * 255) / height;
                act_color = lv_color_mix(mcolor, gcolor, mix);
            }
            fill_fp(&edge_bot_area, mask, act_color, opa);
        }

         /*Save the current coordinates*/
        lv_area_set(&mid_bot_area,  lb_origo.x + LV_CIRC_OCT4_X(cir),
                                 lb_origo.y + LV_CIRC_OCT4_Y(cir),
                                 rb_origo.x + LV_CIRC_OCT1_X(cir),
                                 rb_origo.y + LV_CIRC_OCT1_Y(cir));

        lv_area_set(&edge_bot_area, lb_origo.x + LV_CIRC_OCT3_X(cir),
                                 lb_origo.y + LV_CIRC_OCT3_Y(cir),
                                 rb_origo.x + LV_CIRC_OCT2_X(cir),
                                 rb_origo.y + LV_CIRC_OCT2_Y(cir));

        lv_area_set(&mid_top_area,  lt_origo.x + LV_CIRC_OCT5_X(cir),
                                 lt_origo.y + LV_CIRC_OCT5_Y(cir),
                                 rt_origo.x + LV_CIRC_OCT8_X(cir),
                                 rt_origo.y + LV_CIRC_OCT8_Y(cir));

        lv_area_set(&edge_top_area, lt_origo.x + LV_CIRC_OCT6_X(cir),
                                 lt_origo.y + LV_CIRC_OCT6_Y(cir),
                                 rt_origo.x + LV_CIRC_OCT7_X(cir),
                                 rt_origo.y + LV_CIRC_OCT7_Y(cir));

        lv_circ_next(&cir, &cir_tmp);
    }

    if(mcolor.full == gcolor.full) act_color = mcolor;
    else {
        mix = (uint32_t)((uint32_t)(coords->y2 - edge_top_area.y1)  * 255) / height;
        act_color = lv_color_mix(mcolor, gcolor, mix);
    }
	fill_fp(&edge_top_area, mask, act_color, opa);

	if(edge_top_area.y1 != mid_top_area.y1) {

        if(mcolor.full == gcolor.full) act_color = mcolor;
        else {
            mix = (uint32_t)((uint32_t)(coords->y2 - mid_top_area.y1) * 255) / height;
            act_color = lv_color_mix(mcolor, gcolor, mix);
	    }
		fill_fp(&mid_top_area, mask, act_color, opa);
	}

    if(mcolor.full == gcolor.full) act_color = mcolor;
    else {
        mix = (uint32_t)((uint32_t)(coords->y2 - mid_bot_area.y1) * 255) / height;
        act_color = lv_color_mix(mcolor, gcolor, mix);
    }
	fill_fp(&mid_bot_area, mask, act_color, opa);

	if(edge_bot_area.y1 != mid_bot_area.y1) {

        if(mcolor.full == gcolor.full) act_color = mcolor;
        else {
            mix = (uint32_t)((uint32_t)(coords->y2 - edge_bot_area.y1) * 255) / height;
            act_color = lv_color_mix(mcolor, gcolor, mix);
        }
		fill_fp(&edge_bot_area, mask, act_color, opa);
	}


#if LV_ANTIALIAS
	/*The first and the last line is not drawn*/
	edge_top_area.x1 = coords->x1 + radius + 2;
    edge_top_area.x2 = coords->x2 - radius - 2;
    edge_top_area.y1 = coords->y1;
    edge_top_area.y2 = coords->y1;
    fill_fp(&edge_top_area, mask, style->body.main_color, opa);

    edge_top_area.y1 = coords->y2;
    edge_top_area.y2 = coords->y2;
    fill_fp(&edge_top_area, mask, style->body.grad_color, opa);

    /*Last parts of the anti-alias*/
    out_y_seg_end = cir.y;
    lv_coord_t seg_size = out_y_seg_end - out_y_seg_start;
    lv_point_t aa_p;

    aa_p.x = out_x_last;
    aa_p.y = out_y_seg_start;

    mix = (uint32_t)((uint32_t)(radius - out_x_last) * 255) / height;
    aa_color_hor_bottom = lv_color_mix(gcolor, mcolor, mix);
    aa_color_hor_top = lv_color_mix(mcolor, gcolor, mix);

    lv_coord_t i;
    for(i = 0; i  < seg_size; i++) {
        lv_opa_t aa_opa = opa - antialias_get_opa(seg_size, i, opa);
        px_fp(rb_origo.x + LV_CIRC_OCT2_X(aa_p) + i, rb_origo.y + LV_CIRC_OCT2_Y(aa_p) + 1, mask, aa_color_hor_top, aa_opa);
        px_fp(lb_origo.x + LV_CIRC_OCT3_X(aa_p) - i, lb_origo.y + LV_CIRC_OCT3_Y(aa_p) + 1, mask, aa_color_hor_top, aa_opa);
        px_fp(lt_origo.x + LV_CIRC_OCT6_X(aa_p) - i, lt_origo.y + LV_CIRC_OCT6_Y(aa_p) - 1, mask, aa_color_hor_bottom, aa_opa);
        px_fp(rt_origo.x + LV_CIRC_OCT7_X(aa_p) + i, rt_origo.y + LV_CIRC_OCT7_Y(aa_p) - 1, mask, aa_color_hor_bottom, aa_opa);

        mix = (uint32_t)((uint32_t)(radius - out_y_seg_start + i) * 255) / height;
        aa_color_ver = lv_color_mix(mcolor, gcolor, mix);
        px_fp(rb_origo.x + LV_CIRC_OCT1_X(aa_p) + 1, rb_origo.y + LV_CIRC_OCT1_Y(aa_p) + i, mask, aa_color_ver, aa_opa);
        px_fp(lb_origo.x + LV_CIRC_OCT4_X(aa_p) - 1, lb_origo.y + LV_CIRC_OCT4_Y(aa_p) + i, mask, aa_color_ver, aa_opa);

        aa_color_ver = lv_color_mix(gcolor, mcolor, mix);
        px_fp(lt_origo.x + LV_CIRC_OCT5_X(aa_p) - 1, lt_origo.y + LV_CIRC_OCT5_Y(aa_p) - i, mask, aa_color_ver, aa_opa);
        px_fp(rt_origo.x + LV_CIRC_OCT8_X(aa_p) + 1, rt_origo.y + LV_CIRC_OCT8_Y(aa_p) - i, mask, aa_color_ver, aa_opa);
    }

    /*In some cases the last pixel is not drawn*/
    if(LV_MATH_ABS(aa_p.x - aa_p.y) == seg_size) {
        aa_p.x = out_x_last;
        aa_p.y = out_x_last;

        mix = (uint32_t)((uint32_t)(out_x_last) * 255) / height;
        aa_color_hor_top = lv_color_mix(gcolor, mcolor, mix);
        aa_color_hor_bottom = lv_color_mix(mcolor, gcolor, mix);

        lv_opa_t aa_opa = style->body.opa >> 1;
        px_fp(rb_origo.x + LV_CIRC_OCT2_X(aa_p), rb_origo.y + LV_CIRC_OCT2_Y(aa_p), mask, aa_color_hor_bottom, aa_opa);
        px_fp(lb_origo.x + LV_CIRC_OCT4_X(aa_p), lb_origo.y + LV_CIRC_OCT4_Y(aa_p), mask, aa_color_hor_bottom, aa_opa);
        px_fp(lt_origo.x + LV_CIRC_OCT6_X(aa_p), lt_origo.y + LV_CIRC_OCT6_Y(aa_p), mask, aa_color_hor_top, aa_opa);
        px_fp(rt_origo.x + LV_CIRC_OCT8_X(aa_p), rt_origo.y + LV_CIRC_OCT8_Y(aa_p), mask, aa_color_hor_top, aa_opa);
    }

#endif


}

/**
 * Draw the corners of a rectangle border
 * @param coords the coordinates of the original rectangle
 * @param mask the rectangle will be drawn only  on this area
 * @param rects_p pointer to a rectangle style
 * @param opa opacity of the rectangle (0..255)
 */
static void lv_draw_rect_border_corner(const lv_area_t * coords, const lv_area_t * mask, const  lv_style_t * style)
{
    uint16_t radius = style->body.radius ;
    uint16_t bwidth = style->body.border.width;
    lv_color_t color = style->body.border.color;
    lv_opa_t opa = style->body.border.opa;
    lv_border_part_t part = style->body.border.part;

    /*0 px border width drawn as 1 px, so decrement the bwidth*/
    bwidth--;

#if LV_ANTIALIAS
    bwidth--;    /*Because of anti-aliasing the border seems one pixel ticker*/
#endif

    lv_coord_t width = lv_area_get_width(coords);
    lv_coord_t height = lv_area_get_height(coords);

    radius = lv_draw_cont_radius_corr(radius, width, height);

    lv_point_t lt_origo;   /*Left  Top    origo*/
    lv_point_t lb_origo;   /*Left  Bottom origo*/
    lv_point_t rt_origo;   /*Right Top    origo*/
    lv_point_t rb_origo;   /*Left  Bottom origo*/

    lt_origo.x = coords->x1 + radius + LV_ANTIALIAS;
    lt_origo.y = coords->y1 + radius + LV_ANTIALIAS;

    lb_origo.x = coords->x1 + radius + LV_ANTIALIAS;
    lb_origo.y = coords->y2 - radius - LV_ANTIALIAS;

    rt_origo.x = coords->x2 - radius - LV_ANTIALIAS;
    rt_origo.y = coords->y1 + radius + LV_ANTIALIAS;

    rb_origo.x = coords->x2 - radius - LV_ANTIALIAS;
    rb_origo.y = coords->y2 - radius - LV_ANTIALIAS;

    lv_point_t cir_out;
    lv_coord_t tmp_out;
    lv_circ_init(&cir_out, &tmp_out, radius);

    lv_point_t cir_in;
    lv_coord_t tmp_in;
    lv_coord_t radius_in = radius - bwidth;

    if(radius_in < 0){
        radius_in = 0;
    }

    lv_circ_init(&cir_in, &tmp_in, radius_in);

    lv_area_t circ_area;
    lv_coord_t act_w1;
    lv_coord_t act_w2;

#if LV_ANTIALIAS
    /*Store some internal states for anti-aliasing*/
    lv_coord_t out_y_seg_start = 0;
    lv_coord_t out_y_seg_end = 0;
    lv_coord_t out_x_last = radius;


    lv_coord_t in_y_seg_start = 0;
    lv_coord_t in_y_seg_end = 0;
    lv_coord_t in_x_last = radius - bwidth;
#endif

    while( cir_out.y <= cir_out.x) {

        /*Calculate the actual width to avoid overwriting pixels*/
        if(cir_in.y < cir_in.x) {
            act_w1 = cir_out.x - cir_in.x;
            act_w2 = act_w1;
        } else {
            act_w1 = cir_out.x - cir_out.y;
            act_w2 = act_w1 - 1;
        }

#if LV_ANTIALIAS != 0
        /*New step in y on the outter circle*/
        if(out_x_last != cir_out.x) {
            out_y_seg_end = cir_out.y;
            lv_coord_t seg_size = out_y_seg_end - out_y_seg_start;
            lv_point_t aa_p;

            aa_p.x = out_x_last;
            aa_p.y = out_y_seg_start;

            lv_coord_t i;
            for(i = 0; i  < seg_size; i++) {
                lv_opa_t aa_opa;

                if(seg_size > CIRCLE_AA_NON_LINEAR_OPA_THRESHOLD) {    /*Use non-linear opa mapping on the first segment*/
                    aa_opa = antialias_get_opa_circ(seg_size, i, style->body.border.opa);
                } else {
                    aa_opa = style->body.border.opa - antialias_get_opa(seg_size, i, style->body.border.opa);
                }

                if((part & LV_BORDER_BOTTOM) && (part & LV_BORDER_RIGHT)) {
                    px_fp(rb_origo.x + LV_CIRC_OCT1_X(aa_p) + 1, rb_origo.y + LV_CIRC_OCT1_Y(aa_p) + i, mask, style->body.border.color, aa_opa);
                    px_fp(rb_origo.x + LV_CIRC_OCT2_X(aa_p) + i, rb_origo.y + LV_CIRC_OCT2_Y(aa_p) + 1, mask, style->body.border.color, aa_opa);
                }

                if((part & LV_BORDER_BOTTOM) && (part & LV_BORDER_LEFT)) {
                    px_fp(lb_origo.x + LV_CIRC_OCT3_X(aa_p) - i, lb_origo.y + LV_CIRC_OCT3_Y(aa_p) + 1, mask, style->body.border.color, aa_opa);
                    px_fp(lb_origo.x + LV_CIRC_OCT4_X(aa_p) - 1, lb_origo.y + LV_CIRC_OCT4_Y(aa_p) + i, mask, style->body.border.color, aa_opa);
                }


                if((part & LV_BORDER_TOP) && (part & LV_BORDER_LEFT)) {
                    px_fp(lt_origo.x + LV_CIRC_OCT5_X(aa_p) - 1, lt_origo.y + LV_CIRC_OCT5_Y(aa_p) - i, mask, style->body.border.color, aa_opa);
                    px_fp(lt_origo.x + LV_CIRC_OCT6_X(aa_p) - i, lt_origo.y + LV_CIRC_OCT6_Y(aa_p) - 1, mask, style->body.border.color, aa_opa);
                }

                if((part & LV_BORDER_TOP) && (part & LV_BORDER_RIGHT)) {
                    px_fp(rt_origo.x + LV_CIRC_OCT7_X(aa_p) + i, rt_origo.y + LV_CIRC_OCT7_Y(aa_p) - 1, mask, style->body.border.color, aa_opa);
                    px_fp(rt_origo.x + LV_CIRC_OCT8_X(aa_p) + 1, rt_origo.y + LV_CIRC_OCT8_Y(aa_p) - i, mask, style->body.border.color, aa_opa);
                }
            }

            out_x_last = cir_out.x;
            out_y_seg_start = out_y_seg_end;
        }

        /*New step in y on the inner circle*/
       if(in_x_last != cir_in.x) {
           in_y_seg_end = cir_out.y;
           lv_coord_t seg_size = in_y_seg_end - in_y_seg_start;
           lv_point_t aa_p;

           aa_p.x = in_x_last;
           aa_p.y = in_y_seg_start;

           lv_coord_t i;
           for(i = 0; i  < seg_size; i++) {
               lv_opa_t aa_opa;

               if(seg_size > CIRCLE_AA_NON_LINEAR_OPA_THRESHOLD) {    /*Use non-linear opa mapping on the first segment*/
                   aa_opa = style->body.border.opa - antialias_get_opa_circ(seg_size, i, style->body.border.opa);
               } else {
                   aa_opa = antialias_get_opa(seg_size, i, style->body.border.opa);
               }

               if((part & LV_BORDER_BOTTOM) && (part & LV_BORDER_RIGHT)) {
                   px_fp(rb_origo.x + LV_CIRC_OCT1_X(aa_p) - 1, rb_origo.y + LV_CIRC_OCT1_Y(aa_p) + i, mask, style->body.border.color, aa_opa);
               }

               if((part & LV_BORDER_BOTTOM) && (part & LV_BORDER_LEFT)) {
                   px_fp(lb_origo.x + LV_CIRC_OCT3_X(aa_p) - i, lb_origo.y + LV_CIRC_OCT3_Y(aa_p) - 1, mask, style->body.border.color, aa_opa);
               }

               if((part & LV_BORDER_TOP) && (part & LV_BORDER_LEFT)) {
                   px_fp(lt_origo.x + LV_CIRC_OCT5_X(aa_p) + 1, lt_origo.y + LV_CIRC_OCT5_Y(aa_p) - i, mask, style->body.border.color, aa_opa);
               }

               if((part & LV_BORDER_TOP) && (part & LV_BORDER_RIGHT)) {
                  px_fp(rt_origo.x + LV_CIRC_OCT7_X(aa_p) + i, rt_origo.y + LV_CIRC_OCT7_Y(aa_p) + 1, mask, style->body.border.color, aa_opa);
               }

               /*Be sure the pixels on the middle are not drawn twice*/
               if(LV_CIRC_OCT1_X(aa_p) - 1 != LV_CIRC_OCT2_X(aa_p) + i) {
                   if((part & LV_BORDER_BOTTOM) && (part & LV_BORDER_RIGHT)) {
                       px_fp(rb_origo.x + LV_CIRC_OCT2_X(aa_p) + i, rb_origo.y + LV_CIRC_OCT2_Y(aa_p) - 1, mask, style->body.border.color, aa_opa);
                   }

                   if((part & LV_BORDER_BOTTOM) && (part & LV_BORDER_LEFT)) {
                       px_fp(lb_origo.x + LV_CIRC_OCT4_X(aa_p) + 1, lb_origo.y + LV_CIRC_OCT4_Y(aa_p) + i, mask, style->body.border.color, aa_opa);
                   }

                   if((part & LV_BORDER_TOP) && (part & LV_BORDER_LEFT)) {
                       px_fp(lt_origo.x + LV_CIRC_OCT6_X(aa_p) - i, lt_origo.y + LV_CIRC_OCT6_Y(aa_p) + 1, mask, style->body.border.color, aa_opa);
                   }

                   if((part & LV_BORDER_TOP) && (part & LV_BORDER_RIGHT)) {
                       px_fp(rt_origo.x + LV_CIRC_OCT8_X(aa_p) - 1, rt_origo.y + LV_CIRC_OCT8_Y(aa_p) - i, mask, style->body.border.color, aa_opa);
                   }
               }

           }

           in_x_last = cir_in.x;
           in_y_seg_start = in_y_seg_end;

       }

#endif


        /*Draw the octets to the right bottom corner*/
        if((part & LV_BORDER_BOTTOM) && (part & LV_BORDER_RIGHT)) {
            circ_area.x1 = rb_origo.x + LV_CIRC_OCT1_X(cir_out) - act_w2;
            circ_area.x2 = rb_origo.x + LV_CIRC_OCT1_X(cir_out);
            circ_area.y1 = rb_origo.y + LV_CIRC_OCT1_Y(cir_out);
            circ_area.y2 = rb_origo.y + LV_CIRC_OCT1_Y(cir_out);
            fill_fp(&circ_area, mask, color, opa);

            circ_area.x1 = rb_origo.x + LV_CIRC_OCT2_X(cir_out);
            circ_area.x2 = rb_origo.x + LV_CIRC_OCT2_X(cir_out);
            circ_area.y1 = rb_origo.y + LV_CIRC_OCT2_Y(cir_out)- act_w1;
            circ_area.y2 = rb_origo.y + LV_CIRC_OCT2_Y(cir_out);
            fill_fp(&circ_area, mask, color, opa);
        }

        /*Draw the octets to the left bottom corner*/
        if((part & LV_BORDER_BOTTOM) && (part & LV_BORDER_LEFT)) {
            circ_area.x1 = lb_origo.x + LV_CIRC_OCT3_X(cir_out);
            circ_area.x2 = lb_origo.x + LV_CIRC_OCT3_X(cir_out);
            circ_area.y1 = lb_origo.y + LV_CIRC_OCT3_Y(cir_out) - act_w2;
            circ_area.y2 = lb_origo.y + LV_CIRC_OCT3_Y(cir_out);
            fill_fp(&circ_area, mask, color, opa);

            circ_area.x1 = lb_origo.x + LV_CIRC_OCT4_X(cir_out);
            circ_area.x2 = lb_origo.x + LV_CIRC_OCT4_X(cir_out) + act_w1;
            circ_area.y1 = lb_origo.y + LV_CIRC_OCT4_Y(cir_out);
            circ_area.y2 = lb_origo.y + LV_CIRC_OCT4_Y(cir_out);
            fill_fp(&circ_area, mask, color, opa);
        }

        /*Draw the octets to the left top corner*/
        if((part & LV_BORDER_TOP) && (part & LV_BORDER_LEFT)) {
            if(lb_origo.y + LV_CIRC_OCT4_Y(cir_out) > lt_origo.y + LV_CIRC_OCT5_Y(cir_out)) {
                /*Don't draw if the lines are common in the middle*/
                circ_area.x1 = lt_origo.x + LV_CIRC_OCT5_X(cir_out);
                circ_area.x2 = lt_origo.x + LV_CIRC_OCT5_X(cir_out) + act_w2;
                circ_area.y1 = lt_origo.y + LV_CIRC_OCT5_Y(cir_out);
                circ_area.y2 = lt_origo.y + LV_CIRC_OCT5_Y(cir_out);
                fill_fp(&circ_area, mask, color, opa);
            }

            circ_area.x1 = lt_origo.x + LV_CIRC_OCT6_X(cir_out);
            circ_area.x2 = lt_origo.x + LV_CIRC_OCT6_X(cir_out);
            circ_area.y1 = lt_origo.y + LV_CIRC_OCT6_Y(cir_out);
            circ_area.y2 = lt_origo.y + LV_CIRC_OCT6_Y(cir_out) + act_w1;
            fill_fp(&circ_area, mask, color, opa);
        }

        /*Draw the octets to the right top corner*/
        if((part & LV_BORDER_TOP) && (part & LV_BORDER_RIGHT)) {
            circ_area.x1 = rt_origo.x + LV_CIRC_OCT7_X(cir_out);
            circ_area.x2 = rt_origo.x + LV_CIRC_OCT7_X(cir_out);
            circ_area.y1 = rt_origo.y + LV_CIRC_OCT7_Y(cir_out);
            circ_area.y2 = rt_origo.y + LV_CIRC_OCT7_Y(cir_out) + act_w2;
            fill_fp(&circ_area, mask, color, opa);

            /*Don't draw if the lines are common in the middle*/
            if(rb_origo.y + LV_CIRC_OCT1_Y(cir_out) > rt_origo.y + LV_CIRC_OCT8_Y(cir_out)) {
                circ_area.x1 = rt_origo.x + LV_CIRC_OCT8_X(cir_out) - act_w1;
                circ_area.x2 = rt_origo.x + LV_CIRC_OCT8_X(cir_out);
                circ_area.y1 = rt_origo.y + LV_CIRC_OCT8_Y(cir_out);
                circ_area.y2 = rt_origo.y + LV_CIRC_OCT8_Y(cir_out);
                fill_fp(&circ_area, mask, color, opa);
            }
        }
        lv_circ_next(&cir_out, &tmp_out);

        /*The internal circle will be ready faster
         * so check it! */
        if(cir_in.y < cir_in.x) {
            lv_circ_next(&cir_in, &tmp_in);
        }
    }


#if LV_ANTIALIAS != 0

    /*Last parts of the outer anti-alias*/
    out_y_seg_end = cir_out.y;
    lv_coord_t seg_size = out_y_seg_end - out_y_seg_start;
    lv_point_t aa_p;

    aa_p.x = out_x_last;
    aa_p.y = out_y_seg_start;

    lv_coord_t i;
    for(i = 0; i  < seg_size; i++) {
        lv_opa_t aa_opa = style->body.border.opa - antialias_get_opa(seg_size, i, style->body.border.opa);
        if((part & LV_BORDER_BOTTOM) && (part & LV_BORDER_RIGHT)) {
            px_fp(rb_origo.x + LV_CIRC_OCT1_X(aa_p) + 1, rb_origo.y + LV_CIRC_OCT1_Y(aa_p) + i, mask, style->body.border.color, aa_opa);
            px_fp(rb_origo.x + LV_CIRC_OCT2_X(aa_p) + i, rb_origo.y + LV_CIRC_OCT2_Y(aa_p) + 1, mask, style->body.border.color, aa_opa);
        }

        if((part & LV_BORDER_BOTTOM) && (part & LV_BORDER_LEFT)) {
            px_fp(lb_origo.x + LV_CIRC_OCT3_X(aa_p) - i, lb_origo.y + LV_CIRC_OCT3_Y(aa_p) + 1, mask, style->body.border.color, aa_opa);
            px_fp(lb_origo.x + LV_CIRC_OCT4_X(aa_p) - 1, lb_origo.y + LV_CIRC_OCT4_Y(aa_p) + i, mask, style->body.border.color, aa_opa);
        }

        if((part & LV_BORDER_TOP) && (part & LV_BORDER_LEFT)) {
            px_fp(lt_origo.x + LV_CIRC_OCT5_X(aa_p) - 1, lt_origo.y + LV_CIRC_OCT5_Y(aa_p) - i, mask, style->body.border.color, aa_opa);
            px_fp(lt_origo.x + LV_CIRC_OCT6_X(aa_p) - i, lt_origo.y + LV_CIRC_OCT6_Y(aa_p) - 1, mask, style->body.border.color, aa_opa);
        }

        if((part & LV_BORDER_TOP) && (part & LV_BORDER_RIGHT)) {
            px_fp(rt_origo.x + LV_CIRC_OCT7_X(aa_p) + i, rt_origo.y + LV_CIRC_OCT7_Y(aa_p) - 1, mask, style->body.border.color, aa_opa);
            px_fp(rt_origo.x + LV_CIRC_OCT8_X(aa_p) + 1, rt_origo.y + LV_CIRC_OCT8_Y(aa_p) - i, mask, style->body.border.color, aa_opa);
        }
    }

    /*In some cases the last pixel in the outer middle is not drawn*/
    if(LV_MATH_ABS(aa_p.x - aa_p.y) == seg_size) {
        aa_p.x = out_x_last;
        aa_p.y = out_x_last;

        lv_opa_t aa_opa = style->body.border.opa >> 1;

        if((part & LV_BORDER_BOTTOM) && (part & LV_BORDER_RIGHT)) {
            px_fp(rb_origo.x + LV_CIRC_OCT2_X(aa_p), rb_origo.y + LV_CIRC_OCT2_Y(aa_p), mask, style->body.border.color, aa_opa);
        }

        if((part & LV_BORDER_BOTTOM) && (part & LV_BORDER_LEFT)) {
            px_fp(lb_origo.x + LV_CIRC_OCT4_X(aa_p), lb_origo.y + LV_CIRC_OCT4_Y(aa_p), mask, style->body.border.color, aa_opa);
        }

        if((part & LV_BORDER_TOP) && (part & LV_BORDER_LEFT)) {
            px_fp(lt_origo.x + LV_CIRC_OCT6_X(aa_p), lt_origo.y + LV_CIRC_OCT6_Y(aa_p), mask, style->body.border.color, aa_opa);
        }

        if((part & LV_BORDER_TOP) && (part & LV_BORDER_RIGHT)) {
            px_fp(rt_origo.x + LV_CIRC_OCT8_X(aa_p), rt_origo.y + LV_CIRC_OCT8_Y(aa_p), mask, style->body.border.color, aa_opa);
        }
    }

    /*Last parts of the inner anti-alias*/
    in_y_seg_end = cir_in.y;
    aa_p.x = in_x_last;
    aa_p.y = in_y_seg_start;
    seg_size = in_y_seg_end - in_y_seg_start;

    for(i = 0; i  < seg_size; i++) {
        lv_opa_t aa_opa =  antialias_get_opa(seg_size, i, style->body.border.opa);
        if((part & LV_BORDER_BOTTOM) && (part & LV_BORDER_RIGHT)) {
            px_fp(rb_origo.x + LV_CIRC_OCT1_X(aa_p) - 1, rb_origo.y + LV_CIRC_OCT1_Y(aa_p) + i, mask, style->body.border.color, aa_opa);
        }

        if((part & LV_BORDER_BOTTOM) && (part & LV_BORDER_LEFT)) {
            px_fp(lb_origo.x + LV_CIRC_OCT3_X(aa_p) - i, lb_origo.y + LV_CIRC_OCT3_Y(aa_p) - 1, mask, style->body.border.color, aa_opa);
        }

        if((part & LV_BORDER_TOP) && (part & LV_BORDER_LEFT)) {
            px_fp(lt_origo.x + LV_CIRC_OCT5_X(aa_p) + 1, lt_origo.y + LV_CIRC_OCT5_Y(aa_p) - i, mask, style->body.border.color, aa_opa);
        }

        if((part & LV_BORDER_TOP) && (part & LV_BORDER_RIGHT)) {
            px_fp(rt_origo.x + LV_CIRC_OCT7_X(aa_p) + i, rt_origo.y + LV_CIRC_OCT7_Y(aa_p) + 1, mask, style->body.border.color, aa_opa);
        }

        if(LV_CIRC_OCT1_X(aa_p) - 1 != LV_CIRC_OCT2_X(aa_p) + i) {
            if((part & LV_BORDER_BOTTOM) && (part & LV_BORDER_RIGHT)) {
                px_fp(rb_origo.x + LV_CIRC_OCT2_X(aa_p) + i, rb_origo.y + LV_CIRC_OCT2_Y(aa_p) - 1, mask, style->body.border.color, aa_opa);
            }

            if((part & LV_BORDER_BOTTOM) && (part & LV_BORDER_LEFT)) {
                px_fp(lb_origo.x + LV_CIRC_OCT4_X(aa_p) + 1, lb_origo.y + LV_CIRC_OCT4_Y(aa_p) + i, mask, style->body.border.color, aa_opa);
            }

            if((part & LV_BORDER_TOP) && (part & LV_BORDER_LEFT)) {
                px_fp(lt_origo.x + LV_CIRC_OCT6_X(aa_p) - i, lt_origo.y + LV_CIRC_OCT6_Y(aa_p) + 1, mask, style->body.border.color, aa_opa);
            }

            if((part & LV_BORDER_TOP) && (part & LV_BORDER_RIGHT)) {
                px_fp(rt_origo.x + LV_CIRC_OCT8_X(aa_p) - 1, rt_origo.y + LV_CIRC_OCT8_Y(aa_p) - i, mask, style->body.border.color, aa_opa);
            }
        }
    }

#endif

}


/**
 * Draw the straight parts of a rectangle border
 * @param coords the coordinates of the original rectangle
 * @param mask_ the rectangle will be drawn only  on this area
 * @param rstyle pointer to a rectangle style
 */
static void lv_draw_rect_border_straight(const lv_area_t * coords, const lv_area_t * mask, const lv_style_t * style)
{
    uint16_t radius = style->body.radius;

    lv_coord_t width = lv_area_get_width(coords);
    lv_coord_t height = lv_area_get_height(coords);
    uint16_t bwidth = style->body.border.width;
    lv_opa_t opa = style->body.border.opa;
    lv_border_part_t part = style->body.border.part;
    lv_color_t color = style->body.border.color;
    lv_area_t work_area;
    lv_coord_t length_corr = 0;
    lv_coord_t corner_size = 0;

    /*the 0 px border width drawn as 1 px, so decrement the b_width*/
    bwidth--;

    radius = lv_draw_cont_radius_corr(radius, width, height);

    if(radius < bwidth)
    {
        length_corr = bwidth - radius - LV_ANTIALIAS;
        corner_size = bwidth;
    }
    else
    {
        corner_size = radius + LV_ANTIALIAS;
    }

    /* Modify the corner_size if corner is drawn */
    corner_size ++;

    /*Depending one which part's are drawn modify the area lengths */
    if(part & LV_BORDER_TOP)
    {
    	work_area.y1 = coords->y1 + corner_size;
    }
    else
    {
    	work_area.y1 = coords->y1 + radius;
    }

    if(part & LV_BORDER_BOTTOM)
    {
    	work_area.y2 = coords->y2 - corner_size;
    }
    else
    {
    	work_area.y2 = coords->y2 - radius;
    }

    /*Left border*/
    if(part & LV_BORDER_LEFT)
    {
        work_area.x1 = coords->x1;
        work_area.x2 = work_area.x1 + bwidth;
        fill_fp(&work_area, mask, color, opa);
    }

    /*Right border*/
    if(part & LV_BORDER_RIGHT)
    {
        work_area.x2 = coords->x2;
        work_area.x1 = work_area.x2 - bwidth;
        fill_fp(&work_area, mask, color, opa);
    }

    work_area.x1 = coords->x1 + corner_size - length_corr;
    work_area.x2 = coords->x2 - corner_size + length_corr;

    /*Upper border*/
    if(part & LV_BORDER_TOP)
    {
        work_area.y1 = coords->y1;
        work_area.y2 = coords->y1 + bwidth;
        fill_fp(&work_area, mask, color, opa);
    }

    /*Lower border*/
    if(part & LV_BORDER_BOTTOM)
    {
        work_area.y2 = coords->y2;
        work_area.y1 = work_area.y2 - bwidth;
        fill_fp(&work_area, mask, color, opa);
    }

    /*Draw the a remaining rectangles if the radius is smaller then bwidth */
    if(length_corr != 0)
    {
        /*Left top correction*/
        if((part & LV_BORDER_TOP) && (part & LV_BORDER_LEFT))
        {
            work_area.x1 = coords->x1;
            work_area.x2 = coords->x1 + radius + LV_ANTIALIAS;
            work_area.y1 = coords->y1 + radius + 1 + LV_ANTIALIAS;
            work_area.y2 = coords->y1 + bwidth;
            fill_fp(&work_area, mask, color, opa);
        }

        /*Right top correction*/
        if((part & LV_BORDER_TOP) && (part & LV_BORDER_RIGHT))
        {
            work_area.x1 = coords->x2 - radius - LV_ANTIALIAS;
            work_area.x2 = coords->x2;
            work_area.y1 = coords->y1 + radius + 1 + LV_ANTIALIAS;
            work_area.y2 = coords->y1 + bwidth;
            fill_fp(&work_area, mask, color, opa);
        }

        /*Left bottom correction*/
        if((part & LV_BORDER_BOTTOM) && (part & LV_BORDER_LEFT))
        {
            work_area.x1 = coords->x1;
            work_area.x2 = coords->x1 + radius + LV_ANTIALIAS;
            work_area.y1 = coords->y2 - bwidth;
            work_area.y2 = coords->y2 - radius - 1 - LV_ANTIALIAS;
            fill_fp(&work_area, mask, color, opa);
        }

        /*Right bottom correction*/
        if((part & LV_BORDER_BOTTOM) && (part & LV_BORDER_RIGHT))
        {
            work_area.x1 = coords->x2 - radius - LV_ANTIALIAS;
            work_area.x2 = coords->x2;
            work_area.y1 = coords->y2 - bwidth;
            work_area.y2 = coords->y2 - radius - 1 - LV_ANTIALIAS;
            fill_fp(&work_area, mask, color, opa);
        }
    }

    /*If radius == 0 one px on the corners are not drawn by main drawer*/
    if(style->body.radius == 0)
    {
        /*Left top corner*/
        if(part & (LV_BORDER_TOP | LV_BORDER_LEFT))
        {
            work_area.x1 = coords->x1;
            work_area.x2 = coords->x1 + LV_ANTIALIAS;
            work_area.y1 = coords->y1;
            work_area.y2 = coords->y1 + LV_ANTIALIAS;
            fill_fp(&work_area, mask, color, opa);
        }

        /*Right top corner*/
        if(part & (LV_BORDER_TOP | LV_BORDER_RIGHT))
        {
            work_area.x1 = coords->x2 - LV_ANTIALIAS;
            work_area.x2 = coords->x2;
            work_area.y1 = coords->y1;
            work_area.y2 = coords->y1 + LV_ANTIALIAS;
            fill_fp(&work_area, mask, color, opa);
        }

        /*Left bottom corner*/
        if(part & (LV_BORDER_BOTTOM | LV_BORDER_LEFT))
        {
            work_area.x1 = coords->x1;
            work_area.x2 = coords->x1 + LV_ANTIALIAS;
            work_area.y1 = coords->y2 - LV_ANTIALIAS;
            work_area.y2 = coords->y2;
            fill_fp(&work_area, mask, color, opa);
        }

        /*Right bottom corner*/
        if(part & (LV_BORDER_BOTTOM | LV_BORDER_RIGHT))
        {
            work_area.x1 = coords->x2 - LV_ANTIALIAS;
            work_area.x2 = coords->x2;
            work_area.y1 = coords->y2 - LV_ANTIALIAS;
            work_area.y2 = coords->y2;
            fill_fp(&work_area, mask, color, opa);
        }
    }
}

static void lv_draw_shadow_bottom(const lv_area_t * coords, const lv_area_t * mask, const lv_style_t * style)
{
    lv_coord_t radius = style->body.radius;
    lv_coord_t swidth = style->body.shadow.width;
    lv_coord_t width = lv_area_get_width(coords);
    lv_coord_t height = lv_area_get_height(coords);

    radius = lv_draw_cont_radius_corr(radius, width, height);
    radius += LV_ANTIALIAS * SHADOW_BOTTOM_AA_EXTRA_RADIUS;
    swidth += LV_ANTIALIAS;

    lv_coord_t curve_x[radius + 1];             /*Stores the 'x' coordinates of a quarter circle.*/
    lv_point_t circ;
    lv_coord_t circ_tmp;
    lv_circ_init(&circ, &circ_tmp, radius);
    while(lv_circ_cont(&circ)) {
        curve_x[LV_CIRC_OCT1_Y(circ)] = LV_CIRC_OCT1_X(circ);
        curve_x[LV_CIRC_OCT2_Y(circ)] = LV_CIRC_OCT2_X(circ);
        lv_circ_next(&circ, &circ_tmp);
    }

    int16_t col;
    lv_opa_t line_1d_blur[swidth];

    for(col = 0; col < swidth; col++) {
        line_1d_blur[col] = (uint32_t)((uint32_t)(swidth - col) * style->body.opa / 2) / (swidth);
    }

    lv_point_t point_l;
    lv_point_t point_r;
    lv_area_t area_mid;
    lv_point_t ofs_l;
    lv_point_t ofs_r;

    ofs_l.x = coords->x1 + radius;
    ofs_l.y = coords->y2 - radius + 1 - LV_ANTIALIAS;

    ofs_r.x = coords->x2 - radius;
    ofs_r.y = coords->y2 - radius + 1 - LV_ANTIALIAS;

    for(col = 0; col <= radius; col++) {
        point_l.x = ofs_l.x - col ;
        point_l.y = ofs_l.y + curve_x[col];

        point_r.x = ofs_r.x + col;
        point_r.y = ofs_r.y + curve_x[col];

        lv_opa_t px_opa;
        int16_t diff = col == 0 ? 0 : curve_x[col-1] - curve_x[col];
        uint16_t d;
        for(d = 0; d < swidth; d++) {
            /*When stepping a pixel in y calculate the average with the pixel from the prev. column to make a blur */
            if(diff == 0) {
                px_opa = line_1d_blur[d];
            } else {
                px_opa = (uint16_t)((uint16_t)line_1d_blur[d] + line_1d_blur[d - diff]) >> 1;
            }
            px_fp(point_l.x, point_l.y, mask, style->body.shadow.color, px_opa);
            point_l.y ++;

            /*Don't overdraw the pixel on the middle*/
            if(point_r.x > ofs_l.x) {
                px_fp(point_r.x, point_r.y, mask, style->body.shadow.color, px_opa);
            }
            point_r.y ++;
        }

    }

    area_mid.x1 = ofs_l.x + 1;
    area_mid.y1 = ofs_l.y + radius;
    area_mid.x2 = ofs_r.x - 1;
    area_mid.y2 = area_mid.y1;

    uint16_t d;
    for(d = 0; d < swidth; d++) {
        fill_fp(&area_mid, mask, style->body.shadow.color, line_1d_blur[d]);
        area_mid.y1 ++;
        area_mid.y2 ++;
    }
}

static void lv_draw_shadow_full_straight(const lv_area_t * coords, const lv_area_t * mask, const lv_style_t * style, const lv_opa_t * map)
{
    lv_coord_t radius = style->body.radius;
    lv_coord_t swidth = style->body.shadow.width + LV_ANTIALIAS;
    lv_coord_t width = lv_area_get_width(coords);
    lv_coord_t height = lv_area_get_height(coords);

    radius = lv_draw_cont_radius_corr(radius, width, height);
    if(radius == 0) radius += LV_ANTIALIAS;

    lv_area_t right_area;
    right_area.x1 = coords->x2 + 1 - LV_ANTIALIAS;
    right_area.y1 = coords->y1 + radius;
    right_area.x2 = right_area.x1;
    right_area.y2 = coords->y2 -  radius;

    lv_area_t left_area;
    left_area.x1 = coords->x1 - 1 + LV_ANTIALIAS;
    left_area.y1 = coords->y1 + radius;
    left_area.x2 = left_area.x1;
    left_area.y2 = coords->y2 - radius;

    lv_area_t top_area;
    top_area.x1 = coords->x1 + radius;
    top_area.y1 = coords->y1 - 1 + LV_ANTIALIAS;
    top_area.x2 = coords->x2 - radius;
    top_area.y2 = top_area.y1;

    lv_area_t bottom_area;
    bottom_area.x1 = coords->x1 + radius;
    bottom_area.y1 = coords->y2 + 1 - LV_ANTIALIAS;
    bottom_area.x2 = coords->x2 - radius;
    bottom_area.y2 = bottom_area.y1;

    lv_opa_t opa_act;
    int16_t d;
    for(d = 1; d <= swidth; d++) {
        opa_act = map[d];
        fill_fp(&right_area, mask, style->body.shadow.color, opa_act);
        right_area.x1++;
        right_area.x2++;

        fill_fp(&left_area, mask, style->body.shadow.color, opa_act);
        left_area.x1--;
        left_area.x2--;

        fill_fp(&top_area, mask, style->body.shadow.color, opa_act);
        top_area.y1--;
        top_area.y2--;

        fill_fp(&bottom_area, mask, style->body.shadow.color, opa_act);
        bottom_area.y1++;
        bottom_area.y2++;
    }

}

static void lv_draw_shadow_full(const lv_area_t * coords, const lv_area_t * mask, const lv_style_t * style)
{

    lv_coord_t radius = style->body.radius;
    lv_coord_t swidth = style->body.shadow.width;

    lv_coord_t width = lv_area_get_width(coords);
    lv_coord_t height = lv_area_get_height(coords);

    radius = lv_draw_cont_radius_corr(radius, width, height);

    if(radius != 0) radius -= LV_ANTIALIAS;
    swidth += LV_ANTIALIAS;


    lv_coord_t curve_x[radius + swidth + 1];     /*Stores the 'x' coordinates of a quarter circle.*/
    memset(curve_x, 0, sizeof(curve_x));
    lv_point_t circ;
    lv_coord_t circ_tmp;
    lv_circ_init(&circ, &circ_tmp, radius);
    while(lv_circ_cont(&circ)) {
        curve_x[LV_CIRC_OCT1_Y(circ)] = LV_CIRC_OCT1_X(circ);
        curve_x[LV_CIRC_OCT2_Y(circ)] = LV_CIRC_OCT2_X(circ);
        lv_circ_next(&circ, &circ_tmp);
    }
    int16_t line;

    int16_t filter_width = 2 * swidth + 1;
    uint32_t line_1d_blur[filter_width];

    /*1D Blur horizontally*/
    for(line = 0; line < filter_width; line++) {
        line_1d_blur[line] = (uint32_t)((uint32_t)(filter_width - line) * (style->body.opa * 2)  << SHADOW_OPA_EXTRA_PRECISION) / (filter_width * filter_width);
    }

    uint16_t col;
    lv_opa_t line_2d_blur[radius + swidth];

    lv_point_t point_rt;
    lv_point_t point_rb;
    lv_point_t point_lt;
    lv_point_t point_lb;
    lv_point_t ofs_rb;
    lv_point_t ofs_rt;
    lv_point_t ofs_lb;
    lv_point_t ofs_lt;
    ofs_rb.x = coords->x2 - radius - LV_ANTIALIAS;
    ofs_rb.y = coords->y2 - radius - LV_ANTIALIAS;

    ofs_rt.x = coords->x2 - radius - LV_ANTIALIAS;
    ofs_rt.y = coords->y1 + radius + LV_ANTIALIAS;

    ofs_lb.x = coords->x1 + radius + LV_ANTIALIAS;
    ofs_lb.y = coords->y2 - radius - LV_ANTIALIAS;

    ofs_lt.x = coords->x1 + radius + LV_ANTIALIAS;
    ofs_lt.y = coords->y1 + radius + LV_ANTIALIAS;
    bool line_ready;
    for(line = 1; line <= radius + swidth; line++) {        /*Check all rows and make the 1D blur to 2D*/
        line_ready = false;
        for(col = 1; col < radius + swidth + 10; col++) {        /*Check all pixels in a 1D blur line (from the origo to last shadow pixel (radius + swidth))*/

            /*Sum the opacities from the lines above and below this 'row'*/
            int16_t line_rel;
            uint32_t px_opa_sum = 0;
            for(line_rel = -swidth; line_rel <= swidth; line_rel ++) {
                /*Get the relative x position of the 'line_rel' to 'line'*/
                int16_t col_rel;
                if(line + line_rel < 0) {                       /*Below the radius, here is the blur of the edge */
                    col_rel = radius - curve_x[line] - col;
                } else if(line + line_rel > radius) {           /*Above the radius, here won't be more 1D blur*/
                    break;
                } else {                                        /*Blur from the curve*/
                    col_rel = curve_x[line + line_rel] - curve_x[line] - col;
                }

                /*Add the value of the 1D blur on 'col_rel' position*/
                if(col_rel < -swidth) {                         /*Outside of the burred area. */
                    if(line_rel == -swidth) line_ready = true;  /*If no data even on the very first line then it wont't be anything else in this line*/
                    break;                                      /*Break anyway because only smaller 'col_rel' values will come */
                }
                else if (col_rel > swidth) px_opa_sum += line_1d_blur[0];       /*Inside the not blurred area*/
                else px_opa_sum += line_1d_blur[swidth - col_rel];              /*On the 1D blur (+ swidth to align to the center)*/
            }

            line_2d_blur[col] = px_opa_sum >> SHADOW_OPA_EXTRA_PRECISION;
            if(line_ready) break;

        }

        /*Flush the line*/
        point_rt.x = curve_x[line] + ofs_rt.x + 1;
        point_rt.y = ofs_rt.y - line;

        point_rb.x = curve_x[line] + ofs_rb.x + 1;
        point_rb.y = ofs_rb.y + line;

        point_lt.x = ofs_lt.x -  curve_x[line] - 1;
        point_lt.y = ofs_lt.y - line;

        point_lb.x = ofs_lb.x - curve_x[line] - 1;
        point_lb.y = ofs_lb.y + line;

        uint16_t d;
        for(d = 1; d <= col; d++) {

            if(point_rt.x != point_lt.x) {
                px_fp(point_lt.x,point_lt.y , mask, style->body.shadow.color, line_2d_blur[d]);
            }

            if(point_rb.x != point_lb.x && point_lt.y != point_lb.y) {
                px_fp(point_lb.x,point_lb.y , mask, style->body.shadow.color, line_2d_blur[d]);
            }

            if(point_lt.y != point_lb.y) {
                px_fp(point_rb.x,point_rb.y , mask, style->body.shadow.color, line_2d_blur[d]);
            }

            px_fp(point_rt.x,point_rt.y , mask, style->body.shadow.color, line_2d_blur[d]);


            point_rb.x++;
            point_lb.x--;

            point_rt.x++;
            point_lt.x--;
        }

        /* Put the first line to the edges too.
         * It is not correct because blur should be done below the corner too
         * but is is simple, fast and gives a good enough result*/
        if(line == 1)
        {
        	lv_draw_shadow_full_straight(coords, mask, style, line_2d_blur);
        }
    }
}

/*============================================================================*/

void draw(void)
{
	lv_point_t p1,p2;
	lv_style_t s;


	s.line.color.full =RGB565(31,0,0);
	s.line.opa =LV_OPA_100;
	s.line.width =5;

	p1.x =10;
	p1.y =20;
	p2.x =200;
	p2.y =100;
	lv_draw_line(&p1,&p2,&vdb.area,&s);

	s.line.color.full =RGB565(0,0,20);
	s.line.opa =LV_OPA_80;
	s.line.width =7;

	p2.x =300;
	p2.y =200;
	lv_draw_line(&p1,&p2,&vdb.area,&s);

	////////
	lv_area_t rc;
	rc.x1 =4;
	rc.y1 =100;
	rc.x2 =120;
	rc.y2 =150;

	s.body.main_color.full =RGB565(10,60,20);
	s.body.grad_color.full =RGB565(0,10,20);
	s.body.radius =16;
	s.body.opa =LV_OPA_100;
	lv_draw_rect_main_corner(&rc,&vdb.area,&s);
	lv_draw_rect_main_mid(&rc,&vdb.area,&s);
	s.body.shadow.color.full =RGB565(3,6,3);
	s.body.shadow.width = 6;
	lv_draw_shadow_full(&rc,&vdb.area,&s);

	////////
	rc.x1 =200;
	rc.y1 =4;
	rc.x2 =300;
	rc.y2 =80;

	s.body.border.width =6;
	s.body.border.opa =LV_OPA_100;
	s.body.border.color.full =RGB565(20,0,0);
	s.body.border.part =LV_BORDER_FULL;

	s.body.shadow.color.full =RGB565(3,6,3);
	s.body.shadow.width = 4;

	lv_draw_rect_border_corner(&rc,&vdb.area,&s);
	lv_draw_rect_border_straight(&rc,&vdb.area,&s);
	lv_draw_shadow_full(&rc,&vdb.area,&s);
}

/*============================================================================*/
static void back_init(HDC hdc,int w,int h)
{
	RECT rc;
	int x,y,i,yy;

	rc.w =80;
	rc.h =80;

	yy=0;
	for(y=0;y<h;y+=rc.h)
	{
		i=yy&1;
		for(x=0;x<w;x+=rc.w)
		{
			if(i&1)
			{
				SetBrushColor(hdc,MapRGB(hdc,220,220,220));
			}
			else
			{
				SetBrushColor(hdc,MapRGB(hdc,160,160,175));
			}

			rc.x =x;
			rc.y =y;
			FillRect(hdc,&rc);
			i++;
		}
		yy++;
	}

}


static LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc;

	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{

			//vlcd_fb =vmalloc(VLCD_W*VLCD_H*2);
			vlcd_fb =GUI_GRAM_Alloc(VLCD_W*VLCD_H*2);
			if(vlcd_fb!=NULL)
			{
				memset(vlcd_fb,0xFF,VLCD_W*VLCD_H*2);
			}
			else
			{
				return FALSE;
			}

			GetClientRect(hwnd,&rc); //获得窗口的客户区矩形.

			CreateWindow(BUTTON,L"OK",WS_VISIBLE,rc.w-80,8,68,32,hwnd,ID_OK,NULL,NULL); //创建一个按钮(示例).
		}
		return TRUE;
		////

		case WM_NOTIFY: //WM_NOTIFY消息:wParam低16位为发送该消息的控件ID,高16位为通知码;lParam指向了一个NMHDR结构体.
		{
			u16 code,id;

			code =HIWORD(wParam); //获得通知码类型.
			id   =LOWORD(wParam); //获得产生该消息的控件ID.

			if(id==ID_OK && code==BN_CLICKED) // 按钮“单击”了.
			{
				PostCloseMessage(hwnd); //使产生WM_CLOSE消息关闭窗口.
			}
		}
		break;
		////

		case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
		{
			PAINTSTRUCT ps;
			HDC hdc;
			BITMAP bm;
			hdc =BeginPaint(hwnd,&ps); //开始绘图

			GetClientRect(hwnd,&rc);
			back_init(hdc,rc.w,rc.h);

			memset(vlcd_fb,0xFF,VLCD_W*VLCD_H*2);
			draw();

			////用户的绘制内容...
			//TextOut(hdc,10,10,L"Hello",-1);
			bm.Format =BM_RGB565;
			bm.Width =VLCD_W;
			bm.Height =VLCD_H;
			bm.WidthBytes =VLCD_W*2;
			bm.Bits =vlcd_fb;
			bm.LUT =NULL;
			DrawBitmap(hdc,10,20,&bm,NULL);

			EndPaint(hwnd,&ps); //结束绘图
		}
		break;
		////

		case WM_CLOSE: //窗口关闭时，会自动产生该消息.
		{
			//vfree(vlcd_fb);
			GUI_GRAM_Free(vlcd_fb);
			return DestroyWindow(hwnd); //调用DestroyWindow函数销毁窗口，该函数会使主窗口结束并退出消息循环;否则窗口将继续运行.
		}
		break;
		////

		default: //用户不关心的消息,由系统处理.
		{
			return DefWindowProc(hwnd,msg,wParam,lParam);
		}

	}

	return WM_NULL;
}

/*============================================================================*/

//static void	AppMain(void)
void	GUI_DEMO_AA(void)
{
	HWND	hwnd;
	WNDCLASS	wcex;
	MSG msg;

	/////

	wcex.Tag 		    = WNDCLASS_TAG;

	wcex.Style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WinProc; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= NULL;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;

	//创建主窗口
	hwnd	=CreateWindowEx(	WS_EX_FRAMEBUFFER,
								&wcex,
								_T("emXGUI Window"), //窗口名称
								WS_CAPTION|WS_DLGFRAME|WS_BORDER|WS_CLIPCHILDREN,
								10,20,600,500,    //窗口位置和大小
								NULL,NULL,NULL,NULL);

	//显示主窗口
	ShowWindow(hwnd,SW_SHOW);

	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while(GetMessage(&msg,hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

/*============================================================================*/

#endif
