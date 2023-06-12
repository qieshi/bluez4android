LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := hcitool
LOCAL_SRC_FILES := hcitool.c
LOCAL_SRC_FILES += lib/bluetooth.c lib/hci.c lib/sdp.c lib/oui.c
LOCAL_CFLAGS += -DVERSION=\"5.10\" -Wno-unused-parameter -Wno-pointer-arith -Wno-unused-variable -Wno-missing-field-initializers  -Wno-unused-value -Wno-unused-function
LOCAL_C_INCLUDES := $(KERNEL_HEADERS) $(LOCAL_PATH)/lib
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := hciconfig
LOCAL_SRC_FILES := hciconfig.c parser/csr.c
LOCAL_SRC_FILES += lib/bluetooth.c lib/hci.c lib/sdp.c
LOCAL_CFLAGS += -Wno-unused-parameter -Wno-pointer-arith -Wno-unused-variable -Wno-missing-field-initializers -Wno-unused-value -Wno-unused-function
LOCAL_C_INCLUDES := $(KERNEL_HEADERS) $(LOCAL_PATH)/lib $(LOCAL_PATH)/parser
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := hcidump
LOCAL_SRC_FILES := hcidump.c lib/hci.c lib/bluetooth.c
LOCAL_SRC_FILES += parser/parser.c parser/lmp.c parser/hci.c parser/l2cap.c parser/amp.c parser/smp.c parser/att.c parser/sdp.c parser/rfcomm.c parser/bnep.c parser/cmtp.c parser/hidp.c parser/hcrp.c parser/avdtp.c parser/avctp.c parser/avrcp.c parser/sap.c parser/obex.c parser/capi.c parser/ppp.c parser/tcpip.c parser/ericsson.c parser/csr.c parser/bpa.c
LOCAL_CFLAGS += -DVERSION=\"5.10\" -Wno-unused-parameter -Wno-pointer-arith -Wno-unused-variable -Wno-missing-field-initializers -Wno-unused-value -Wno-unused-function
LOCAL_C_INCLUDES := $(KERNEL_HEADERS) $(LOCAL_PATH)/lib $(LOCAL_PATH)/parser
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_MODULE := hciattach
LOCAL_SRC_FILES := hciattach_st.c hciattach_ti.c \
		hciattach_tialt.c hciattach_ath3k.c \
		hciattach_qualcomm.c hciattach_intel.c \
		hciattach_bcm43xx.c hciattach.c
LOCAL_SRC_FILES += lib/bluetooth.c lib/hci.c lib/sdp.c
LOCAL_CFLAGS += -DFIRMWARE_DIR=\"/etc/firmware\" -Wno-unused-parameter -Wno-pointer-arith -Wno-unused-variable -Wno-missing-field-initializers -Wno-unused-value -Wno-unused-function
LOCAL_C_INCLUDES := $(KERNEL_HEADERS) $(LOCAL_PATH)/lib
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_MODULE := btmon
LOCAL_SRC_FILES := monitor/main.c monitor/display.c monitor/hcidump.c monitor/ellisys.c monitor/control.c monitor/packet.c monitor/vendor.c monitor/lmp.c monitor/crc.c monitor/ll.c monitor/l2cap.c monitor/sdp.c monitor/avctp.c monitor/avdtp.c monitor/a2dp.c monitor/rfcomm.c monitor/bnep.c monitor/hwdb.c monitor/keys.c monitor/analyze.c monitor/intel.c monitor/broadcom.c monitor/msft.c monitor/jlink.c monitor/att.c
LOCAL_SRC_FILES += lib/bluetooth.c lib/hci.c lib/sdp.c src/shared/mainloop.c src/shared/mainloop-notify.c src/shared/btsnoop.c src/shared/util.c src/shared/queue.c src/shared/crypto.c src/shared/gatt-db.c lib/uuid.c src/shared/timeout-mainloop.c src/shared/io-mainloop.c
LOCAL_CFLAGS += -DVERSION=\"5.10\" -Wno-unused-parameter -Wno-pointer-arith -Wno-unused-variable -Wno-missing-field-initializers -Wno-unused-value -Wno-unused-function -Wno-char-subscripts
LOCAL_C_INCLUDES := $(KERNEL_HEADERS) $(LOCAL_PATH)/lib $(LOCAL_PATH)/monitor $(LOCAL_PATH)/lib/glib
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)
