#ifndef __FV_H_
#define __FV_H_

EFI_STATUS 
Py_LookupDxeImage(
  UINT8         *SpiBuffer,
  UINT32        SpiDataSize,
  UINT32		*VolHandle
  );


EFI_STATUS 
Py_FvVolumeGet(
  UINT8         *SpiBuffer,
  UINT32        SpiDataSize,
  UINT32        VolumeIndex,
  UINT8     **OutBuffer,
  UINT32     *OutSize
  );

EFI_STATUS 
 Py_VolumeFromHandle(
   UINT32        VolumeHandle,
   UINT32       *VolIndex
 );

EFI_STATUS 
Py_FvOpenVolume(
  UINT8      *SpiBuffer,
  UINT32      SpiLength,
  UINT32      VolumeHandle,
  UINT8     **OutBuffer,
  UINT32     *OutSize
  );

EFI_STATUS
Py_FvVolumeAddFile(
  UINT8         *VolumeBuffer,
  UINT32        VolumeLength,
  UINT8         *NewFileBuffer,
  UINT32        NewFileLength
  );


EFI_STATUS
Py_FvCloseVolume(
  UINT8         *VolumeBuffer,
  UINT32        VolumeLength,
  UINT8         *DxeFileBuffer,
  UINT32        DxeLength,
  UINT32        VolumeHandle,
  UINT8     **OutBuffer,
  UINT32     *OutSize
  );


UINT32
Py_FvVolumeAddress(
  UINT8         *SpiBuffer,
  UINT32        SpiDataSize,
  UINT32        VolumeIndex
  );

UINT32
Py_FvVolumeSize(
  UINT8         *SrcBuf,
  UINT32        SrcDataSize,
  UINT32        VolumeIndex
  );

EFI_STATUS
Py_FvDeleteFileFromVolume(
  UINT8         *VolumeBuffer,
  UINT32        VolumeLength,
  UINT8         *NewFileBuffer,
  UINT32        NewFileLength
  );

UINT8 * ptr_to_volume(UINT8 *buffer, UINT32 size, UINT8 index);

UINT32 volume_size(UINT8 *buffer, UINT32 size, UINT8 index);
#endif
