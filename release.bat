build -p OvmfPkg\OvmfPkgIa32.dsc -a IA32 -t VS2008x86 -D USE_OLD_SHELL -b RELEASE
copy /y Build\OvmfX64\RELEASE_VS2008x86\FV\OVMF.fd f:\