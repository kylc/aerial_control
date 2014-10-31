POSSIBLE_BOARD_NAME = $(addsuffix .mk, $(addprefix boards/, $(MAKECMDGOALS)))
ifneq (,$(wildcard $(POSSIBLE_BOARD_NAME)))
include $(POSSIBLE_BOARD_NAME)
else
all:
	@echo "Specify a board"
endif

upload:
	st-flash write build/osuar_control.bin 0x08000000

