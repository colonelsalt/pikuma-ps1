@echo off
setlocal enabledelayedexpansion

for %%f in (*.asm) do (
	armips.exe %%f

	set "bin_file=%%~nf.bin"
	set "exe_file=%%~nf.ps-exe"
	
	python bin2exe.py !bin_file! !exe_file!

)

endlocal
