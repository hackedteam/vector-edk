TITLE   cpu.asm: Assembly code for the i386 resources

; include callconv.inc

PUBLIC store_gdtr

PUBLIC WritePCIByte
PUBLIC WritePCIWord
PUBLIC WritePCIDword
PUBLIC ReadPCIByte
PUBLIC ReadPCIWord
PUBLIC ReadPCIDword
PUBLIC ReadPortByte
PUBLIC ReadPortWord
PUBLIC ReadPortDword
PUBLIC WritePortByte
PUBLIC WritePortWord
PUBLIC WritePortDword

    .586p
    .model  flat,C
    .code
    
;------------------------------------------------------------------------------
; void _store_idtr(
;   unsigned char *address // rcx
;   )
;------------------------------------------------------------------------------
store_idtr PROC
    mov ecx, dword ptr [esp+4]
    sidt dword ptr [ecx]
    ret
store_idtr ENDP

;------------------------------------------------------------------------------
; void _load_idtr(
;   unsigned char *address // rcx
;   )
;------------------------------------------------------------------------------
load_idtr PROC
    mov ecx, dword ptr [esp+4]
    lidt fword ptr [ecx]
    ret
load_idtr ENDP

;------------------------------------------------------------------------------
; void _store_gdtr(
;   unsigned char *address // rcx
;   )
;------------------------------------------------------------------------------
store_gdtr PROC
    mov ecx, dword ptr [esp+4]
    sgdt dword ptr [ecx]
    RET
store_gdtr ENDP

;------------------------------------------------------------------------------
; void _store_ldtr(
;   unsigned char *address // rcx
;   )
;------------------------------------------------------------------------------
store_ldtr PROC
    mov ecx, dword ptr [esp+4]
    sldt word ptr [ecx]
    RET
store_ldtr ENDP

;------------------------------------------------------------------------------
;  void __stdcall _rdmsr(
;    IN UINT32 msr,
;    OUT UINT32* msrlo,
;    OUT UINT32* msrhi 
;    )
;------------------------------------------------------------------------------
_rdmsr PROC
    mov ecx, dword ptr [esp + 4] ; msr

    rdmsr

    mov ecx, dword ptr [esp + 8] 
    mov dword ptr [ecx], eax ; msrlo
    mov ecx, dword ptr [esp + 12] 
    mov dword ptr [ecx], edx ; msrhi

    RET
    
_rdmsr ENDP

;------------------------------------------------------------------------------
;  VOID __stdcall _wrmsr(
;    IN UINT32 msr,
;    IN UINT32 msrlo,
;    IN UINT32 msrhi 
;    )
;------------------------------------------------------------------------------
_wrmsr PROC
    mov ecx, dword ptr [esp + 4]  ; msr
    mov eax, dword ptr [esp + 8] ; msrlo
    mov edx, dword ptr [esp + 12] ; msrhi

    wrmsr

    RET
_wrmsr ENDP


;------------------------------------------------------------------------------
;UINT32 _eflags()
;------------------------------------------------------------------------------
eflags PROC
    pushfd
    pop eax
    RET 
eflags ENDP

;------------------------------------------------------------------------------
;  VOID
;  WritePortDword (
;    IN   UINT32    out_value
;    IN   UINT16    port_num
;    )
;------------------------------------------------------------------------------
WritePortDword PROC
    mov eax, dword ptr [esp + 4]    ; out_value
    mov edx, dword ptr [esp + 8]   ; port_num
    out dx, eax

    RET
WritePortDword ENDP

;------------------------------------------------------------------------------
;  VOID
;  WritePortWord (
;    IN   UINT16    out_value
;    IN   UINT16    port_num
;    )
;------------------------------------------------------------------------------
WritePortWord PROC
    mov eax, dword ptr [esp + 4]   ; out_value
    mov edx, dword ptr [esp + 8]  ; port_num
    out dx, ax

    RET
WritePortWord ENDP

;------------------------------------------------------------------------------
;  VOID
;  WritePortByte (
;    IN   UINT8     out_value
;    IN   UINT16    port_num
;    )
;------------------------------------------------------------------------------
WritePortByte PROC
    mov eax, dword ptr [esp + 4]   ; out_value
    mov edx, dword ptr [esp + 8]  ; port_num
    out dx, al

    RET
WritePortByte ENDP

;------------------------------------------------------------------------------
;  UINT32
;  ReadPortDword (
;    IN   UINT16    port_num
;    )
;------------------------------------------------------------------------------
ReadPortDword PROC
    xor eax, eax
    mov edx, dword ptr [esp + 4] ; port_num
    in eax, dx

    RET
ReadPortDword ENDP


;------------------------------------------------------------------------------
;  UINT16
;  ReadPortWord (
;    IN   UINT16    port_num
;    )
;------------------------------------------------------------------------------
ReadPortWord PROC
    xor eax, eax    
    mov edx, dword ptr [esp + 4] ; port_num
    in ax, dx

    RET
ReadPortWord ENDP

;------------------------------------------------------------------------------
;  UINT8
;  ReadPortByte (
;    IN   UINT16    port_num
;    )
;------------------------------------------------------------------------------
ReadPortByte PROC
    xor eax, eax    
    mov edx, dword ptr [esp + 4] ; port_num
    in al, dx

    RET
ReadPortByte ENDP

