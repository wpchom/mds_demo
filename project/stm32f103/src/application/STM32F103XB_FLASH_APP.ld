/******************************************************************************
 * @file     startup_memory.ld
 * @brief    Linker Script for Cortex-M based device
 ******************************************************************************/

MEMORY
{
    LOGSTR  (r)     : ORIGIN = 0xFF000000, LENGTH = 16M

    BOOT    (rx)    : ORIGIN = 0x08000000, LENGTH = 10K
    FLASH   (rx)    : ORIGIN = 0x08002800, LENGTH = 32K
    DFT     (rx)    : ORIGIN = 0x0800A800, LENGTH = 22K

    RAM     (rwx)   : ORIGIN = 0x20000000, LENGTH = 20K
}

__STACK_SIZE = 0x400;
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
    .vectors ORIGIN(FLASH) : {
        KEEP ( *(.vectors .vectors.*) )
    } > FLASH

    .text : {
        . = ALIGN(4);
        __text_start = .;

        *(.text .text.*)

        KEEP ( *(SORT_BY_INIT_PRIORITY(.mds.init.*)) )

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

    .boot ORIGIN(RAM) (NOLOAD) : {
        . = ALIGN(4);

        KEEP(*(.mds.boot .mds.boot.*))

        . = ALIGN(4);
    } > RAM

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

    .bss : {
        . = ALIGN(4);
        PROVIDE ( __bss_start = . );
        PROVIDE ( __bss_start__ = . );

        *(.bss .bss.*)
        *(.sbss .sbss.*)
        *(COMMON)

        . = ALIGN(4);
        PROVIDE ( __bss_end = . );
        PROVIDE ( __bss_end__ = . );
    } > RAM AT > RAM
    PROVIDE ( __end = __bss_end );
    PROVIDE ( _end = __bss_end );
    PROVIDE ( end = __bss_end );

    .heap (NOLOAD) : {
        . = ALIGN(8);
        PROVIDE ( __HeapBase = . );

        . += __HEAP_SIZE;

        . = ALIGN(8);
        PROVIDE ( __HeapLimit = . );
    } > RAM

    .stack (__STACK_LOCAL) (NOLOAD) : {
        . = ALIGN(8);
        PROVIDE ( __StackLimit = . );

        . += __STACK_SIZE;

        . = ALIGN(8);
        PROVIDE ( __StackTop = . );
        PROVIDE ( __stack = . );
    } > RAM
}

/* Alias */
memset = MDS_MemBuffSet;
memcpy = MDS_MemBuffCpy;
memccpy = MDS_MemBuffCcpy;
memcmp = MDS_MemBuffCmp;

strtoull = MDS_Strtoull;
strtoll = MDS_Strtoll;
strtoul = MDS_Strtoul;
strtol = MDS_Strtol;

vsnprintf = MDS_VaStringNPrintf;
snprintf = MDS_StringNPrintf;
vsprintf = MDS_VaStringPrintf;
sprintf = MDS_StringPrintf;
vsscanf = MDS_VaStringScanf;
sscanf = MDS_StringScanf;

vsprintf_s = MDS_VaStringNPrintf;
sprintf_s = MDS_StringNPrintf;
vsscanf_s = MDS_VaStringScanfS;
sscanf_s = MDS_StringScanfS;
