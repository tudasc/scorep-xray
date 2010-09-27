 global scorep_ftrace_getname
scorep_ftrace_getname:
	                lds     $s123,3*8(,$s1)         # ($s123) = caller's $s4
	                lea     $s123,8*8(,$s123)       # ($s123) = caller's name
	                b       0(,$s32)
 global scorep_ftrace_getname_len
scorep_ftrace_getname_len:
	                lds     $s123,3*8(,$s1)         # ($s123) = caller's $s4
	                ldl     $s123,8*8-4(,$s123)     # ($s123) = length of caller's name
	                b       0(,$s32)

	