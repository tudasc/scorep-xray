# include <iostream>
#ifdef XRAY_INSTRUMENTED
    #include <xray/xray_log_interface.h>
    #include <xray/xray_interface.h>
    #include <xray/xray_records.h>
#endif