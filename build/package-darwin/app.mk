# $Id$
#
# Create an application directory for Darwin.

# Start from platform-independent bindist support.
include $(MAKE_PATH)/bindist.mk

APP_SUPPORT_DIR:=build/package-darwin
APP_DIR:=$(BINDIST_DIR)/openMSX.app
APP_EXE_DIR:=$(APP_DIR)/Contents/MacOS
APP_PLIST:=$(APP_DIR)/Contents/Info.plist
APP_RES:=$(APP_DIR)/Contents/Resources
APP_ICON:=$(APP_RES)/openmsx-logo.icns

# Override install locations.
INSTALL_BINARY_DIR:=$(APP_EXE_DIR)
INSTALL_SHARE_DIR:=$(APP_DIR)/share
INSTALL_DOC_DIR:=$(BINDIST_DIR)/Documentation

BINDIST_IMAGE:=$(BUILD_PATH)/$(PACKAGE_FULL)-mac-$(OPENMSX_TARGET_CPU)-bin.dmg
BINDIST_README:=$(BINDIST_DIR)/README.html

bindist: $(APP_PLIST) $(APP_ICON) $(BINDIST_README)
	@echo "Creating disk image:"
	@hdiutil create -srcfolder $(BINDIST_DIR) \
		-volname openMSX \
		-imagekey zlib-level=9 \
		-ov $(BINDIST_IMAGE)
	@hdiutil internet-enable -yes $(BINDIST_IMAGE)

$(APP_PLIST): $(APP_DIR)/Contents/%: $(APP_SUPPORT_DIR)/% bindistclean
	@echo "  Writing meta-info..."
	@mkdir -p $(@D)
	@sed -e 's/%ICON%/$(notdir $(APP_ICON))/' \
		-e 's/%VERSION%/$(PACKAGE_DETAILED_VERSION)/' < $< > $@
	@echo "APPLoMSX" > $(@D)/PkgInfo

$(APP_ICON): $(APP_RES)/%: $(APP_SUPPORT_DIR)/% bindistclean
	@echo "  Copying resources..."
	@mkdir -p $(@D)
	@cp $< $@

$(BINDIST_README): $(APP_SUPPORT_DIR)/README.html
	@echo "  Copying README..."
	@mkdir -p $(@D)
	@cp $< $@
# Remove form feeds from the GPL document, so Safari will treat it as text.
	@awk '!/\f/ ; /\f/ { print "" }' GPL > $(INSTALL_DOC_DIR)/GPL
