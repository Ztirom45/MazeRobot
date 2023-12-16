all:
	pio run -t upload
	pio device monitor

build:
	pio run
monitor:
	pio device monitor
