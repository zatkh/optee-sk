ifeq ($(CFG_WITH_USER_TA),y)
srcs-$(CFG_CACHE_API) += svc_cache.c
endif
srcs-y += init.c
srcs-y += cache.c
