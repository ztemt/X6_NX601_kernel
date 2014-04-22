#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/device.h>
#include <linux/sysdev.h>
#include <linux/timer.h>
#include <linux/err.h>
#include <linux/ctype.h>
#include "zte_disp_enhance.h"

#define ZTE_DISP_ENHANCE_DEBUG

#if defined(CONFIG_ZTEMT_MIPI_1080P_R63311_SHARP_IPS_6P4) || defined(CONFIG_ZTEMT_MIPI_1080P_R63311_SHARP_IPS_5P5)
#define ZTE_SHARP_R63311_IPS
#endif

#ifdef ZTE_SHARP_R63311_IPS
#define ZTE_SHARP_ENHANCE_CMD_COUNT 1
#define ZTE_SHARP_COLORTMP_CMD_COUNT 3
#define ZTE_SHARP_WARM_GAMA_MAX_BLLEVEL 3212 //4095 
#define ZTE_SHARP_NOR_GAMA_MAX_BLLEVEL  3694 //4095 
#define ZTE_SHARP_COOL_GAMA_MAX_BLLEVEL 4048 //4095 
#define ZTE_SHARP_WARM_BL_OFF 88 //-96 
#define ZTE_SHARP_NOR_BL_OFF  102 //33 
#define ZTE_SHARP_COOL_BL_OFF 111 //113 
#endif

static struct zte_enhance_type zte_enhance_val ={
  .en_saturation =1,
#ifdef CONFIG_ZTEMT_MIPI_1080P_R63311_SHARP_IPS_6P4
  .en_colortmp = 1,
#else
  .en_colortmp = 0,
#endif
  .saturation = INTENSITY_01,
  .colortmp =  INTENSITY_NORMAL,
  .cur_bl_max = ZTE_SHARP_WARM_GAMA_MAX_BLLEVEL,
  .cur_bl_off = ZTE_SHARP_WARM_BL_OFF
};

static struct mdss_dsi_ctrl_pdata *zte_mdss_dsi_ctrl = NULL;

void zte_set_ctrl_point(struct mdss_dsi_ctrl_pdata * ctrl)
{
  zte_mdss_dsi_ctrl = ctrl;
}
static void zte_set_max_bllevel(unsigned int max_bl)
{
  if(max_bl < 255)
     return;
  zte_enhance_val.cur_bl_max = max_bl;
}

struct zte_enhance_type zte_get_lcd_enhance_param(void)
{
  return zte_enhance_val;
}

unsigned int zte_get_lcd_bl_off(void)
{
  return zte_enhance_val.cur_bl_off;
}

#ifdef ZTE_SHARP_R63311_IPS
static char sharpca_basic_9b[] = {
  0xca,0x01,0x80,0x9c,0x9b,0x9b,0x40,0xbe,0xbe,0x20,
  0x20,0x80,0xfe,0x0a,0x4a,0x37,0xa0,0x55,0xf8,0x0c,
  0x0c,0x20,0x10,0x3f,0x3f,0x00,0x00,0x10,0x10,0x3f,
  0x3f,0x3f,0x3f,
};
static struct dsi_cmd_desc sharp_1080p_display_9b = {
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(sharpca_basic_9b)}, sharpca_basic_9b

};

static char sharpca_basic_92[] = {
  0xca,0x01,0x80,0x92,0x92,0x9b,0x75,0x9b,0x9b,0x20,
  0x20,0x80,0xfe,0x0a,0x4a,0x37,0xa0,0x55,0xf8,0x0c,
  0x0c,0x20,0x10,0x3f,0x3f,0x00,0x00,0x10,0x10,0x3f,
  0x3f,0x3f,0x3f,
};

static struct dsi_cmd_desc sharp_1080p_display_92 = {
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(sharpca_basic_92)}, sharpca_basic_92

};
static char sharpca_basic_norm[] = {
  0xca,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x08,
  0x20,0x80,0x80,0x0a,0x4a,0x37,0xa0,0x55,0xf8,0x0c,
  0x0c,0x20,0x10,0x3f,0x3f,0x00,0x00,0x10,0x10,0x3f,
  0x3f,0x3f,0x3f,
};

static struct dsi_cmd_desc sharp_1080p_display_norm = {
	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(sharpca_basic_norm)}, sharpca_basic_norm

};
//warm gama
static char sharp_gama_warm_c7[] ={
 0xc7,0x0F,0x15,0x1A,0x21,0x2E,0x48,0x3B,0x4D,0x5A,
 0x63,0x6A,0x6F,0x11,0x17,0x1C,0x22,0x2E,0x44,0x39,
 0x4E,0x5C,0x64,0x6B,0x70
};
static char sharp_gama_warm_c8[] ={
 0xc8,0x0F,0x14,0x19,0x20,0x2D,0x47,0x3B,0x4C,0x5A,
 0x63,0x6B,0x74,0x11,0x16,0x1B,0x22,0x2E,0x43,0x39,
 0x4E,0x5B,0x64,0x6C,0x75
};
static char sharp_gama_warm_c9[] ={
 0xc9,0x13,0x18,0x1C,0x23,0x2F,0x48,0x3B,0x4D,0x5B,
 0x64,0x6C,0x70,0x15,0x1A,0x1E,0x24,0x30,0x44,0x3A,
 0x4E,0x5C,0x65,0x6D,0x71
};
static struct dsi_cmd_desc sharp_1080p_gama_warm[] = {
	{{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(sharp_gama_warm_c7)}, sharp_gama_warm_c7},
	{{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(sharp_gama_warm_c8)}, sharp_gama_warm_c8},
	{{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(sharp_gama_warm_c9)}, sharp_gama_warm_c9},
};

