/*
 * linux/sound/soc/codecs/tlv320aic3262.h
 *
 *
 * Copyright (C) 2011 Mistral Solutions Pvt Ltd.
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * History:
 *  Rev 0.1   ASoC driver support    Mistral         20-01-2011
 *
 * The AIC3262 ASoC driver is ported for the codec AIC3262.
 *
 */

#ifndef _TLV320AIC3262_H
#define _TLV320AIC3262_H

#define AUDIO_NAME "aic3262"
#define AIC3262_VERSION "1.1"
/* Macro to enable the inclusion of tiload kernel driver */
/*#define AIC3262_TiLoad*/


/* Macro for McBsp master / slave configuration */
#define AIC3262_MCBSP_SLAVE
/*#undef AIC3262_MCBSP_SLAVE*/

/* Enable this macro allow for different ASI formats */
/*#define ASI_MULTI_FMT*/
#undef ASI_MULTI_FMT
/* Enable register caching on write */
#define EN_REG_CACHE

/* Enable Headset Detection */
/*#define HEADSET_DETECTION*/
#undef HEADSET_DETECTION

/* Enable or disable controls to have Input routing*/
/*#define FULL_IN_CNTL*/
#undef FULL_IN_CNTL
/* AIC3262 supported sample rate are 8k to 192k */
#define AIC3262_RATES	SNDRV_PCM_RATE_8000_192000

/* AIC3262 supports the word formats 16bits, 20bits, 24bits and 32 bits */
#define AIC3262_FORMATS	(SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S20_3LE \
			 | SNDRV_PCM_FMTBIT_S24_3LE | SNDRV_PCM_FMTBIT_S32_LE)

#define AIC3262_FREQ_12000000 12000000
#define AIC3262_FREQ_24000000 24000000

/* Audio data word length = 16-bits (default setting) */
#define AIC3262_WORD_LEN_16BITS		0x00
#define AIC3262_WORD_LEN_20BITS		0x01
#define AIC3262_WORD_LEN_24BITS		0x02
#define AIC3262_WORD_LEN_32BITS		0x03

/* sink: name of target widget */
#define AIC3262_WIDGET_NAME			0
/* control: mixer control name */
#define AIC3262_CONTROL_NAME		1
/* source: name of source name */
#define AIC3262_SOURCE_NAME			2

/* D15..D8 aic3262 register offset */
#define AIC3262_REG_OFFSET_INDEX    0
/* D7...D0 register data */
#define AIC3262_REG_DATA_INDEX      1

/* Serial data bus uses I2S mode (Default mode) */
#define AIC3262_I2S_MODE				0x00
#define AIC3262_DSP_MODE				0x01
#define AIC3262_RIGHT_JUSTIFIED_MODE	0x02
#define AIC3262_LEFT_JUSTIFIED_MODE		0x03

/* 8 bit mask value */
#define AIC3262_8BITS_MASK           0xFF

/* shift value for CLK_REG_3 register */
#define CLK_REG_3_SHIFT					6
/* shift value for DAC_OSR_MSB register */
#define DAC_OSR_MSB_SHIFT				4

/* number of codec specific register for configuration */
#define NO_FEATURE_REGS			2

/* AIC3262 register space */
/* Updated from 256 to support Page 3 registers */
#define	AIC3262_CACHEREGNUM	1024

/* ****************** Book 0 Registers **************************************/

/* ****************** Page 0 Registers **************************************/

#define PAGE_SEL_REG		0
#define RESET_REG		1
#define DAC_ADC_CLKIN_REG	4
#define PLL_CLKIN_REG		5
#define PLL_CLK_RANGE_REG	5
#define PLL_PR_POW_REG		6
#define PLL_J_REG		7
#define PLL_D_MSB		8
#define PLL_D_LSB		9
#define PLL_CKIN_DIV		10

#define NDAC_DIV_POW_REG	11
#define MDAC_DIV_POW_REG	12
#define DOSR_MSB_REG		13
#define DOSR_LSB_REG		14

#define NADC_DIV_POW_REG	18
#define MADC_DIV_POW_REG	19
#define AOSR_REG		20
#define CLKOUT_MUX		21
#define CLKOUT_MDIV_VAL		22
#define TIMER_REG		23

#define LF_CLK_CNTL		24
#define HF_CLK_CNTL_R1		25
#define HF_CLK_CNTL_R2		26
#define HF_CLK_CNTL_R3		27
#define HF_CLK_CNTL_R4		28
#define HF_CLK_TRIM_R1		29
#define HF_CLK_TRIM_R2		30
#define HF_CLK_TRIM_R3		31
#define HF_CLK_TRIM_R4		32

#define STICKY_FLAG2		44
#define INT_FLAG2		46
#define INT1_CNTL		48
#define INT2_CNTL		49
#define INT_FMT			51

