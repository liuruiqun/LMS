@REM This bat file has been generated by the IAR Embeddded Workbench
@REM C-SPY interactive debugger,as an aid to preparing a command
@REM line for running the cspybat command line utility with the
@REM appropriate settings.
@REM
@REM After making some adjustments to this file, you can launch cspybat
@REM by typing the name of this file followed by the name of the debug
@REM file (usually an ubrof file). Note that this file is generated
@REM every time a new debug session is initialized, so you may want to
@REM move or rename the file before making changes.
@REM
@REM Note: some command line arguments cannot be properly generated
@REM by this process. Specifically, the plugin which is responsible
@REM for the Terminal I/O window (and other C runtime functionality)
@REM comes in a special version for cspybat, and the name of that
@REM plugin dll is not known when generating this file. It resides in
@REM the $TOOLKIT_DIR$\bin folder and is usually called XXXbat.dll or
@REM XXXlibsupportbat.dll, where XXX is the name of the corresponding
@REM tool chain. Replace the '<libsupport_plugin>' parameter
@REM below with the appropriate file name. Other plugins loaded by
@REM C-SPY are usually not needed by, or will not work in, cspybat
@REM but they are listed at the end of this file for reference.


"C:\Program Files\IAR Systems\Embedded Workbench 4.0 Evaluation version\common\bin\cspybat" "C:\Program Files\IAR Systems\Embedded Workbench 4.0 Evaluation version\8051\bin\8051proc.dll" "C:\Program Files\IAR Systems\Embedded Workbench 4.0 Evaluation version\8051\bin\8051emu_cc.dll"  %1 --plugin "C:\Program Files\IAR Systems\Embedded Workbench 4.0 Evaluation version\8051\bin\<libsupport_plugin>" --backend -B "-p" "C:\Program Files\IAR Systems\Embedded Workbench 4.0 Evaluation version\8051\config\derivatives\Chipcon\CC2430.ddf" "--proc_core" "plain" "--proc_code_model" "banked" "--proc_nr_virtual_regs" "8" "--proc_dptr_nr_of" "1" "--proc_DPL1" "0x84" "--proc_DPH1" "0x85" "--proc_codebank_reg" "0xC7" "--proc_codebank_start" "0x2900" "--proc_codebank_end" "0xFFFF" "--derivative_file" "C:\Program Files\IAR Systems\Embedded Workbench 4.0 Evaluation version\8051\config\derivatives\Chipcon\CC2430.i51" "--proc_driver" "chipcon" "--verify_download" "use_crc16" "--cache_code" "--stack_overflow" "--no_banks=4" 


@REM Loaded plugins:
@REM    8051libsupport.dll
@REM    C:\Program Files\IAR Systems\Embedded Workbench 4.0 Evaluation version\common\plugins\CodeCoverage\CodeCoverage.dll
@REM    C:\Program Files\IAR Systems\Embedded Workbench 4.0 Evaluation version\common\plugins\Profiling\Profiling.dll
@REM    C:\Program Files\IAR Systems\Embedded Workbench 4.0 Evaluation version\common\plugins\stack\stack.dll
