##
 # Copyright (C) 2017-2024 Alibaba Group Holding Limited
 #
 # SPDX-License-Identifier: Apache-2.0
 #
 # Licensed under the Apache License, Version 2.0 (the "License");
 # you may not use this file except in compliance with the License.
 # You may obtain a copy of the License at
 #
 #     http://www.apache.org/licenses/LICENSE-2.0
 #
 # Unless required by applicable law or agreed to in writing, software
 # distributed under the License is distributed on an "AS IS" BASIS,
 # WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 # See the License for the specific language governing permissions and
 # limitations under the License.
##

get_build_path_func = $(BUILD_DIR)/$(patsubst $(TOP_DIR)/%,%,$(1))

define remove_duplicate_defines_func
$(shell echo '$(1)' | tr ' ' '\n' | awk -F= ' \
BEGIN { \
    delete macros; \
    delete output; \
    count = 0; \
} \
/^-D/ { \
    key = substr($$0,3); \
    sub(/=.*/, "", key); \
    macros[key] = count; \
} \
{ \
    output[count++] = $$0; \
} \
END { \
    for (i = 0; i < count; i++) { \
        line = output[i]; \
        if (match(line, /^-D/)) { \
            key = substr(line,3); \
            sub(/=.*/, "", key); \
            if (macros[key] == i) printf "%s ", line; \
        } else { \
            printf "%s ", line; \
        } \
    } \
}' | sed 's/ $$//')
endef

define module_inlcude_template_func
COMP_PATH := $$(TOP_DIR)/components/$(1)
# $$(info Loading module: $$(COMP_PATH)/sub.mk)
include $$(COMP_PATH)/sub.mk
endef

define lib_rule_template_func
$(1)_OBJS := $$(foreach src, $$($(1)_c_srcs), $$(call get_build_path_func, $$(src:.c=.o))) \
             $$(foreach src, $$($(1)_cpp_srcs), $$(call get_build_path_func, $$(src:.cpp=.o))) \
             $$(foreach src, $$($(1)_cxx_srcs), $$(call get_build_path_func, $$(src:.cxx=.o))) \
             $$(foreach src, $$($(1)_cc_srcs), $$(call get_build_path_func, $$(src:.cc=.o))) \
             $$(foreach src, $$($(1)_asm_srcs), $$(call get_build_path_func, $$(src:.S=.o)))

$$(BUILD_DIR)/libs/lib$(1).a: $$($(1)_OBJS)
	@echo "[AR] $$@"
	@mkdir -p $$(dir $$@)
	$$(CPRE)$$(AR) rcs $$@ $$^
endef

define check_cflag_exact_func
$(strip \
    $(foreach opt,$(subst $(space),$(comma),$(CFLAGS)), \
        $(if $(filter $(subst $(comma),$(space),$(opt)),$1),1) \
    ) \
)
endef
space := $(subst ,, )
comma := ,