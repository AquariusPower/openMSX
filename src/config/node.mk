# $Id$

include build/node-start.mk

SRC_HDR:= \
	XMLLoader XMLElement \
	HardwareConfig MachineConfig ExtensionConfig \
	SettingsConfig

HDR_ONLY:= \
	ConfigException \
	XMLException

include build/node-end.mk

