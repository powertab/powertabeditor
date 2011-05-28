# common build settings for the main application and unit test projects
# (settings for linking with the various static libraries that are built)

# NOTE - should define SOURCE_DIR=/relative/path/to/source/dir before including this file
LIBS += \
    -L$${SOURCE_DIR}/app/$${BUILDTYPE} -lapp \
    -L$${SOURCE_DIR}/widgets/$${BUILDTYPE} -lwidgets\
    -L$${SOURCE_DIR}/dialogs/$${BUILDTYPE} -ldialogs \
    -L$${SOURCE_DIR}/actions/$${BUILDTYPE} -lactions \
    -L$${SOURCE_DIR}/audio/$${BUILDTYPE} -lpteaudio \
    -L$${SOURCE_DIR}/audio/rtmidi/$${BUILDTYPE} -lrtmidi \
    -L$${SOURCE_DIR}/painters/$${BUILDTYPE} -lpainters \
    -L$${SOURCE_DIR}/powertabdocument/$${BUILDTYPE} -lpowertabdocument

LIB_EXT='a'

PRE_TARGETDEPS += \
    $${SOURCE_DIR}/app/$${BUILDTYPE}/libapp.$${LIB_EXT} \
    $${SOURCE_DIR}/widgets/$${BUILDTYPE}/libwidgets.$${LIB_EXT} \
    $${SOURCE_DIR}/dialogs/$${BUILDTYPE}/libdialogs.$${LIB_EXT} \
    $${SOURCE_DIR}/actions/$${BUILDTYPE}/libactions.$${LIB_EXT} \
    $${SOURCE_DIR}/audio/$${BUILDTYPE}/libpteaudio.$${LIB_EXT} \
    $${SOURCE_DIR}/audio/rtmidi/$${BUILDTYPE}/librtmidi.$${LIB_EXT} \
    $${SOURCE_DIR}/painters/$${BUILDTYPE}/libpainters.$${LIB_EXT} \
    $${SOURCE_DIR}/powertabdocument/$${BUILDTYPE}/libpowertabdocument.$${LIB_EXT}
