ifeq ($(CONFIG_ARM_STREAMLINE_DEBUGGER),y)

ifeq ($(CONFIG_ANDROID_PARANOID_NETWORK),y)
    # Uncomment the following line to enable kernel stack unwinding within gator, or update gator_backtrace.c
    EXTRA_CFLAGS +=	-DGATOR_KERNEL_STACK_UNWINDING
endif

obj-m := gator.o

gator-y :=	gator_main.o \
		gator_events_irq.o \
		gator_events_sched.o \
		gator_events_net.o \
		gator_events_block.o \
		gator_events_meminfo.o \
		gator_events_perf_pmu.o

gator-y +=	gator_events_mmaped.o

#ifeq ($(CONFIG_GATOR_MALI_SUPPORT),y)
#
#    ifeq ($(CONFIG_GATOR_MALI_400),y)
#        gator-y +=	gator_events_mali_400.o
#        EXTRA_CFLAGS +=	-DMALI_SUPPORT=0x0B07
#    else
#        gator-y +=	gator_events_mali_t6xx.o \
#		            gator_events_mali_t6xx_hw.o
#        include $(M)/mali_t6xx.mk
#        EXTRA_CFLAGS +=	-DMALI_SUPPORT=0x0056
#    endif
#
#    gator-y +=	gator_events_mali_common.o
#
#endif

ifeq ($(CONFIG_GATOR_DRIVER_TEST),y)
    EXTRA_CFLAGS +=	-DGATOR_TEST=1
else
    EXTRA_CFLAGS +=	-DGATOR_TEST=0
endif

gator-$(CONFIG_ARM) +=	gator_events_armv6.o \
			gator_events_armv7.o \
			gator_events_l2c-310.o \
			gator_events_scorpion.o

$(obj)/gator_main.o: gator_events.h

clean-files := gator_events.h

chk_events.h = :
quiet_chk_events.h = echo '  CHK     $@'
silent_chk_events.h = :
gator_events.h: FORCE
	@$($(quiet)chk_events.h)
	$(CONFIG_SHELL) $(obj)/gator_events.sh $@ $(obj)

endif