;------------------------------------------------------------------------------
;  VOID
;  WriteHighCMOSByte (
;    IN   UINT8     cmos_off
;    IN   UINT8     val
;    )
;------------------------------------------------------------------------------
WriteHighCMOSByte PROC
    mov eax, dword ptr [esp + 4]  ; cmos_off
    out 72h, al
    mov eax, dword ptr [esp + 8]  ; val
    out 73h, al

    ret
WriteHighCMOSByte ENDP

;------------------------------------------------------------------------------
;  VOID
;  WriteLowCMOSByte (
;    IN   UINT8     cmos_off
;    IN   UINT8     val
;    )
;------------------------------------------------------------------------------
WriteLowCMOSByte PROC
    mov eax, dword ptr [esp + 4]  ; cmos_off
    or al, 80h
    out 70h, al
    mov eax, dword ptr [esp + 8]  ; val
    out 71h, al

    RET
WriteLowCMOSByte ENDP


;------------------------------------------------------------------------------
;  VOID
;  SendAPMSMI (
;    IN   UINT32	apm_port_value
;    IN   UINT64	rax_value               // NOT USED???
;    )
;------------------------------------------------------------------------------
SendAPMSMI PROC
    mov eax, dword ptr [esp + 4]  ; apm_port_value
    mov dx, 0B2h
    out dx, eax

    RET
SendAPMSMI ENDP

;------------------------------------------------------------------------------
;  VOID
;  WritePCIByte (
;    IN   UINT32    pci_reg
;    IN   UINT16    cfg_data_port
;    IN   UINT8     byte_value
;    )
;------------------------------------------------------------------------------
WritePCIByte PROC
    mov eax, dword ptr [esp + 4]  ; pci_reg
    mov dx, 0CF8h
    out dx, eax

    mov eax, dword ptr [esp + 12]  ; word_value
    mov edx, dword ptr [esp + 8]  ; cfg_data_port
    out dx, al

    RET
WritePCIByte ENDP

;------------------------------------------------------------------------------
;  VOID
;  WritePCIWord (
;    IN   UINT32    pci_reg
;    IN   UINT16    cfg_data_port
;    IN   UINT16    word_value
;    )
;------------------------------------------------------------------------------
WritePCIWord PROC
    mov eax, dword ptr [esp + 4]  ; pci_reg
    mov dx, 0CF8h
    out dx, eax

    mov eax, dword ptr [esp + 12]  ; word_value
    mov edx, dword ptr [esp + 8]  ; cfg_data_port
    out dx, ax

    RET
WritePCIWord ENDP


;------------------------------------------------------------------------------
;  VOID
;  WritePCIDword (
;    IN   UINT32	pci_reg
;    IN   UINT16	cfg_data_port    // rdx
;    IN   UINT32	dword_value      // r8
;    )
;------------------------------------------------------------------------------
WritePCIDword PROC
    mov eax, dword ptr [esp + 4]  ; pci_reg
    mov dx, 0CF8h
    out dx, eax

    mov eax, dword ptr [esp + 12]  ; dword_value
    mov edx, dword ptr [esp + 8]  ; cfg_data_port
    out dx, eax

    RET
WritePCIDword ENDP

;------------------------------------------------------------------------------
;  unsigned char
;  ReadPCIByte (
;    unsigned int	pci_reg          // rcx
;    unsigned short	cfg_data_port    // rdx
;    )
;------------------------------------------------------------------------------
ReadPCIByte PROC
    cli
    mov eax, dword ptr [esp + 4]  ; pci_reg
    mov dx, 0CF8h
    out dx, eax
	
    xor eax, eax	
    mov edx, dword ptr [esp + 8]   ; cfg_data_port
    in  al, dx
    sti

    RET
ReadPCIByte ENDP

;------------------------------------------------------------------------------
;  unsigned short
;  ReadPCIWord (
;    unsigned int	pci_reg          // rcx
;    unsigned short	cfg_data_port    // rdx
;    )
;------------------------------------------------------------------------------
ReadPCIWord PROC
    cli
    mov eax, dword ptr [esp + 4]  ; pci_reg
    mov dx, 0CF8h
    out dx, eax

    xor eax, eax	
    mov edx, dword ptr [esp + 8]   ; cfg_data_port
    in  ax, dx
    sti

    RET
ReadPCIWord ENDP

;------------------------------------------------------------------------------
;  unsigned int
;  ReadPCIDword (
;    unsigned int	pci_reg          // rcx
;    unsigned short	cfg_data_port    // rdx
;    )
;------------------------------------------------------------------------------
ReadPCIDword PROC
    cli
    mov eax, dword ptr [esp + 4]  ; pci_reg
    mov dx, 0CF8h
    out dx, eax

    xor eax, eax	
    mov edx, dword ptr [esp + 8]   ; cfg_data_port
    in  eax, dx
    sti

    RET
ReadPCIDword ENDP

;------------------------------------------------------------------------------
;  void
;  _swsmi (
;    unsigned int	smi_code_data	// rcx
;    IN   UINT32	rax_value	// rdx
;    IN   UINT32	rbx_value	// r8
;    IN   UINT32	rcx_value	// r9
;    IN   UINT32	rdx_value	// r10
;    IN   UINT32	rsi_value	// r11
;    IN   UINT32	rdi_value	// r12
;    )
;------------------------------------------------------------------------------

swsmi PROC
    xor eax, eax
    RET
swsmi ENDP

END
