@echo off

mv chipsec\utilcmd\spi_cmd.py chipsec\utilcmd\spi_cmd.p

mv chipsec\utilcmd\spi_cmd.p chipsec\utilcmd\spi_cmd.py

if not exist \tmp\spi.bin then
python chipsec_util.py spi dump \tmp\spi.bin
endif

if not exist \tmp\vol02.bin then
python2 patch.py
endif

if exist flash.nsh then
echo "Run flash procedure...."
flash.nsh
endif

