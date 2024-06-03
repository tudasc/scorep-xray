# include <iostream>

// Don't check whether XRAY plugin is enabled since this header is only included when xray plugin instrumentation
// is enabled, which means compiling and linking with -fxray-instrumented
#include <xray/xray_log_interface.h>
#include <xray/xray_interface.h>
#include <xray/xray_records.h>
