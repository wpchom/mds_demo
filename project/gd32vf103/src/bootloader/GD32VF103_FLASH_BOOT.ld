/******************************************************************************
 * @file     startup_memory.ld
 * @brief    Linker Script for GD32VF103 based device
 ******************************************************************************/

MEMORY
{
    FLASH  (rx)     : ORIGIN = 0x08000000, LENGTH = 128K
    RAM    (rwx)    : ORIGIN = 0x00000000, LENGTH = 32K
}

__STACK_SIZE = 0x800;
__STACK_LOCAL = ORIGIN(RAM) + LENGTH(RAM) - __STACK_SIZE;
__HEAP_SIZE = __StackLimit - __HeapBase;

/* Linker script to place sections and symbol values. Should be used together
 * with other linker script that defines memory regions FLASH and RAM.
 * It references following symbols, which must be defined in code:
 *   Reset_Handler : Entry of reset handler
 */
ENTRY(Reset_Handler);

SECTIONS
{
    .vectors : {
        KEEP ( *(.vectors .vectors.*) )
    } > FLASH

    .text : {
        . = ALIGN(4);
        __text_start = .;

        *(.text .text.*)

        KEEP ( *(SORT_BY_NAME(.init) SORT_BY_NAME(.init.*)) )
        KEEP ( *(.fini .fini.*) )

        /* preinit data */
        PROVIDE_HIDDEN ( __preinit_array_start = . );
        KEEP ( *(.preinit_array) )
        PROVIDE_HIDDEN ( __preinit_array_end = . );

        /* init data */
        PROVIDE_HIDDEN ( __init_array_start = . );
        KEEP ( *(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)) )
        KEEP ( *(.init_array .ctors) )
        PROVIDE_HIDDEN ( __init_array_end = . );

        /* finit data */
        PROVIDE_HIDDEN ( __fini_array_start = . );
        KEEP ( *(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*)) )
        KEEP ( *(.fini_array .dtors) )
        PROVIDE_HIDDEN ( __fini_array_end = . );

        . = ALIGN(4);
        __text_end = .;
    } > FLASH
    PROVIDE ( __etext = __text_end );
    PROVIDE ( _etext = __text_end );
    PROVIDE ( etxet = __text_end );

    .rodata : {
        . = ALIGN(4);

        *(.rdata)
        *(.rodata .rodata.*)
        *(.srodata .srodata.*)
        *(.logstr .logstr.*)

        . = ALIGN(4);
    } > FLASH

    .ARM.extab : {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > FLASH

    .ARM.exidx : {
        PROVIDE ( __exidx_start = . );
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        PROVIDE ( __exidx_end = . );
    } > FLASH

    .table.copy : {
        . = ALIGN(4);
        PROVIDE_HIDDEN ( __copy_table_start = . );
        PROVIDE_HIDDEN ( __copy_table_start__ = . );

        LONG (LOADADDR(.data))
        LONG (__data_start)
        LONG ((__data_end - __data_start) / 4)

        PROVIDE_HIDDEN ( __copy_table_end = . );
        PROVIDE_HIDDEN ( __copy_table_end__ = . );
    } > FLASH

    .table.zero : {
        . = ALIGN(4);
        PROVIDE_HIDDEN ( __zero_table_start = . );
        PROVIDE_HIDDEN ( __zero_table_start__ = . );

        LONG (__bss_start)
        LONG ((__bss_end - __bss_start) / 4)

        PROVIDE_HIDDEN ( __zero_table_end = . );
        PROVIDE_HIDDEN ( __zero_table_end__ = . );
    } > FLASH

    .data : {
        . = ALIGN(4);
        PROVIDE ( __data_start = . );

        *(.data .data.*)
        *(.sdata .sdata.*)
        *(.ramfunc .ramfunc.*)

        . = ALIGN(4);
        PROVIDE ( __data_end = . );
    } > RAM AT > FLASH
    PROVIDE ( __edata = __data_end );
    PROVIDE ( _edata = __data_end );
    PROVIDE ( edata = __data_end );

    .bss (NOLOAD) : {
        . = ALIGN(4);
        PROVIDE ( __bss_start = . );
        PROVIDE ( __bss_start__ = . );

        *(.bss .bss.*)
        *(.sbss .sbss.*)
        *(COMMON)

        . = ALIGN(4);
        PROVIDE ( __bss_end = . );
        PROVIDE ( __bss_end__ = . );
    } > RAM
    PROVIDE ( __end = __bss_end );
    PROVIDE ( _end = __bss_end );
    PROVIDE ( end = __bss_end );

    .heap (NOLOAD) : {
        . = ALIGN(4);
        PROVIDE ( __HeapBase = . );

        . += __HEAP_SIZE;

        . = ALIGN(4);
        PROVIDE ( __HeapLimit = . );
    } > RAM

    .stack (__STACK_LOCAL) (NOLOAD) : {
        . = ALIGN(4);
        PROVIDE ( __StackLimit = . );

        . += __STACK_SIZE;

        . = ALIGN(4);
        PROVIDE ( __StackTop = . );
        PROVIDE ( __stack = . );
    } > RAM
}
