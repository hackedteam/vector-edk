rem build -p OvmfPkg\OvmfPkgIa32.dsc -a IA32
build -a IA32 -p OvmfPkg\OvmfPkgIA32.dsc -D SOURCE_DEBUG_ENABLE -t VS2008x86 -D USE_OLD_SHELL -D INCLUDE_VECTOR
copy /y Build\OvmfIa32\DEBUG_VS2008x86\FV\OVMF.fd D:\TEST