//cool gama new
static char sharp_gama_nor_c7[] ={
 0xc7, 0x2A, 0x2C, 0x2F, 0x33, 0x3B, 0x4D, 0x3E, 0x50, 0x5C, 
 0x65, 0x6C, 0x6F, 0x2D, 0x2F, 0x30, 0x34, 0x3C, 0x4B, 0x3C,
 0x51, 0x5D, 0x66, 0x6D, 0x70
};

static char sharp_gama_nor_c8[] ={
 0xc8, 0x21, 0x24, 0x28, 0x2C, 0x36, 0x4A, 0x3C, 0x4E, 0x5B,
 0x64, 0x6C, 0x74, 0x23, 0x26, 0x29, 0x2D, 0x36, 0x48, 0x3A,
 0x4E, 0x5B, 0x65, 0x6D, 0x75
};

static char sharp_gama_nor_c9[] ={
 0xc9, 0x13, 0x18, 0x1C, 0x23, 0x2F, 0x48, 0x3B, 0x4D, 0x5A,
 0x64, 0x6C, 0x70, 0x15, 0x1A, 0x1E, 0x24, 0x30, 0x44, 0x3A,
 0x4E, 0x5C, 0x66, 0x6D, 0x71
};


static struct dsi_cmd_desc sharp_1080p_gama_nor[] = {
	{{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(sharp_gama_nor_c7)}, sharp_gama_nor_c7},
	{{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(sharp_gama_nor_c8)}, sharp_gama_nor_c8},
	{{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(sharp_gama_nor_c9)}, sharp_gama_nor_c9},
};

//cool gama
static char sharp_gama_cool_c7[] ={
 0xc7,0x2D,0x30,0x32,0x36,0x3D,0x4F,0x3F,0x51,0x5D,
 0x66,0x6C,0x6F,0x30,0x32,0x34,0x38,0x3E,0x4C,0x3D,
 0x51,0x5E,0x67,0x6D,0x70  

};

static char sharp_gama_cool_c8[] ={
 0xc8,0x29,0x2C,0x2E,0x33,0x3B,0x4D,0x3D,0x4F,0x5C,
 0x65,0x6C,0x74,0x2C,0x2E,0x31,0x34,0x3B,0x4A,0x3B,
 0x4F,0x5D,0x66,0x6E,0x75
};

static char sharp_gama_cool_c9[] ={
 0xc9,0x13,0x18,0x1C,0x23,0x2F,0x48,0x3B,0x4D,0x5A,
 0x64,0x6C,0x70,0x15,0x1A,0x1E,0x24,0x30,0x44,0x3A,
 0x4E,0x5C,0x66,0x6D,0x71
};
static struct dsi_cmd_desc sharp_1080p_gama_cool[] = {
	{{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(sharp_gama_cool_c7)}, sharp_gama_cool_c7},
	{{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(sharp_gama_cool_c8)}, sharp_gama_cool_c8},
	{{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(sharp_gama_cool_c9)}, sharp_gama_cool_c9},
};
#endif

void zte_send_cmd(struct dsi_cmd_desc *cmds,int cmdcount)
{
  struct dcs_cmd_req cmdreq;

  if( (!zte_mdss_dsi_ctrl) || (cmdcount < 1))
  {
    pr_err("lcd:faild:%s zte_mdss_dsi_ctrl is null\n",__func__);
    return;
  }

  memset(&cmdreq, 0, sizeof(cmdreq));
  cmdreq.cmds = cmds;
  cmdreq.cmds_cnt = cmdcount;
	cmdreq.flags = CMD_REQ_COMMIT;
	cmdreq.rlen = 0;
	cmdreq.cb = NULL;

  mdss_dsi_cmdlist_put(zte_mdss_dsi_ctrl, &cmdreq);
}

void zte_mipi_saturation(void)
{

	unsigned int value;
	value =zte_enhance_val.saturation;

  if(!zte_enhance_val.en_saturation)
     return;
	//printk("lcd:%s value=%d\n", __func__, value);
#ifdef ZTE_SHARP_R63311_IPS
	switch (value) {
	case INTENSITY_NORMAL:
		zte_send_cmd(&sharp_1080p_display_norm,ZTE_SHARP_ENHANCE_CMD_COUNT);
		break;
	case INTENSITY_01:
		zte_send_cmd(&sharp_1080p_display_92,ZTE_SHARP_ENHANCE_CMD_COUNT);
		break;
	case INTENSITY_02:
		zte_send_cmd(&sharp_1080p_display_9b,ZTE_SHARP_ENHANCE_CMD_COUNT);
		break;
	default:
		zte_send_cmd(&sharp_1080p_display_92,ZTE_SHARP_ENHANCE_CMD_COUNT);
		break;

	}
#endif
}