#define DAC_PRB			60
#define ADC_PRB			61
#define PASI_DAC_DP_SETUP	63
#define DAC_MVOL_CONF		64
#define DAC_LVOL		65
#define DAC_RVOL		66
#define HP_DETECT		67
#define DRC_CNTL_R1		68
#define DRC_CNTL_R2		69
#define DRC_CNTL_R3		70
#define BEEP_CNTL_R1		71
#define BEEP_CNTL_R2		72

#define ADC_CHANNEL_POW		81
#define ADC_FINE_GAIN		82
#define LADC_VOL		83
#define RADC_VOL		84
#define ADC_PHASE		85

#define LAGC_CNTL		86
#define LAGC_CNTL_R2		87
#define LAGC_CNTL_R3		88
#define LAGC_CNTL_R4		89
#define LAGC_CNTL_R5		90
#define LAGC_CNTL_R6		91
#define LAGC_CNTL_R7		92
#define LAGC_CNTL_R8		93

#define RAGC_CNTL		94
#define RAGC_CNTL_R2		95
#define RAGC_CNTL_R3		96
#define RAGC_CNTL_R4		97
#define RAGC_CNTL_R5		98
#define RAGC_CNTL_R6		99
#define RAGC_CNTL_R7		100
#define RAGC_CNTL_R8		101
#define MINIDSP_ACCESS_CTRL	121
/* ****************** Page 1 Registers **************************************/
#define PAGE_1			128

#define POWER_CONF		(PAGE_1 + 1)
#define LDAC_PTM		(PAGE_1 + 3)
#define RDAC_PTM		(PAGE_1 + 4)
#define CM_REG			(PAGE_1 + 8)
#define HP_CTL			(PAGE_1 + 9)
#define HP_DEPOP		(PAGE_1 + 11)
#define RECV_DEPOP		(PAGE_1 + 12)
#define MA_CNTL			(PAGE_1 + 17)
#define LADC_PGA_MAL_VOL	(PAGE_1 + 18)
#define RADC_PGA_MAR_VOL	(PAGE_1 + 19)


#define LINE_AMP_CNTL_R1	(PAGE_1 + 22)
#define LINE_AMP_CNTL_R2	(PAGE_1 + 23)

#define HP_AMP_CNTL_R1		(PAGE_1 + 27)
#define HP_AMP_CNTL_R2		(PAGE_1 + 28)
#define HP_AMP_CNTL_R3		(PAGE_1 + 29)

#define HPL_VOL			(PAGE_1 + 31)
#define HPR_VOL			(PAGE_1 + 32)
#define INT1_SEL_L		(PAGE_1 + 34)
#define RAMP_CNTL_R1		(PAGE_1 + 36)
#define RAMP_CNTL_R2		(PAGE_1 + 37)
#define INT1_SEL_RM		(PAGE_1 + 39)
#define REC_AMP_CNTL_R5		(PAGE_1 + 40)
#define RAMPR_VOL		(PAGE_1 + 41)
#define RAMP_TIME_CNTL		(PAGE_1 + 42)
#define SPK_AMP_CNTL_R1		(PAGE_1 + 45)
#define SPK_AMP_CNTL_R2		(PAGE_1 + 46)
#define SPK_AMP_CNTL_R3		(PAGE_1 + 47)
#define SPK_AMP_CNTL_R4		(PAGE_1 + 48)
#define MIC_BIAS_CNTL		(PAGE_1 + 51)

#define LMIC_PGA_PIN		(PAGE_1 + 52)
#define LMIC_PGA_PM_IN4		(PAGE_1 + 53)
#define LMIC_PGA_MIN		(PAGE_1 + 54)
#define RMIC_PGA_PIN		(PAGE_1 + 55)
#define RMIC_PGA_PM_IN4		(PAGE_1 + 56)
#define RMIC_PGA_MIN		(PAGE_1 + 57)
/* MIC PGA Gain Registers */
#define MICL_PGA		(PAGE_1 + 59)
#define MICR_PGA		(PAGE_1 + 60)
#define MIC_PWR_DLY		(PAGE_1 + 121)
#define REF_PWR_DLY		(PAGE_1 + 122)

