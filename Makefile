BUILD_DIR := build

.PHONY: clean all

all: clean
	@echo "Building project..."
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake ..
	cd $(BUILD_DIR) && make

clean:
	@echo "Cleaning up building artefacts..."
	rm -rf $(BUILD_DIR)