static ssize_t saturation_store(struct kobject *kobj, struct kobj_attribute *attr,
    const char *buf, size_t size)
{
	ssize_t ret = 0;
	int val;
	static bool is_firsttime = true;

  if(!zte_enhance_val.en_saturation)
     return ret;

	sscanf(buf, "%d", &val);

#ifdef ZTE_DISP_ENHANCE_DEBUG
	printk("lcd:%s state=%d size=%d\n", __func__, (int)val, (int)size);
#endif
  zte_enhance_val.saturation =val;

	if (is_firsttime) {
		is_firsttime = false;
		return ret;
	}
	zte_mipi_saturation();

	return ret;
}

void zte_mipi_colortmp(void)
{

  unsigned int value;
	value =zte_enhance_val.colortmp;

  if(!zte_enhance_val.en_colortmp)
     return ;
	//printk("lcd:%s value=%d\n", __func__, value);
#ifdef ZTE_SHARP_R63311_IPS
	switch (value) {
	case INTENSITY_NORMAL:
		zte_send_cmd(sharp_1080p_gama_warm,ZTE_SHARP_COLORTMP_CMD_COUNT);
    zte_set_max_bllevel(ZTE_SHARP_WARM_GAMA_MAX_BLLEVEL);
    zte_enhance_val.cur_bl_off = ZTE_SHARP_WARM_BL_OFF;
		break;
	case INTENSITY_01:
		zte_send_cmd(sharp_1080p_gama_nor,ZTE_SHARP_COLORTMP_CMD_COUNT);
    zte_set_max_bllevel(ZTE_SHARP_NOR_GAMA_MAX_BLLEVEL); 
    zte_enhance_val.cur_bl_off = ZTE_SHARP_NOR_BL_OFF; 
		break;
	case INTENSITY_02:
		zte_send_cmd(sharp_1080p_gama_cool,ZTE_SHARP_COLORTMP_CMD_COUNT);
    zte_set_max_bllevel(ZTE_SHARP_COOL_GAMA_MAX_BLLEVEL); 
    zte_enhance_val.cur_bl_off = ZTE_SHARP_COOL_BL_OFF; 
		break;
	default:
		zte_send_cmd(sharp_1080p_gama_warm,ZTE_SHARP_COLORTMP_CMD_COUNT);
    zte_set_max_bllevel(ZTE_SHARP_WARM_GAMA_MAX_BLLEVEL);  
    zte_enhance_val.cur_bl_off = ZTE_SHARP_WARM_BL_OFF;
		break;

	}
#endif
}


static ssize_t colortmp_store(struct kobject *kobj, struct kobj_attribute *attr,
    const char *buf, size_t size)
{
	ssize_t ret = 0;
	int val;
	static bool is_firsttime = true;

  if(!zte_enhance_val.en_colortmp)
     return ret;

	sscanf(buf, "%d", &val);

#ifdef ZTE_DISP_ENHANCE_DEBUG
	printk("lcd:%s state=%d size=%d\n", __func__, (int)val, (int)size);
#endif
	
  zte_enhance_val.colortmp = val;

	if (is_firsttime) {
		is_firsttime = false;
		return ret;
	}
	zte_mipi_colortmp();

	return ret;
}

void zte_boot_begin_enhance(struct mdss_dsi_ctrl_pdata *ctrl_pdata)
{
	static bool is_firsttime = true;

  if(is_firsttime){
     printk("lcd:%s \n",__func__);
     is_firsttime = false;
     zte_mdss_dsi_ctrl = ctrl_pdata;   
     zte_mipi_saturation();
     zte_mipi_colortmp();
  }
}

static struct kobj_attribute saturation_attribute = __ATTR(saturation, 0664, NULL, saturation_store);
static struct kobj_attribute colortmp_attribute = __ATTR(colortmp, 0664, NULL, colortmp_store);
static struct attribute *enhance_attrs[] = {
  &saturation_attribute.attr,
  &colortmp_attribute.attr,
  NULL, /* need to NULL terminate the list of attributes */
};
static struct attribute_group enhance_attr_group = {
  .attrs = enhance_attrs,
};
 
static struct kobject *enhance__kobj;


static int __init enhance_init(void)
{
  int retval;
 
  enhance__kobj = kobject_create_and_add("lcd_enhance", kernel_kobj);
  if (!enhance__kobj)
   return -ENOMEM;
 
  /* Create the files associated with this kobject */
  retval = sysfs_create_group(enhance__kobj, &enhance_attr_group);
  if (retval)
   kobject_put(enhance__kobj);

  zte_mdss_dsi_ctrl = NULL;
 
  return retval;
}
 
static void __exit enhance_exit(void)
{
  kobject_put(enhance__kobj);
}
 
 module_init(enhance_init);
 module_exit(enhance_exit);
 
