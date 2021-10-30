.PHONY: clean install build flash

# NOTE: This serial port may be different on macOS. If errors occur, plug in device
# and look for the new serial device that appears under /dev.
SERIAL_PORT=/dev/ttyACM0
SKETCH_FILE=candy-chute-client.ino
OUT_DIR=target
SRC_FILES=

$(OUT_DIR)/$(SKETCH_FILE).hex: $(SKETCH_FILE) $(SRC_FILES)
	arduino-cli compile -b adafruit:nrf52:feather52840 --output-dir $(OUT_DIR) ./$(SKETCH_FILE)

install:
	./install

build: $(OUT_DIR)/$(SKETCH_FILE).hex

flash: $(OUT_DIR)/$(SKETCH_FILE).hex
	arduino-cli upload -p $(SERIAL_PORT) --input-dir $(OUT_DIR) -b adafruit:nrf52:feather52840 ./$(SKETCH_FILE)

clean:
	rm target/*
