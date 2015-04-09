UINT8	CalculateChecksum8(UINT8 *Buffer, UINTN Size);
UINT8	CalculateSum8(UINT8 *Buffer, UINTN Size);

UINT16 CalculateChecksum16( UINT16 *Buffer, UINTN Size);
UINT16 CalculateSum16 (UINT16       *Buffer, UINTN        Size);
DWORD Expand24bit(UINT8 *ptr);
UINT32 efi_ffs_file_size(PEFI_FFS_FILE_HEADER pFile);
VOID Pack24bit(UINT32 value, UINT8 *ffsSize);