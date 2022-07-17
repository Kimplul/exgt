HTML_LOCAL != echo src/html/*.c
SOURCES += $(HTML_LOCAL)

include src/html/pages/source.mk
