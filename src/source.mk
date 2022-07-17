SRC_LOCAL != echo src/*.c
SOURCES += $(SRC_LOCAL)

include src/utils/source.mk
include src/html/source.mk
include src/css/source.mk
