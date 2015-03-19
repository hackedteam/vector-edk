#ifndef CPU_H
#define CPU_H

//#ifndef _WIN64 TODO: remove
#if 0
#define _eflags( eflags ) __asm \
          {                             \
            __asm push eax		\
            __asm pushfd                \
            __asm pop eax		\
            __asm mov eflags, eax	\
            __asm pop eax               \
          }
#endif

#if defined(_M_AMD64)
typedef UINT64 CPU_REG_TYPE;
#elif defined(_M_IX86)
typedef UINT32 CPU_REG_TYPE;
#else
#error "Architecture not supported"
#endif

/*
 * External Assembly Functions
 */
// -- Access to CPU MSRs
extern void _rdmsr( unsigned int msr_num, unsigned int* msr_lo, unsigned int* msr_hi );
extern void _wrmsr( unsigned int msr_num, unsigned int  msr_hi, unsigned int  msr_lo );

  // -- Access to PCI CFG space
extern void WritePCIByte          ( unsigned int pci_reg, unsigned short cfg_data_port, unsigned char  byte_value );
extern void WritePCIWord          ( unsigned int pci_reg, unsigned short cfg_data_port, unsigned short word_value );
extern void WritePCIDword         ( unsigned int pci_reg, unsigned short cfg_data_port, unsigned int   dword_value );
extern unsigned char  ReadPCIByte ( unsigned int pci_reg, unsigned short cfg_data_port );
extern unsigned short ReadPCIWord ( unsigned int pci_reg, unsigned short cfg_data_port );
extern unsigned int   ReadPCIDword( unsigned int pci_reg, unsigned short cfg_data_port );
// -- Access to Port I/O
extern unsigned int   ReadPortDword ( unsigned short port_num );
extern unsigned short ReadPortWord  ( unsigned short port_num );
extern unsigned char  ReadPortByte  ( unsigned short port_num );
extern void           WritePortDword( unsigned int   out_value, unsigned short port_num );
extern void           WritePortWord ( unsigned short out_value, unsigned short port_num );
extern void           WritePortByte ( unsigned char  out_value, unsigned short port_num );

  // -- Access to CPU Descriptor tables
extern void _store_idtr( void* desc_address );
extern void _load_idtr ( void* desc_address );
extern void _store_gdtr( void* desc_address );
extern void _store_ldtr( void* desc_address );


// general purprose
extern int usleep(int microseconds);

// -- Interrupts
//extern void __stdcall _swsmi( UINT32 smi_code_data, CPU_REG_TYPE rax_value, CPU_REG_TYPE rbx_value, CPU_REG_TYPE rcx_value, CPU_REG_TYPE rdx_value, CPU_REG_TYPE rsi_value, CPU_REG_TYPE rdi_value );

// --
// -- MSR definitions
// --
#define MSR_IA32_BIOS_UPDT_TRIG 0x79
#define MSR_IA32_BIOS_SIGN_ID   0x8b


// -- CPU Descriptor tables
typedef enum {
  CPU_DT_CODE_IDTR = 0x0,
  CPU_DT_CODE_GDTR = 0x1,
  CPU_DT_CODE_LDTR = 0x2,
} DTR_CODE;

#pragma pack(1)
typedef struct _DESCRIPTOR_TABLE_RECORD {
  UINT16    limit;
  //UINT32    base_hi;
  //UINT32    base_lo;
  UINT32 base;
} DESCRIPTOR_TABLE_RECORD, *PDESCRIPTOR_TABLE_RECORD;
#pragma pack()

#endif	// CPU_H
