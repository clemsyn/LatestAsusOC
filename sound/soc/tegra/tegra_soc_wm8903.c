/*
 * tegra_soc_wm8903.c  --  SoC audio for tegra
 *
 * (c) 2010-2011 Nvidia Graphics Pvt. Ltd.
 *  http://www.nvidia.com
 *
 * Copyright 2007 Wolfson Microelectronics PLC.
 * Author: Graeme Gregory
 *         graeme.gregory@wolfsonmicro.com or linux@wolfsonmicro.com
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 */

#include "tegra_soc.h"
#include <linux/gpio.h>
#include <sound/soc-dapm.h>
#include <linux/regulator/consumer.h>
#include "../codecs/wm8903.h"
#include "../codecs/codec_param.h"

static struct platform_device *tegra_snd_device;

extern struct snd_soc_dai tegra_i2s_dai[];
extern struct snd_soc_dai tegra_spdif_dai;
extern struct snd_soc_dai tegra_generic_codec_dai[];
extern struct snd_soc_platform tegra_soc_platform;
extern struct wired_jack_conf tegra_wired_jack_conf;
extern bool jack_alive;
extern bool lineout_alive;
extern int check_hs_type();
extern int fm34_config_DSP();
extern struct wm8903_parameters audio_params[];

int mic_type;
EXPORT_SYMBOL(mic_type);

#define MIC_DIGITAL 0
#define MIC_ANALOG 1
#define MIC_INACTIVE 2

/* codec register values */
#define B07_INEMUTE		7
#define B06_VOL_M3DB		6
#define B00_IN_VOL		0
#define B00_INR_ENA		0
#define B01_INL_ENA		1
#define R06_MICBIAS_CTRL_0	6
#define B07_MICDET_HYST_ENA	7
#define B04_MICDET_THR		4
#define B02_MICSHORT_THR	2
#define B01_MICDET_ENA		1
#define B00_MICBIAS_ENA		0
#define B15_DRC_ENA		15
#define B03_DACL_ENA		3
#define B02_DACR_ENA		2
#define B01_ADCL_ENA		1
#define B00_ADCR_ENA		0
#define B06_IN_CM_ENA		6
#define B04_IP_SEL_N		4
#define B02_IP_SEL_P		2
#define B00_MODE 		0
#define B06_AIF_ADCL		7
#define B06_AIF_ADCR		6
#define B05_ADC_HPF_CUT		5
#define B04_ADC_HPF_ENA		4
#define B01_ADCL_DATINV		1
#define B00_ADCR_DATINV		0
#define R20_SIDETONE_CTRL	32
#define R29_DRC_1		41
#define R18_AUDIO_INTERFACE_0   24
#define B12_DACL_DATINV         12
#define B11_DACR_DATINV         11
#define B09_DAC_BOOST           9
#define B08_LOOPBACK            8
#define B07_AIFADCL_SRC         7
#define B06_AIFADCR_SRC         6
#define B05_AIFDACL_SRC         5
#define B04_AIFDACR_SRC         4
#define B03_ADC_COMP            3
#define B02_ADC_COMPMODE        2
#define B01_DAC_COMP            1
#define B00_DAC_COMPMODE        0
#define R74_GPIO_CTRL_1         116
#define R75_GPIO_CTRL_2         117
#define R76_GPIO_CTRL_3         118
#define R77_GPIO_CTRL_4         119     /* Interupt */
#define B08_GPIO_FN             8
#define B07_GPIO_DIR            7
#define B06_GPIO_OP_CFG         6
#define B05_GPIO_IP_CFG         5
#define B04_GPIO_LVL            4
#define B03_GPIO_PD             3
#define B02_GPIO_PU             2
#define B01_GPIO_INTMODE        1
#define B00_GPIO_DB             0
#define RA4_ADC_DIG_MIC         164
#define B09_DIGMIC              9
#define SET_REG_VAL(r,m,l,v) (((r)&(~((m)<<(l))))|(((v)&(m))<<(l)))


