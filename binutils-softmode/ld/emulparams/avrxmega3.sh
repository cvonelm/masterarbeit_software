ARCH=avr:103
MACHINE=
SCRIPT_NAME=avr
OUTPUT_FORMAT="elf32-avr"
MAXPAGESIZE=1
EMBEDDED=yes
TEMPLATE_NAME=elf

TEXT_LENGTH=1024K
DATA_ORIGIN=0x802000
DATA_LENGTH=0xffa0
RODATA_PM_OFFSET=0x8000
EXTRA_EM_FILE=avrelf

FUSE_NAME=fuse

EEPROM_LENGTH=64K
FUSE_LENGTH=1K
LOCK_LENGTH=1K
SIGNATURE_LENGTH=1K
USER_SIGNATURE_LENGTH=1K