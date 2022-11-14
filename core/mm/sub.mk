srcs-y += fobj.c
srcs-y += file.c
srcs-y += tee_mm.c
srcs-y += mobj.c
srcs-$(CFG_CORE_DYN_SHM) += mobj_dyn_shm.c
srcs-y += tee_mmu.c
srcs-y += pgt_cache.c