static int tegra_hifi_hw_params(struct snd_pcm_substream *substream,
					struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *codec_dai = rtd->dai->codec_dai;
	struct snd_soc_dai *cpu_dai = rtd->dai->cpu_dai;
	struct snd_soc_codec *codec = codec_dai->codec;
	struct tegra_audio_data* audio_data = rtd->socdev->codec_data;
	enum dac_dap_data_format data_fmt;
	int dai_flag = 0, sys_clk;
	int err;
	int hs_type;
	int CtrlReg = 0;

	if (tegra_das_is_port_master(tegra_audio_codec_type_hifi))
		dai_flag |= SND_SOC_DAIFMT_CBM_CFM;
	else
		dai_flag |= SND_SOC_DAIFMT_CBS_CFS;

	data_fmt = tegra_das_get_codec_data_fmt(tegra_audio_codec_type_hifi);

	/* We are supporting DSP and I2s format for now */
	if (data_fmt & dac_dap_data_format_i2s)
		dai_flag |= SND_SOC_DAIFMT_I2S;
	else
		dai_flag |= SND_SOC_DAIFMT_DSP_A;

	err = snd_soc_dai_set_fmt(codec_dai, dai_flag);
	if (err < 0) {
		pr_err("codec_dai fmt not set \n");
		return err;
	}

	err = snd_soc_dai_set_fmt(cpu_dai, dai_flag);
	if (err < 0) {
		pr_err("cpu_dai fmt not set \n");
		return err;
	}

	sys_clk = clk_get_rate(audio_data->dap_mclk);
	err = snd_soc_dai_set_sysclk(codec_dai, 0, sys_clk, SND_SOC_CLOCK_IN);
	if (err < 0) {
		pr_err("codec_dai clock not set\n");
		return err;
	}

	err = snd_soc_dai_set_sysclk(cpu_dai, 0, sys_clk, SND_SOC_CLOCK_IN);
	if (err < 0) {
		pr_err("cpu_dai clock not set\n");
		return err;
	}

	if (substream->stream != SNDRV_PCM_STREAM_PLAYBACK) {
		int VolumeCtrlReg = 0;

		hs_type = check_hs_type();
		if(jack_alive && hs_type){
		printk("Headset connected, enable external Mic(AMIC)\n");
		mic_type = MIC_ANALOG;

		/* Disable Digital Microphone(DMIC) for audio input function */
		CtrlReg = (0x0 << B08_GPIO_FN) |(0x1 << B07_GPIO_DIR) |(0x1 << B05_GPIO_IP_CFG) |(0x1 << B03_GPIO_PD);
		snd_soc_write(codec, R74_GPIO_CTRL_1, CtrlReg); /*0x00A8*/

		CtrlReg = (0x0 << B08_GPIO_FN) |(0x1 << B07_GPIO_DIR) |(0x1 << B05_GPIO_IP_CFG) |(0x1 << B03_GPIO_PD);
		snd_soc_write(codec, R75_GPIO_CTRL_2, CtrlReg); /*0x00A8*/

		CtrlReg = (0x0 << B09_DIGMIC);
		snd_soc_write(codec, RA4_ADC_DIG_MIC, CtrlReg);/*0x0000*/

		snd_soc_write(codec, WM8903_ANALOGUE_LEFT_INPUT_0, 0X7);
		snd_soc_write(codec, WM8903_ANALOGUE_RIGHT_INPUT_0, 0X7);
		// Mic Bias enable
		CtrlReg = (0x1<<B00_MICBIAS_ENA) | (0x1<<B01_MICDET_ENA);
		snd_soc_write(codec, WM8903_MIC_BIAS_CONTROL_0, CtrlReg);
		// Enable DRC
		CtrlReg = snd_soc_read(codec, WM8903_DRC_0);
		CtrlReg |= (1<<B15_DRC_ENA);
		snd_soc_write(codec, WM8903_DRC_0, CtrlReg);
		// Differential Mic
		CtrlReg = (0x0<<B06_IN_CM_ENA) |
			(0x2<<B00_MODE) | (0x0<<B04_IP_SEL_N)
					| (0x1<<B02_IP_SEL_P);
		VolumeCtrlReg = (audio_params[EP101].analog_headset_mic_volume << B00_IN_VOL);
		// Mic Setting
		snd_soc_write(codec, WM8903_ANALOGUE_LEFT_INPUT_1, CtrlReg);
		snd_soc_write(codec, WM8903_ANALOGUE_RIGHT_INPUT_1, CtrlReg);
		// voulme for single ended mic
		snd_soc_write(codec, WM8903_ANALOGUE_LEFT_INPUT_0,
				VolumeCtrlReg);
		snd_soc_write(codec, WM8903_ANALOGUE_RIGHT_INPUT_0,
				VolumeCtrlReg);
		/* Left ADC data on both channels */
		CtrlReg = snd_soc_read(codec, WM8903_AUDIO_INTERFACE_0);
		CtrlReg  = SET_REG_VAL(CtrlReg, 0x1, B06_AIF_ADCR, 0x0);
		CtrlReg  = SET_REG_VAL(CtrlReg, 0x1, B06_AIF_ADCL, 0x0);
		snd_soc_write(codec, WM8903_AUDIO_INTERFACE_0, CtrlReg);
		}else{
		printk("Headset disconnected, enable internal Mic(DMIC)\n");
		mic_type = MIC_DIGITAL;

		/* Enable Digital Microphone(DMIC) for audio input function */
		CtrlReg = (0x1 << B06_AIFADCR_SRC) |(0x1 << B04_AIFDACR_SRC);
		snd_soc_write(codec, WM8903_AUDIO_INTERFACE_0, CtrlReg); /*0x0050*/

		CtrlReg = (0x6 << B08_GPIO_FN) |(0x0 << B07_GPIO_DIR) |(0x1 << B05_GPIO_IP_CFG) |(0x1 << B03_GPIO_PD);
		snd_soc_write(codec, WM8903_GPIO_CONTROL_1, CtrlReg); /*0x0628*/

		CtrlReg = (0x6 << B08_GPIO_FN) |(0x1 << B07_GPIO_DIR) |(0x1 << B05_GPIO_IP_CFG) |(0x1 << B03_GPIO_PD);
		snd_soc_write(codec, WM8903_GPIO_CONTROL_2, CtrlReg); /*0x06A8*/

		CtrlReg = (0x1 << B09_DIGMIC);
		snd_soc_write(codec, WM8903_CLOCK_RATE_TEST_4, CtrlReg);/*0x2A38*/

		snd_soc_write(codec, WM8903_ADC_DIGITAL_VOLUME_LEFT, audio_params[EP101].analog_DMIC_ADC_volume | 0x100); /* ADC Digital volume left: 17.625dB*/
		snd_soc_write(codec, WM8903_ADC_DIGITAL_VOLUME_RIGHT, audio_params[EP101].analog_DMIC_ADC_volume | 0x100); /* ADC Digital volume right: 17.625dB*/
		}
		//--------------------------------------
		// Enable analog inputs
		CtrlReg = (0x1<<B01_INL_ENA);
		snd_soc_write(codec, WM8903_POWER_MANAGEMENT_0, CtrlReg);
		// ADC Settings
		CtrlReg = snd_soc_read(codec, WM8903_ADC_DIGITAL_0);
		CtrlReg |= (0x1<<B04_ADC_HPF_ENA);
		snd_soc_write(codec, WM8903_ADC_DIGITAL_0, CtrlReg);
		/* Disable sidetone */
		CtrlReg = 0;
		snd_soc_write(codec, R20_SIDETONE_CTRL, CtrlReg);
		// Enable ADC
		CtrlReg = snd_soc_read(codec, WM8903_POWER_MANAGEMENT_6);
		CtrlReg |= (0x1<<B01_ADCL_ENA);
		snd_soc_write(codec, WM8903_POWER_MANAGEMENT_6, CtrlReg);
		CtrlReg = snd_soc_read(codec, R29_DRC_1);
		CtrlReg |= 0x3; //mic volume 18 db
		snd_soc_write(codec, R29_DRC_1, CtrlReg);
	}else{
		mic_type = MIC_INACTIVE;
		/* Mic Bias disable */
		CtrlReg = (0x0<<0) | (0x0<<1);
		snd_soc_write(codec, WM8903_MIC_BIAS_CONTROL_0, CtrlReg);
		fm34_config_DSP();
	}

	return 0;
}

