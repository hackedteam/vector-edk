/** @file

Copyright (c) 2009 - 2010, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available 
under the terms and conditions of the BSD License which accompanies this 
distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Modified for uefi_firmware_parser:
This includes minor API changes for Tiano and EFI decompressor, as well as LZMA.

**/

#include <Python.h>

#include "Include/CompressionTypes.h"

#include "Tiano/Decompress.h"
#include "Tiano/Compress.h"
#include "LZMA/LzmaDecompress.h"
#include "LZMA/LzmaCompress.h"

//#include "EfiFile.h"

#define EFI_COMPRESSION   1 //defined as PI_STD, section type= 0x01
#define TIANO_COMPRESSION 2 //not defined, section type= 0x01
#define LZMA_COMPRESSION  3 //not defined, section type= 0x02

