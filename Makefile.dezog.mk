## DeZog build helper for z88dk C, assembly, or mixed projects.
##
## Set SOURCE, ZCC, ZCC_TARGET, ZCC_FLAGS, ZCC_LIBS and DEZOG_TARGET in the
## including Makefile. HEADER and ASM_INCLUDE_DEPS are optional prerequisites.
## CRT is optional: if unset, zcc selects its target's default startup; if set,
## the caller's startup is passed to the linker. C sources use SDCC for source
## mapping; assembly-only targets do not need a C runtime.
## Set DEZOG_DIR to choose where the binary, map, objects and listings go.
## For targets needing appmake, set DEZOG_APP_OUTPUT to appmake's generated
## filename; the helper copies it over the requested debug binary.

DEZOG_DIR ?= .tmp/dezog
DEZOG_OBJ_DIR = $(DEZOG_DIR)/obj
DEZOG_C_SOURCES = $(filter %.c,$(SOURCE))
DEZOG_ASM_SOURCES = $(filter %.asm,$(SOURCE))
DEZOG_C_OBJS = $(patsubst %.c,$(DEZOG_OBJ_DIR)/%.o,$(DEZOG_C_SOURCES))
DEZOG_ASM_OBJS = $(patsubst %.asm,$(DEZOG_OBJ_DIR)/%.o,$(DEZOG_ASM_SOURCES))
DEZOG_OBJS = $(DEZOG_C_OBJS) $(DEZOG_ASM_OBJS)
DEZOG_C_LISTINGS = $(patsubst %.c,%.c.lis,$(DEZOG_C_SOURCES))
DEZOG_ASM_LISTINGS = $(patsubst %.asm,%.asm.lis,$(DEZOG_ASM_SOURCES))
DEZOG_LISTINGS = $(DEZOG_C_LISTINGS) $(DEZOG_ASM_LISTINGS)
DEZOG_BASE = $(basename $(notdir $(DEZOG_TARGET)))
DEZOG_ZCC ?= $(ZCC)
DEZOG_MAP = $(DEZOG_DIR)/$(DEZOG_BASE).map
DEZOG_BIN = $(DEZOG_DIR)/$(notdir $(DEZOG_TARGET))
DEZOG_C_FLAGS = $(ZCC_TARGET) $(ZCC_FLAGS) -SO0 -c --list --c-code-in-asm -compiler=sdcc
DEZOG_ASM_FLAGS = $(ZCC_TARGET) $(ZCC_FLAGS) -c -debug --list
DEZOG_LINK_COMPILER = $(if $(DEZOG_C_SOURCES),-compiler=sdcc)
DEZOG_LINK_FLAGS = $(ZCC_TARGET) $(ZCC_FLAGS) -debug $(if $(CRT),-crt0=$(CRT)) $(DEZOG_LINK_COMPILER) $(if $(DEZOG_APP_OUTPUT),-create-app)
DEZOG_MAP_SED = 's@([[:alnum:]_./-]+\.c)(::[^: ]+)+:([0-9]+)@\1:\3@g'
DEZOG_LISTING_SED = \
	-e 's@^([[:alnum:]_./-]+\.c)::.*:@\1:@' \
	-e 's@("[[:alnum:]_./-]+\.c)::[^"]*"@\1"@g'

.PHONY: dezog clean-dezog
dezog: $(DEZOG_BIN) $(DEZOG_MAP) $(DEZOG_LISTINGS:%=$(DEZOG_DIR)/%)

clean: clean-dezog

clean-dezog:
	rm -rf $(DEZOG_DIR)

$(DEZOG_DIR):
	mkdir -p $@

$(DEZOG_OBJ_DIR)/%.o: %.c $(HEADER)
	mkdir -p $(dir $@)
	$(DEZOG_ZCC) $(DEZOG_C_FLAGS) $< -o $@

$(DEZOG_OBJ_DIR)/%.o: %.asm $(ASM_INCLUDE_DEPS)
	mkdir -p $(dir $@)
	$(DEZOG_ZCC) $(DEZOG_ASM_FLAGS) $< -o $@

$(DEZOG_BIN) $(DEZOG_MAP) &: $(DEZOG_OBJS) $(CRT) | $(DEZOG_DIR)
	$(DEZOG_ZCC) $(DEZOG_LINK_FLAGS) \
		$(DEZOG_OBJS) $(ZCC_LIBS) -o $(DEZOG_BIN) -m --list
	$(if $(DEZOG_APP_OUTPUT),cp $(DEZOG_APP_OUTPUT) $(DEZOG_BIN))
	mv $(DEZOG_MAP) $(DEZOG_MAP).raw
	sed -E $(DEZOG_MAP_SED) $(DEZOG_MAP).raw > $(DEZOG_MAP)
	rm -f $(DEZOG_MAP).raw

$(DEZOG_DIR)/%.c.lis: %.c.lis
	mkdir -p $(dir $@)
	sed -E $(DEZOG_LISTING_SED) $< > $@

$(DEZOG_DIR)/%.asm.lis: %.asm.lis
	mkdir -p $(dir $@)
	cp $< $@