static int tegra_voice_hw_params(struct snd_pcm_substream *substream,
					struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *codec_dai = rtd->dai->codec_dai;
	struct snd_soc_dai *cpu_dai = rtd->dai->cpu_dai;
	struct tegra_audio_data* audio_data = rtd->socdev->codec_data;
	enum dac_dap_data_format data_fmt;
	int dai_flag = 0, sys_clk;
	int err;

	if (tegra_das_is_port_master(tegra_audio_codec_type_bluetooth))
		dai_flag |= SND_SOC_DAIFMT_CBM_CFM;
	else
		dai_flag |= SND_SOC_DAIFMT_CBS_CFS;

	data_fmt = tegra_das_get_codec_data_fmt(tegra_audio_codec_type_bluetooth);

	/* We are supporting DSP and I2s format for now */
	if (data_fmt & dac_dap_data_format_dsp)
		dai_flag |= SND_SOC_DAIFMT_DSP_A;
	else
		dai_flag |= SND_SOC_DAIFMT_I2S;

	err = snd_soc_dai_set_fmt(codec_dai, dai_flag);
	if (err < 0) {
		pr_err("codec_dai fmt not set \n");
		return err;
	}

	err = snd_soc_dai_set_fmt(cpu_dai, dai_flag);
	if (err < 0) {
		pr_err("cpu_dai fmt not set \n");
		return err;
	}

	sys_clk = clk_get_rate(audio_data->dap_mclk);
	err = snd_soc_dai_set_sysclk(codec_dai, 0, sys_clk, SND_SOC_CLOCK_IN);
	if (err < 0) {
		pr_err("cpu_dai clock not set\n");
		return err;
	}

	err = snd_soc_dai_set_sysclk(cpu_dai, 0, sys_clk, SND_SOC_CLOCK_IN);
	if (err < 0) {
		pr_err("cpu_dai clock not set\n");
		return err;
	}

	return 0;
}

