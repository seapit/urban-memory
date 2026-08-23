# Define build directory
BUILD_DIR := build
BLD := $(BUILD_DIR)

# Extract arguments from the command line
ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))

# File to remember last preset
PRESET_FILE := .last_preset

# Default preset if none remembered yet
DEFAULT_PRESET := x86-gcc

# Load last preset if file exists
ifeq ($(wildcard $(PRESET_FILE)),)
  PRESET ?= $(DEFAULT_PRESET)
else
  PRESET ?= $(shell cat $(PRESET_FILE))
endif

# Save preset whenever explicitly set
ifeq ($(origin PRESET), command line)
  $(shell echo $(PRESET) > $(PRESET_FILE))
endif

# File to remember last chip - orthogonal to PRESET (that's the
# toolchain). Only matters for a chip-aware module (CHIPS populated, see
# that module's README): each chip is always its own target
# (<Module>-<chip>, build via `make tgt <Module>-<chip>` or
# `make tgt <Module>-all`) regardless of CHIP. CHIP just picks what the
# plain <Module> name resolves to. Plain modules ignore it.
CHIP_FILE := .last_chip

# Default chip if none remembered yet.
DEFAULT_CHIP := none

# Load last chip if file exists
ifeq ($(wildcard $(CHIP_FILE)),)
  CHIP ?= $(DEFAULT_CHIP)
else
  CHIP ?= $(shell cat $(CHIP_FILE))
endif

# Save chip whenever explicitly set
ifeq ($(origin CHIP), command line)
  $(shell echo $(CHIP) > $(CHIP_FILE))
endif


# Default target to prevent errors if no target is specified
%:
	@true

.PHONY: help
help:
	@echo "Usage:"
	@echo "  make config [PRESET=x86-gcc] [CHIP=<chip>]          - Configure project (Release)"
	@echo "  make config-testdeb [PRESET=x86-gcc] [CHIP=<chip>]  - Configure project (Debug + Tests)"
	@echo "  make config-testrel [PRESET=x86-gcc] [CHIP=<chip>]  - Configure project (Release + Tests)"
	@echo "  make tgt [Target] PRESET=x86-clang                  - Build specific target"
	@echo "  make test [Lib] PRESET=armv7a-gcc                   - Build and run tests"
	@echo "  make purge                                          - Purge build dir"
	@echo "  make clean                                          - Clean build dir"
	@echo ""
	@echo "PRESET and CHIP are remembered between runs (.last_preset/.last_chip)."
	@echo "A module can be plain (just 'make tgt <ModuleName>') or chip-aware"
	@echo "(CHIPS populated - see that module's README.md):"
	# @echo "  make tgt <Module>-<chip> PRESET=x86-clang           - build just one chip's variant"
	# @echo "  make tgt <Module>-all PRESET=x86-clang              - build every chip variant"
	# @echo "  make test <Module>-<chip> PRESET=x86-clang          - build+run just one chip's test"
	@echo "CHIP=<chip> at config time only picks what the plain module name resolves"
	@echo "to, for consumers (apps) that don't want to hardcode a chip."

.PHONY: config config-testdeb config-testrel
config:
	@echo "Configuring with preset $(PRESET), chip $(CHIP) (Release)..."
	@cmake --preset=$(PRESET) -DCMAKE_BUILD_TYPE=Release -DCHIP=$(CHIP) --log-level=WARNING

config-testdeb:
	@echo "Configuring with preset $(PRESET), chip $(CHIP) (Debug+Tests)..."
	@cmake --preset=$(PRESET) -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON -DCHIP=$(CHIP) --log-level=WARNING

config-testrel:
	@echo "Configuring with preset $(PRESET), chip $(CHIP) (Release+Tests)..."
	@cmake --preset=$(PRESET) -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON -DCHIP=$(CHIP) --log-level=WARNING

# Build specified target
.PHONY: tgt
tgt:
	@if [ -z "$(ARGS)" ]; then \
			echo "Please specify an app/lib using 'make tgt <App> <Lib>' (or 'make tgt <Module>-<chip>' for a chip-aware module)"; \
			exit 1; \
	fi
	@echo "Building target: $(ARGS) with preset $(PRESET)"
	cmake --build build/$(PRESET)/output --target $(ARGS)

# Run tests
.PHONY: test
test:
	@if [ -z "$(ARGS)" ]; then \
		echo "Please specify a unit test using 'make test <Lib> PRESET=x86-clang' (or 'make test <Module>-<chip>' for a chip-aware module)"; \
		exit 1; \
	fi
	@echo "Building and running tests for target: $(ARGS)_Test with preset $(PRESET)"
	cmake --build build/$(PRESET)/output --target $(ARGS)_Test
	ctest --test-dir build/$(PRESET)/output -R "^$(ARGS)" --output-on-failure -V

# Clean/purge
.PHONY: purge clean
purge:
	rm -rf $(BUILD_DIR)

clean:
	cmake --build $(BLD)/$(PRESET)/output --target clean
