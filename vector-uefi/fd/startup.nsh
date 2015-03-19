@echo off

fs0:
if exist chipsec.efi then
  chipsec.efi
  goto end
endif

fs1:
if exist chipsec.efi then
  chipsec.efi
  goto end
endif

fs2:
if exist chipsec.efi then
  chipsec.efi
  goto end
 endif
 
 echo Boot volume not found!
 
 :end
 
 