static int tegra_spdif_hw_params(struct snd_pcm_substream *substream,
					struct snd_pcm_hw_params *params)
{
	return 0;
}

int tegra_codec_startup(struct snd_pcm_substream *substream)
{
	tegra_das_power_mode(true);

	return 0;
}

void tegra_codec_shutdown(struct snd_pcm_substream *substream)
{
	tegra_das_power_mode(false);
}

int tegra_soc_suspend_pre(struct platform_device *pdev, pm_message_t state)
{
	return 0;
}

int tegra_soc_suspend_post(struct platform_device *pdev, pm_message_t state)
{
	struct snd_soc_device *socdev = platform_get_drvdata(pdev);
	struct tegra_audio_data* audio_data = socdev->codec_data;

	clk_disable(audio_data->dap_mclk);

	return 0;
}

int tegra_soc_resume_pre(struct platform_device *pdev)
{
	struct snd_soc_device *socdev = platform_get_drvdata(pdev);
	struct tegra_audio_data* audio_data = socdev->codec_data;

	clk_enable(audio_data->dap_mclk);

	return 0;
}

int tegra_soc_resume_post(struct platform_device *pdev)
{
	return 0;
}

static struct snd_soc_ops tegra_hifi_ops = {
	.hw_params = tegra_hifi_hw_params,
	.startup = tegra_codec_startup,
	.shutdown = tegra_codec_shutdown,
};

static struct snd_soc_ops tegra_voice_ops = {
	.hw_params = tegra_voice_hw_params,
	.startup = tegra_codec_startup,
	.shutdown = tegra_codec_shutdown,
};

static struct snd_soc_ops tegra_spdif_ops = {
	.hw_params = tegra_spdif_hw_params,
};

void tegra_ext_control(struct snd_soc_codec *codec, int new_con)
{
	struct tegra_audio_data* audio_data = codec->socdev->codec_data;

	audio_data->codec_con = new_con;
}

