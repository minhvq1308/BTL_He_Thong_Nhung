/* Linker script STM32F103C8T6: FLASH 64K, RAM 20K */
ENTRY(Reset_Handler)

_estack = 0x20005000;          /* dinh RAM (0x20000000 + 20K) */
_Min_Stack_Size = 0x400;       /* 1 KB cho MSP (ISR + main truoc khi scheduler chay) */

MEMORY
{
  FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 64K
  RAM   (xrw) : ORIGIN = 0x20000000, LENGTH = 20K
}

SECTIONS
{
  .isr_vector : { . = ALIGN(4); KEEP(*(.isr_vector)) . = ALIGN(4); } >FLASH

  .text : {
    . = ALIGN(4);
    *(.text) *(.text*) *(.rodata) *(.rodata*)
    *(.glue_7) *(.glue_7t)
    . = ALIGN(4);
    _etext = .;
  } >FLASH

  _sidata = LOADADDR(.data);

  .data : {
    . = ALIGN(4);
    _sdata = .;
    *(.data) *(.data*)
    . = ALIGN(4);
    _edata = .;
  } >RAM AT> FLASH

  .bss : {
    . = ALIGN(4);
    _sbss = .;
    *(.bss) *(.bss*) *(COMMON)
    . = ALIGN(4);
    _ebss = .;
  } >RAM

  PROVIDE(end = .);
  PROVIDE(_end = .);

  /* Chua cho cho stack chinh (MSP) */
  ._user_heap_stack : {
    . = ALIGN(4);
    . = . + _Min_Stack_Size;
    . = ALIGN(4);
  } >RAM

  .ARM.attributes 0 : { *(.ARM.attributes) }
}
