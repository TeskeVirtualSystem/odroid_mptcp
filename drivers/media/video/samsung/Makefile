obj-$(CONFIG_VIDEO_FIMC)	+= fimc/
obj-$(CONFIG_VIDEO_JPEG)	+= jpeg/
obj-$(CONFIG_VIDEO_JPEG_V2X)	+= jpeg_v2x/
obj-$(CONFIG_VIDEO_TVOUT)	+= tvout/
obj-$(CONFIG_VIDEO_MFC5X)	+= mfc5x/
obj-$(CONFIG_VIDEO_FIMG2D3X)	+= fimg2d3x/
obj-$(CONFIG_VIDEO_FIMG2D4X)	+= fimg2d4x/

ifeq ($(CONFIG_ANDROID_412),y)
obj-$(CONFIG_VIDEO_UMP)		+= ump/
obj-$(CONFIG_VIDEO_MALI400MP)	+= mali/
endif

ifeq ($(CONFIG_ANDROID_422),y)
obj-$(CONFIG_VIDEO_MALI400MP_UMP)	+= ump_r3p1/
obj-$(CONFIG_MALI_400MP_UMP)		+= mali_r3p1/
endif

EXTRA_CFLAGS += -Idrivers/media/video
