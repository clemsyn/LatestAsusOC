voodoo-sound-objs := tegra_soc_voodoo_sound.o

ccflags-y                       += -DNV_DEBUG=0
obj-$(CONFIG_TEGRA_PCM)         += tegra_pcm.o
obj-$(CONFIG_TEGRA_I2S)         += tegra_i2s.o
obj-$(CONFIG_TEGRA_IEC)         += tegra_spdif.o
obj-$(CONFIG_TEGRA_JACK)        += tegra_wired_jack.o
obj-$(CONFIG_TEGRA_ALSA)        += tegra_soc_controls.o
obj-$(CONFIG_TEGRA_VOODOO_SOUND)+= voodoo-sound.o
obj-$(CONFIG_TEGRA_GENERIC_CODEC)+= tegra_generic_codec.o
obj-${CONFIG_SND_SOC_WM8903}    += tegra_soc_wm8903.o
obj-${CONFIG_SND_SOC_WM8753}    += tegra_soc_wm8753.o
#obj-${CONFIG_SND_SOC_TLV320AIC326X}    += tegra_soc_aic326x.o
obj-$(CONFIG_HEADSET_FUNCTION)	+= headset.o
