MK_OUT_IMGS_PATH:=out
LIBCSITEECA_OBJ := $(MK_OUT_IMGS_PATH)/pangu_tee/csi_arch/wsl.o \
				   $(MK_OUT_IMGS_PATH)/tee/tee_ca/csi_tee_aes.o \
				   $(MK_OUT_IMGS_PATH)/tee/tee_ca/csi_tee_base64.o \
				   $(MK_OUT_IMGS_PATH)/tee/tee_ca/csi_tee_cid.o \
				   $(MK_OUT_IMGS_PATH)/tee/tee_ca/csi_tee_common.o \
				   $(MK_OUT_IMGS_PATH)/tee/tee_ca/csi_tee_dev.o \
				   $(MK_OUT_IMGS_PATH)/tee/tee_ca/csi_tee_hmac.o \
				   $(MK_OUT_IMGS_PATH)/tee/tee_ca/csi_tee_lpm.o \
				   $(MK_OUT_IMGS_PATH)/tee/tee_ca/csi_tee_manifest_info.o \
				   $(MK_OUT_IMGS_PATH)/tee/tee_ca/csi_tee_random.o \
				   $(MK_OUT_IMGS_PATH)/tee/tee_ca/csi_tee_rsa.o \
				   $(MK_OUT_IMGS_PATH)/tee/tee_ca/csi_tee_sha.o \
				   $(MK_OUT_IMGS_PATH)/tee/tee_ca/csi_tee_sysfreq.o \
				   $(MK_OUT_IMGS_PATH)/tee/tee_ca/tee_client_api.o \
				   $(MK_OUT_IMGS_PATH)/tee/tee_ca/csi_tee_xor.o

$(LIBCSITEECA): $(LIBCSITEECA_OBJ)
	@echo Generate tee.elf
	@mv yoc.elf tee.elf
	@csky-abiv2-elf-objdump -D tee.elf > tee.asm
	@echo AR $@
	@ar rcs $@ $^