static int tegra_dapm_event_int_spk(struct snd_soc_dapm_widget* w,
				    struct snd_kcontrol* k, int event)
{
	if (tegra_wired_jack_conf.en_spkr != -1) {
		if (tegra_wired_jack_conf.amp_reg) {
			if (SND_SOC_DAPM_EVENT_ON(event) &&
				!tegra_wired_jack_conf.amp_reg_enabled) {
				regulator_enable(tegra_wired_jack_conf.amp_reg);
				tegra_wired_jack_conf.amp_reg_enabled = 1;
			}
			else if (!SND_SOC_DAPM_EVENT_ON(event) &&
				tegra_wired_jack_conf.amp_reg_enabled) {
				regulator_disable(tegra_wired_jack_conf.amp_reg);
				tegra_wired_jack_conf.amp_reg_enabled = 0;
			}
		}

		gpio_set_value_cansleep(tegra_wired_jack_conf.en_spkr,
			SND_SOC_DAPM_EVENT_ON(event) ? 1 : 0);
	}

	return 0;
}

static int tegra_dapm_event_int_mic(struct snd_soc_dapm_widget* w,
				    struct snd_kcontrol* k, int event)
{
	if (tegra_wired_jack_conf.en_mic_int != -1)
		gpio_set_value_cansleep(tegra_wired_jack_conf.en_mic_int,
			SND_SOC_DAPM_EVENT_ON(event) ? 1 : 0);

	if (tegra_wired_jack_conf.en_mic_ext != -1)
		gpio_set_value_cansleep(tegra_wired_jack_conf.en_mic_ext,
			SND_SOC_DAPM_EVENT_ON(event) ? 0 : 1);

	return 0;
}

static int tegra_dapm_event_ext_mic(struct snd_soc_dapm_widget* w,
				    struct snd_kcontrol* k, int event)
{
	if (tegra_wired_jack_conf.en_mic_ext != -1)
		gpio_set_value_cansleep(tegra_wired_jack_conf.en_mic_ext,
			SND_SOC_DAPM_EVENT_ON(event) ? 1 : 0);

	if (tegra_wired_jack_conf.en_mic_int != -1)
		gpio_set_value_cansleep(tegra_wired_jack_conf.en_mic_int,
			SND_SOC_DAPM_EVENT_ON(event) ? 0 : 1);

	return 0;
}

/*tegra machine dapm widgets */
static const struct snd_soc_dapm_widget tegra_dapm_widgets[] = {
	SND_SOC_DAPM_HP("Headphone", NULL),
	SND_SOC_DAPM_HP("Headset", NULL),
	SND_SOC_DAPM_SPK("Lineout", NULL),
	SND_SOC_DAPM_SPK("Int Spk", tegra_dapm_event_int_spk),
	SND_SOC_DAPM_MIC("Ext Mic", tegra_dapm_event_ext_mic),
	SND_SOC_DAPM_MIC("Int Mic", tegra_dapm_event_int_mic),
	SND_SOC_DAPM_LINE("Linein", NULL),
};

/* Tegra machine audio map (connections to the codec pins) */
static const struct snd_soc_dapm_route audio_map[] = {

	/* headphone connected to LHPOUT1, RHPOUT1 */
	{"Headphone", NULL, "HPOUTR"},
	{"Headphone", NULL, "HPOUTL"},

	/* headset Jack  - in = micin, out = HPOUT*/
	{"Headset", NULL, "HPOUTR"},
	{"Headset", NULL, "HPOUTL"},
	{"IN1L", NULL, "Headset"},

	/* lineout connected to LINEOUTR and LINEOUTL */
	{"Lineout", NULL, "LINEOUTR"},
	{"Lineout", NULL, "LINEOUTL"},

	/* build-in speaker connected to LON/P RON/P */
	{"Int Spk", NULL, "RON"},
	{"Int Spk", NULL, "ROP"},
	{"Int Spk", NULL, "LON"},
	{"Int Spk", NULL, "LOP"},

	/* internal mic is mono */
	{"IN1L", NULL, "Int Mic"},

	/* external mic is stereo */
	{"IN1L", NULL, "Ext Mic"},
	{"IN1R", NULL, "Ext Mic"},

	/* Line In */
	{"IN3L", NULL, "Linein"},
	{"IN3R", NULL, "Linein"},
};