/* ****************** Page 4 Registers **************************************/
#define PAGE_4			512
#define ASI1_BUS_FMT		(PAGE_4 + 1)
#define ASI1_LCH_OFFSET		(PAGE_4 + 2)
#define ASI1_RCH_OFFSET		(PAGE_4 + 3)
#define ASI1_CHNL_SETUP		(PAGE_4 + 4)
#define ASI1_MULTI_CH_SETUP_R1	(PAGE_4 + 5)
#define ASI1_MULTI_CH_SETUP_R2	(PAGE_4 + 6)
#define ASI1_ADC_INPUT_CNTL	(PAGE_4 + 7)
#define ASI1_DAC_OUT_CNTL	(PAGE_4 + 8)
#define ASI1_ADC_OUT_TRISTATE	(PAGE_4 + 9)
#define ASI1_BWCLK_CNTL_REG	(PAGE_4 + 10)
#define ASI1_BCLK_N_CNTL	(PAGE_4 + 11)
#define ASI1_BCLK_N		(PAGE_4 + 12)
#define ASI1_WCLK_N		(PAGE_4 + 13)
#define ASI1_BWCLK_OUT_CNTL	(PAGE_4 + 14)
#define ASI2_BUS_FMT		(PAGE_4 + 17)
#define ASI2_BWCLK_CNTL_REG	(PAGE_4 + 26)
#define ASI2_BCLK_N_CNTL	(PAGE_4 + 27)
#define ASI2_BCLK_N		(PAGE_4 + 28)
#define ASI2_WCLK_N		(PAGE_4 + 29)
#define ASI2_BWCLK_OUT_CNTL	(PAGE_4 + 30)
#define GPIO1_IO_CNTL		(PAGE_4 + 86)
#define GPIO2_IO_CNTL		(PAGE_4 + 87)
#define GPI1_EN			(PAGE_4 + 91)
#define GPO2_EN			(PAGE_4 + 92)
#define GPO1_OUT_CNTL		(PAGE_4 + 96)

/****************************             ************************************/

/*
 *****************************************************************************
 * Structures Definitions
 *****************************************************************************
 */
/*
 *----------------------------------------------------------------------------
 * @struct  aic3262_setup_data |
 *          i2c specific data setup for AIC3262.
 * @field   unsigned short |i2c_address |
 *          Unsigned short for i2c address.
 *----------------------------------------------------------------------------
 */
struct aic3262_setup_data {
	unsigned short i2c_address;
};

/*
 *----------------------------------------------------------------------------
 * @struct  aic3262_priv |
 *          AIC3262 priviate data structure to set the system clock, mode and
 *          page number.
 * @field   u32 | sysclk |
 *          system clock
 * @field   s32 | master |
 *          master/slave mode setting for AIC3262
 * @field   u8 | book_no |
 *          book number.
 * @field   u8 | page_no |
 *          page number. Here, page 0 and page 1 are used.
 *----------------------------------------------------------------------------
 */
struct aic3262_priv {
	u32 sysclk;
	s32 master;
	u8 book_no;
	u8 page_no;
};

/*
 *----------------------------------------------------------------------------
 * @struct  aic3262_configs |
 *          AIC3262 initialization data which has register offset and register
 *          value.
 * @field   u8 | book_no |
 *          AIC3262 Book Number Offsets required for initialization..
 * @field   u16 | reg_offset |
 *          AIC3262 Register offsets required for initialization..
 * @field   u8 | reg_val |
 *          value to set the AIC3262 register to initialize the AIC3262.
 *----------------------------------------------------------------------------
 */
struct aic3262_configs {
	u8 book_no;
	u16 reg_offset;
	u8  reg_val;
};

/*
 *----------------------------------------------------------------------------
 * @struct  aic3262_rate_divs |
 *          Setting up the values to get different freqencies
 *
 * @field   u32 | mclk |
 *          Master clock
 * @field   u32 | rate |
 *          sample rate
 * @field   u8 | p_val |
 *          value of p in PLL
 * @field   u32 | pll_j |
 *          value for pll_j
 * @field   u32 | pll_d |
 *          value for pll_d
 * @field   u32 | dosr |
 *          value to store dosr
 * @field   u32 | ndac |
 *          value for ndac
 * @field   u32 | mdac |
 *          value for mdac
 * @field   u32 | aosr |
 *          value for aosr
 * @field   u32 | nadc |
 *          value for nadc
 * @field   u32 | madc |
 *          value for madc
 * @field   u32 | blck_N |
 *          value for block N
 * @field   u32 | aic3262_configs |
 *          configurations for aic3262 register value
 *----------------------------------------------------------------------------
 */
struct aic3262_rate_divs {
	u32 mclk;
	u32 rate;
	u8 p_val;
	u8 pll_j;
	u16 pll_d;
	u16 dosr;
	u8 ndac;
	u8 mdac;
	u8 aosr;
	u8 nadc;
	u8 madc;
	u8 blck_N;
	struct aic3262_configs codec_specific_regs[NO_FEATURE_REGS];
};

/*
 *----------------------------------------------------------------------------
 * @struct  snd_soc_codec_dai |
 *          It is SoC Codec DAI structure which has DAI capabilities viz.,
 *          playback and capture, DAI runtime information viz. state of DAI
 *			and pop wait state, and DAI private data.
 *----------------------------------------------------------------------------
 */
extern struct snd_soc_dai tlv320aic3262_dai;

/*
 *----------------------------------------------------------------------------
 * @struct  snd_soc_codec_device |
 *          This structure is soc audio codec device sturecute which pointer
 *          to basic functions aic3262_probe(), aic3262_remove(),
 *			aic3262_suspend() and aic3262_resume()
 *
 */
extern struct snd_soc_codec_device soc_codec_dev_aic3262;

#endif				/* _TLV320AIC3262_H */


