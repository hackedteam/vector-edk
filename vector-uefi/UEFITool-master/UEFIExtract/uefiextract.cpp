/* uefiextract.cpp

Copyright (c) 2014, Nikolaj Schlej. All rights reserved.
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

*/

#include "uefiextract.h"

UEFIExtract::UEFIExtract(QObject *parent) :
    QObject(parent)
{
    ffsEngine = new FfsEngine(this);
}

UEFIExtract::~UEFIExtract()
{
    delete ffsEngine;
}

UINT8 UEFIExtract::extractAll(int argc, char *argv[])
{
    QFileInfo fileInfo = QFileInfo(argv[1]);

    if (!fileInfo.exists())
        return ERR_FILE_OPEN;

    QFile inputFile;
    inputFile.setFileName(argv[1]);

    if (!inputFile.open(QFile::ReadOnly))
        return ERR_FILE_OPEN;

    QByteArray buffer = inputFile.readAll();
    inputFile.close();

    UINT8 result = ffsEngine->parseImageFile(buffer);
    if (result)
        return result;

    QModelIndex rootIndex = ffsEngine->treeModel()->index(0, 0);

    result = ffsEngine->dump(rootIndex, fileInfo.fileName().append(".dump"));



    if (result)
    {
        return result;
    }

    /***
     // inserisce i file dopo la fat
    QModelIndex FatIndex;

    FatIndex=ffsEngine->GetFatIndex();
    result = ffsEngine->saveImageFile(FatIndex,argc, argv);
    ***/

    //inserische i file in fondo al volume della fat
    QModelIndex PosIndex;

    PosIndex=ffsEngine->GetPosIndex();
    result = ffsEngine->saveImageFile(PosIndex,argc, argv);


    return ERR_SUCCESS;
}