static int tegra_codec_init(struct snd_soc_codec *codec)
{
	struct tegra_audio_data* audio_data = codec->socdev->codec_data;
	int err = 0;

	if (!audio_data->init_done) {
		audio_data->dap_mclk = tegra_das_get_dap_mclk();
		if (!audio_data->dap_mclk) {
			pr_err("Failed to get dap mclk \n");
			err = -ENODEV;
			return err;
		}

		/* Add tegra specific widgets */
		snd_soc_dapm_new_controls(codec, tegra_dapm_widgets,
					ARRAY_SIZE(tegra_dapm_widgets));

		/* Set up tegra specific audio path audio_map */
		snd_soc_dapm_add_routes(codec, audio_map,
					ARRAY_SIZE(audio_map));

		err = tegra_controls_init(codec);
		if (err < 0) {
			pr_err("Failed in controls init \n");
			return err;
		}

		audio_data->codec = codec;
		audio_data->init_done = 1;
	}

	return err;
}

static struct snd_soc_dai_link tegra_soc_dai[] = {
	{
		.name = "WM8903",
		.stream_name = "WM8903 HiFi",
		.cpu_dai = &tegra_i2s_dai[0],
		.codec_dai = &wm8903_dai,
		.init = tegra_codec_init,
		.ops = &tegra_hifi_ops,
	},
	{
		.name = "Tegra-generic",
		.stream_name = "Tegra Generic Voice",
		.cpu_dai = &tegra_i2s_dai[1],
		.codec_dai = &tegra_generic_codec_dai[0],
		.init = tegra_codec_init,
		.ops = &tegra_voice_ops,
	},
	{
		.name = "Tegra-spdif",
		.stream_name = "Tegra Spdif",
		.cpu_dai = &tegra_spdif_dai,
		.codec_dai = &tegra_generic_codec_dai[1],
		.init = tegra_codec_init,
		.ops = &tegra_spdif_ops,
	},
};

static struct tegra_audio_data audio_data = {
	.init_done = 0,
	.play_device = TEGRA_AUDIO_DEVICE_NONE,
	.capture_device = TEGRA_AUDIO_DEVICE_NONE,
	.is_call_mode = false,
	.codec_con = TEGRA_AUDIO_OFF,
};

static struct snd_soc_card tegra_snd_soc = {
	.name = "tegra",
	.platform = &tegra_soc_platform,
	.dai_link = tegra_soc_dai,
	.num_links = ARRAY_SIZE(tegra_soc_dai),
	.suspend_pre = tegra_soc_suspend_pre,
	.suspend_post = tegra_soc_suspend_post,
	.resume_pre = tegra_soc_resume_pre,
	.resume_post = tegra_soc_resume_post,
};

static struct snd_soc_device tegra_snd_devdata = {
	.card = &tegra_snd_soc,
	.codec_dev = &soc_codec_dev_wm8903,
	.codec_data = &audio_data,
};

static int __init tegra_init(void)
{
	int ret = 0;

	tegra_snd_device = platform_device_alloc("soc-audio", -1);
	if (!tegra_snd_device) {
		pr_err("failed to allocate soc-audio \n");
		return -ENOMEM;
	}

	platform_set_drvdata(tegra_snd_device, &tegra_snd_devdata);
	tegra_snd_devdata.dev = &tegra_snd_device->dev;

	ret = platform_device_add(tegra_snd_device);
	if (ret) {
		pr_err("audio device could not be added \n");
		goto fail;
	}

	return 0;

fail:
	if (tegra_snd_device) {
		platform_device_put(tegra_snd_device);
		tegra_snd_device = 0;
	}

	return ret;
}

static void __exit tegra_exit(void)
{
	tegra_jack_exit();
	platform_device_unregister(tegra_snd_device);
}

module_init(tegra_init);
module_exit(tegra_exit);

/* Module information */
MODULE_DESCRIPTION("Tegra ALSA SoC");
MODULE_LICENSE("GPL");
