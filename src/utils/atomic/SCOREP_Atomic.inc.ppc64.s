	.file	"SCOREP_Atomic.inc.c"
	.abiversion 2
	.section	".toc","aw"
	.align 3
	.section	".text"
	.machine power8
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_TestAndSet
	.type	SCOREP_Atomic_TestAndSet, @function
SCOREP_Atomic_TestAndSet:
	sync
	li 9,1
.L2:
	lbarx 10,0,3
	stbcx. 9,0,3
	bne- 0,.L2
	isync
	rldicl 3,10,0,56
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_TestAndSet,.-SCOREP_Atomic_TestAndSet
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_clear
	.type	SCOREP_Atomic_clear, @function
SCOREP_Atomic_clear:
	sync
	li 9,0
	stb 9,0(3)
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_clear,.-SCOREP_Atomic_clear
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_ThreadFence
	.type	SCOREP_Atomic_ThreadFence, @function
SCOREP_Atomic_ThreadFence:
	sync
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_ThreadFence,.-SCOREP_Atomic_ThreadFence
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_LoadN_void_ptr
	.type	SCOREP_Atomic_LoadN_void_ptr, @function
SCOREP_Atomic_LoadN_void_ptr:
	sync
	ld 3,0(3)
	cmpw 7,3,3
	bne- 7,$+4
	isync
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_LoadN_void_ptr,.-SCOREP_Atomic_LoadN_void_ptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_StoreN_void_ptr
	.type	SCOREP_Atomic_StoreN_void_ptr, @function
SCOREP_Atomic_StoreN_void_ptr:
	sync
	std 4,0(3)
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_StoreN_void_ptr,.-SCOREP_Atomic_StoreN_void_ptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_ExchangeN_void_ptr
	.type	SCOREP_Atomic_ExchangeN_void_ptr, @function
SCOREP_Atomic_ExchangeN_void_ptr:
	sync
.L10:
	ldarx 9,0,3
	stdcx. 4,0,3
	bne- 0,.L10
	isync
	mr 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_ExchangeN_void_ptr,.-SCOREP_Atomic_ExchangeN_void_ptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_CompareExchangeN_void_ptr
	.type	SCOREP_Atomic_CompareExchangeN_void_ptr, @function
SCOREP_Atomic_CompareExchangeN_void_ptr:
	ld 10,0(4)
	sync
.L13:
	ldarx 9,0,3
	cmpd 0,9,10
	bne- 0,.L14
	stdcx. 5,0,3
	bne- 0,.L13
.L14:
	isync
	mfcr 3,128
	rlwinm 3,3,3,1
	bne 0,.L17
	rldicl 3,3,0,63
	blr
	.p2align 4,,15
.L17:
	std 9,0(4)
	rldicl 3,3,0,63
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_CompareExchangeN_void_ptr,.-SCOREP_Atomic_CompareExchangeN_void_ptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_AddFetch_void_ptr
	.type	SCOREP_Atomic_AddFetch_void_ptr, @function
SCOREP_Atomic_AddFetch_void_ptr:
	sync
	mr 9,3
.L19:
	ldarx 3,0,9
	add 3,4,3
	stdcx. 3,0,9
	bne- 0,.L19
	isync
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_AddFetch_void_ptr,.-SCOREP_Atomic_AddFetch_void_ptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_SubFetch_void_ptr
	.type	SCOREP_Atomic_SubFetch_void_ptr, @function
SCOREP_Atomic_SubFetch_void_ptr:
	sync
	mr 9,3
.L22:
	ldarx 3,0,9
	subf 3,4,3
	stdcx. 3,0,9
	bne- 0,.L22
	isync
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_SubFetch_void_ptr,.-SCOREP_Atomic_SubFetch_void_ptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_AndFetch_void_ptr
	.type	SCOREP_Atomic_AndFetch_void_ptr, @function
SCOREP_Atomic_AndFetch_void_ptr:
	sync
	mr 9,3
.L25:
	ldarx 3,0,9
	and 3,4,3
	stdcx. 3,0,9
	bne- 0,.L25
	isync
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_AndFetch_void_ptr,.-SCOREP_Atomic_AndFetch_void_ptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_XorFetch_void_ptr
	.type	SCOREP_Atomic_XorFetch_void_ptr, @function
SCOREP_Atomic_XorFetch_void_ptr:
	sync
	mr 9,3
.L28:
	ldarx 3,0,9
	xor 3,4,3
	stdcx. 3,0,9
	bne- 0,.L28
	isync
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_XorFetch_void_ptr,.-SCOREP_Atomic_XorFetch_void_ptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_OrFetch_void_ptr
	.type	SCOREP_Atomic_OrFetch_void_ptr, @function
SCOREP_Atomic_OrFetch_void_ptr:
	sync
	mr 9,3
.L31:
	ldarx 3,0,9
	or 3,4,3
	stdcx. 3,0,9
	bne- 0,.L31
	isync
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_OrFetch_void_ptr,.-SCOREP_Atomic_OrFetch_void_ptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_NandFetch_void_ptr
	.type	SCOREP_Atomic_NandFetch_void_ptr, @function
SCOREP_Atomic_NandFetch_void_ptr:
	sync
	mr 9,3
.L34:
	ldarx 3,0,9
	nand 3,4,3
	stdcx. 3,0,9
	bne- 0,.L34
	isync
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_NandFetch_void_ptr,.-SCOREP_Atomic_NandFetch_void_ptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchAdd_void_ptr
	.type	SCOREP_Atomic_FetchAdd_void_ptr, @function
SCOREP_Atomic_FetchAdd_void_ptr:
	sync
.L37:
	ldarx 9,0,3
	add 10,4,9
	stdcx. 10,0,3
	bne- 0,.L37
	isync
	mr 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchAdd_void_ptr,.-SCOREP_Atomic_FetchAdd_void_ptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchSub_void_ptr
	.type	SCOREP_Atomic_FetchSub_void_ptr, @function
SCOREP_Atomic_FetchSub_void_ptr:
	sync
.L40:
	ldarx 9,0,3
	subf 10,4,9
	stdcx. 10,0,3
	bne- 0,.L40
	isync
	mr 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchSub_void_ptr,.-SCOREP_Atomic_FetchSub_void_ptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchAnd_void_ptr
	.type	SCOREP_Atomic_FetchAnd_void_ptr, @function
SCOREP_Atomic_FetchAnd_void_ptr:
	sync
.L43:
	ldarx 9,0,3
	and 10,4,9
	stdcx. 10,0,3
	bne- 0,.L43
	isync
	mr 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchAnd_void_ptr,.-SCOREP_Atomic_FetchAnd_void_ptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchXor_void_ptr
	.type	SCOREP_Atomic_FetchXor_void_ptr, @function
SCOREP_Atomic_FetchXor_void_ptr:
	sync
.L46:
	ldarx 9,0,3
	xor 10,4,9
	stdcx. 10,0,3
	bne- 0,.L46
	isync
	mr 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchXor_void_ptr,.-SCOREP_Atomic_FetchXor_void_ptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchOr_void_ptr
	.type	SCOREP_Atomic_FetchOr_void_ptr, @function
SCOREP_Atomic_FetchOr_void_ptr:
	sync
.L49:
	ldarx 9,0,3
	or 10,4,9
	stdcx. 10,0,3
	bne- 0,.L49
	isync
	mr 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchOr_void_ptr,.-SCOREP_Atomic_FetchOr_void_ptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchNand_void_ptr
	.type	SCOREP_Atomic_FetchNand_void_ptr, @function
SCOREP_Atomic_FetchNand_void_ptr:
	sync
.L52:
	ldarx 9,0,3
	nand 10,4,9
	stdcx. 10,0,3
	bne- 0,.L52
	isync
	mr 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchNand_void_ptr,.-SCOREP_Atomic_FetchNand_void_ptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_LoadN_bool
	.type	SCOREP_Atomic_LoadN_bool, @function
SCOREP_Atomic_LoadN_bool:
	sync
	lbz 9,0(3)
	cmpw 7,9,9
	bne- 7,$+4
	isync
	rldicl 9,9,0,56
	neg 3,9
	srdi 3,3,63
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_LoadN_bool,.-SCOREP_Atomic_LoadN_bool
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_StoreN_bool
	.type	SCOREP_Atomic_StoreN_bool, @function
SCOREP_Atomic_StoreN_bool:
	sync
	stb 4,0(3)
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_StoreN_bool,.-SCOREP_Atomic_StoreN_bool
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_ExchangeN_bool
	.type	SCOREP_Atomic_ExchangeN_bool, @function
SCOREP_Atomic_ExchangeN_bool:
	sync
.L57:
	lbarx 9,0,3
	stbcx. 4,0,3
	bne- 0,.L57
	isync
	rldicl 9,9,0,56
	neg 3,9
	srdi 3,3,63
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_ExchangeN_bool,.-SCOREP_Atomic_ExchangeN_bool
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_CompareExchangeN_bool
	.type	SCOREP_Atomic_CompareExchangeN_bool, @function
SCOREP_Atomic_CompareExchangeN_bool:
	lbz 10,0(4)
	sync
.L60:
	lbarx 9,0,3
	cmpw 0,9,10
	bne- 0,.L61
	stbcx. 5,0,3
	bne- 0,.L60
.L61:
	isync
	mfcr 3,128
	rlwinm 3,3,3,1
	bne 0,.L64
	rldicl 3,3,0,63
	blr
	.p2align 4,,15
.L64:
	stb 9,0(4)
	rldicl 3,3,0,63
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_CompareExchangeN_bool,.-SCOREP_Atomic_CompareExchangeN_bool
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_LoadN_char
	.type	SCOREP_Atomic_LoadN_char, @function
SCOREP_Atomic_LoadN_char:
	sync
	lbz 3,0(3)
	cmpw 7,3,3
	bne- 7,$+4
	isync
	rldicl 3,3,0,56
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_LoadN_char,.-SCOREP_Atomic_LoadN_char
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_StoreN_char
	.type	SCOREP_Atomic_StoreN_char, @function
SCOREP_Atomic_StoreN_char:
	sync
	stb 4,0(3)
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_StoreN_char,.-SCOREP_Atomic_StoreN_char
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_ExchangeN_char
	.type	SCOREP_Atomic_ExchangeN_char, @function
SCOREP_Atomic_ExchangeN_char:
	sync
.L68:
	lbarx 9,0,3
	stbcx. 4,0,3
	bne- 0,.L68
	isync
	rldicl 3,9,0,56
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_ExchangeN_char,.-SCOREP_Atomic_ExchangeN_char
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_CompareExchangeN_char
	.type	SCOREP_Atomic_CompareExchangeN_char, @function
SCOREP_Atomic_CompareExchangeN_char:
	lbz 10,0(4)
	sync
.L71:
	lbarx 9,0,3
	cmpw 0,9,10
	bne- 0,.L72
	stbcx. 5,0,3
	bne- 0,.L71
.L72:
	isync
	mfcr 3,128
	rlwinm 3,3,3,1
	bne 0,.L75
	rldicl 3,3,0,63
	blr
	.p2align 4,,15
.L75:
	stb 9,0(4)
	rldicl 3,3,0,63
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_CompareExchangeN_char,.-SCOREP_Atomic_CompareExchangeN_char
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_AddFetch_char
	.type	SCOREP_Atomic_AddFetch_char, @function
SCOREP_Atomic_AddFetch_char:
	sync
.L77:
	lbarx 9,0,3
	add 9,9,4
	stbcx. 9,0,3
	bne- 0,.L77
	isync
	rldicl 3,9,0,56
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_AddFetch_char,.-SCOREP_Atomic_AddFetch_char
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_SubFetch_char
	.type	SCOREP_Atomic_SubFetch_char, @function
SCOREP_Atomic_SubFetch_char:
	sync
.L80:
	lbarx 9,0,3
	subf 9,4,9
	stbcx. 9,0,3
	bne- 0,.L80
	isync
	rldicl 3,9,0,56
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_SubFetch_char,.-SCOREP_Atomic_SubFetch_char
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_AndFetch_char
	.type	SCOREP_Atomic_AndFetch_char, @function
SCOREP_Atomic_AndFetch_char:
	sync
.L83:
	lbarx 9,0,3
	and 9,9,4
	stbcx. 9,0,3
	bne- 0,.L83
	isync
	rldicl 3,9,0,56
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_AndFetch_char,.-SCOREP_Atomic_AndFetch_char
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_XorFetch_char
	.type	SCOREP_Atomic_XorFetch_char, @function
SCOREP_Atomic_XorFetch_char:
	sync
.L86:
	lbarx 9,0,3
	xor 9,9,4
	stbcx. 9,0,3
	bne- 0,.L86
	isync
	rldicl 3,9,0,56
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_XorFetch_char,.-SCOREP_Atomic_XorFetch_char
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_OrFetch_char
	.type	SCOREP_Atomic_OrFetch_char, @function
SCOREP_Atomic_OrFetch_char:
	sync
.L89:
	lbarx 9,0,3
	or 9,9,4
	stbcx. 9,0,3
	bne- 0,.L89
	isync
	rldicl 3,9,0,56
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_OrFetch_char,.-SCOREP_Atomic_OrFetch_char
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_NandFetch_char
	.type	SCOREP_Atomic_NandFetch_char, @function
SCOREP_Atomic_NandFetch_char:
	sync
.L92:
	lbarx 9,0,3
	nand 9,9,4
	stbcx. 9,0,3
	bne- 0,.L92
	isync
	rldicl 3,9,0,56
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_NandFetch_char,.-SCOREP_Atomic_NandFetch_char
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchAdd_char
	.type	SCOREP_Atomic_FetchAdd_char, @function
SCOREP_Atomic_FetchAdd_char:
	sync
.L95:
	lbarx 9,0,3
	add 10,9,4
	stbcx. 10,0,3
	bne- 0,.L95
	isync
	rldicl 3,9,0,56
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchAdd_char,.-SCOREP_Atomic_FetchAdd_char
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchSub_char
	.type	SCOREP_Atomic_FetchSub_char, @function
SCOREP_Atomic_FetchSub_char:
	sync
.L98:
	lbarx 9,0,3
	subf 10,4,9
	stbcx. 10,0,3
	bne- 0,.L98
	isync
	rldicl 3,9,0,56
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchSub_char,.-SCOREP_Atomic_FetchSub_char
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchAnd_char
	.type	SCOREP_Atomic_FetchAnd_char, @function
SCOREP_Atomic_FetchAnd_char:
	sync
.L101:
	lbarx 9,0,3
	and 10,9,4
	stbcx. 10,0,3
	bne- 0,.L101
	isync
	rldicl 3,9,0,56
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchAnd_char,.-SCOREP_Atomic_FetchAnd_char
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchXor_char
	.type	SCOREP_Atomic_FetchXor_char, @function
SCOREP_Atomic_FetchXor_char:
	sync
.L104:
	lbarx 9,0,3
	xor 10,9,4
	stbcx. 10,0,3
	bne- 0,.L104
	isync
	rldicl 3,9,0,56
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchXor_char,.-SCOREP_Atomic_FetchXor_char
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchOr_char
	.type	SCOREP_Atomic_FetchOr_char, @function
SCOREP_Atomic_FetchOr_char:
	sync
.L107:
	lbarx 9,0,3
	or 10,9,4
	stbcx. 10,0,3
	bne- 0,.L107
	isync
	rldicl 3,9,0,56
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchOr_char,.-SCOREP_Atomic_FetchOr_char
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchNand_char
	.type	SCOREP_Atomic_FetchNand_char, @function
SCOREP_Atomic_FetchNand_char:
	sync
.L110:
	lbarx 9,0,3
	nand 10,9,4
	stbcx. 10,0,3
	bne- 0,.L110
	isync
	rldicl 3,9,0,56
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchNand_char,.-SCOREP_Atomic_FetchNand_char
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_LoadN_uint8
	.type	SCOREP_Atomic_LoadN_uint8, @function
SCOREP_Atomic_LoadN_uint8:
	sync
	lbz 3,0(3)
	cmpw 7,3,3
	bne- 7,$+4
	isync
	rldicl 3,3,0,56
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_LoadN_uint8,.-SCOREP_Atomic_LoadN_uint8
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_StoreN_uint8
	.type	SCOREP_Atomic_StoreN_uint8, @function
SCOREP_Atomic_StoreN_uint8:
	sync
	stb 4,0(3)
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_StoreN_uint8,.-SCOREP_Atomic_StoreN_uint8
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_ExchangeN_uint8
	.type	SCOREP_Atomic_ExchangeN_uint8, @function
SCOREP_Atomic_ExchangeN_uint8:
	sync
.L115:
	lbarx 9,0,3
	stbcx. 4,0,3
	bne- 0,.L115
	isync
	rldicl 3,9,0,56
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_ExchangeN_uint8,.-SCOREP_Atomic_ExchangeN_uint8
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_CompareExchangeN_uint8
	.type	SCOREP_Atomic_CompareExchangeN_uint8, @function
SCOREP_Atomic_CompareExchangeN_uint8:
	lbz 10,0(4)
	sync
.L118:
	lbarx 9,0,3
	cmpw 0,9,10
	bne- 0,.L119
	stbcx. 5,0,3
	bne- 0,.L118
.L119:
	isync
	mfcr 3,128
	rlwinm 3,3,3,1
	bne 0,.L122
	rldicl 3,3,0,63
	blr
	.p2align 4,,15
.L122:
	stb 9,0(4)
	rldicl 3,3,0,63
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_CompareExchangeN_uint8,.-SCOREP_Atomic_CompareExchangeN_uint8
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_AddFetch_uint8
	.type	SCOREP_Atomic_AddFetch_uint8, @function
SCOREP_Atomic_AddFetch_uint8:
	sync
.L124:
	lbarx 9,0,3
	add 9,9,4
	stbcx. 9,0,3
	bne- 0,.L124
	isync
	rldicl 3,9,0,56
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_AddFetch_uint8,.-SCOREP_Atomic_AddFetch_uint8
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_SubFetch_uint8
	.type	SCOREP_Atomic_SubFetch_uint8, @function
SCOREP_Atomic_SubFetch_uint8:
	sync
.L127:
	lbarx 9,0,3
	subf 9,4,9
	stbcx. 9,0,3
	bne- 0,.L127
	isync
	rldicl 3,9,0,56
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_SubFetch_uint8,.-SCOREP_Atomic_SubFetch_uint8
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_AndFetch_uint8
	.type	SCOREP_Atomic_AndFetch_uint8, @function
SCOREP_Atomic_AndFetch_uint8:
	sync
.L130:
	lbarx 9,0,3
	and 9,9,4
	stbcx. 9,0,3
	bne- 0,.L130
	isync
	rldicl 3,9,0,56
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_AndFetch_uint8,.-SCOREP_Atomic_AndFetch_uint8
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_XorFetch_uint8
	.type	SCOREP_Atomic_XorFetch_uint8, @function
SCOREP_Atomic_XorFetch_uint8:
	sync
.L133:
	lbarx 9,0,3
	xor 9,9,4
	stbcx. 9,0,3
	bne- 0,.L133
	isync
	rldicl 3,9,0,56
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_XorFetch_uint8,.-SCOREP_Atomic_XorFetch_uint8
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_OrFetch_uint8
	.type	SCOREP_Atomic_OrFetch_uint8, @function
SCOREP_Atomic_OrFetch_uint8:
	sync
.L136:
	lbarx 9,0,3
	or 9,9,4
	stbcx. 9,0,3
	bne- 0,.L136
	isync
	rldicl 3,9,0,56
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_OrFetch_uint8,.-SCOREP_Atomic_OrFetch_uint8
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_NandFetch_uint8
	.type	SCOREP_Atomic_NandFetch_uint8, @function
SCOREP_Atomic_NandFetch_uint8:
	sync
.L139:
	lbarx 9,0,3
	nand 9,9,4
	stbcx. 9,0,3
	bne- 0,.L139
	isync
	rldicl 3,9,0,56
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_NandFetch_uint8,.-SCOREP_Atomic_NandFetch_uint8
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchAdd_uint8
	.type	SCOREP_Atomic_FetchAdd_uint8, @function
SCOREP_Atomic_FetchAdd_uint8:
	sync
.L142:
	lbarx 9,0,3
	add 10,9,4
	stbcx. 10,0,3
	bne- 0,.L142
	isync
	rldicl 3,9,0,56
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchAdd_uint8,.-SCOREP_Atomic_FetchAdd_uint8
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchSub_uint8
	.type	SCOREP_Atomic_FetchSub_uint8, @function
SCOREP_Atomic_FetchSub_uint8:
	sync
.L145:
	lbarx 9,0,3
	subf 10,4,9
	stbcx. 10,0,3
	bne- 0,.L145
	isync
	rldicl 3,9,0,56
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchSub_uint8,.-SCOREP_Atomic_FetchSub_uint8
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchAnd_uint8
	.type	SCOREP_Atomic_FetchAnd_uint8, @function
SCOREP_Atomic_FetchAnd_uint8:
	sync
.L148:
	lbarx 9,0,3
	and 10,9,4
	stbcx. 10,0,3
	bne- 0,.L148
	isync
	rldicl 3,9,0,56
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchAnd_uint8,.-SCOREP_Atomic_FetchAnd_uint8
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchXor_uint8
	.type	SCOREP_Atomic_FetchXor_uint8, @function
SCOREP_Atomic_FetchXor_uint8:
	sync
.L151:
	lbarx 9,0,3
	xor 10,9,4
	stbcx. 10,0,3
	bne- 0,.L151
	isync
	rldicl 3,9,0,56
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchXor_uint8,.-SCOREP_Atomic_FetchXor_uint8
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchOr_uint8
	.type	SCOREP_Atomic_FetchOr_uint8, @function
SCOREP_Atomic_FetchOr_uint8:
	sync
.L154:
	lbarx 9,0,3
	or 10,9,4
	stbcx. 10,0,3
	bne- 0,.L154
	isync
	rldicl 3,9,0,56
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchOr_uint8,.-SCOREP_Atomic_FetchOr_uint8
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchNand_uint8
	.type	SCOREP_Atomic_FetchNand_uint8, @function
SCOREP_Atomic_FetchNand_uint8:
	sync
.L157:
	lbarx 9,0,3
	nand 10,9,4
	stbcx. 10,0,3
	bne- 0,.L157
	isync
	rldicl 3,9,0,56
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchNand_uint8,.-SCOREP_Atomic_FetchNand_uint8
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_LoadN_uint16
	.type	SCOREP_Atomic_LoadN_uint16, @function
SCOREP_Atomic_LoadN_uint16:
	sync
	lhz 3,0(3)
	cmpw 7,3,3
	bne- 7,$+4
	isync
	rldicl 3,3,0,48
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_LoadN_uint16,.-SCOREP_Atomic_LoadN_uint16
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_StoreN_uint16
	.type	SCOREP_Atomic_StoreN_uint16, @function
SCOREP_Atomic_StoreN_uint16:
	sync
	sth 4,0(3)
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_StoreN_uint16,.-SCOREP_Atomic_StoreN_uint16
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_ExchangeN_uint16
	.type	SCOREP_Atomic_ExchangeN_uint16, @function
SCOREP_Atomic_ExchangeN_uint16:
	sync
.L162:
	lharx 9,0,3
	sthcx. 4,0,3
	bne- 0,.L162
	isync
	rldicl 3,9,0,48
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_ExchangeN_uint16,.-SCOREP_Atomic_ExchangeN_uint16
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_CompareExchangeN_uint16
	.type	SCOREP_Atomic_CompareExchangeN_uint16, @function
SCOREP_Atomic_CompareExchangeN_uint16:
	lhz 10,0(4)
	sync
.L165:
	lharx 9,0,3
	cmpw 0,9,10
	bne- 0,.L166
	sthcx. 5,0,3
	bne- 0,.L165
.L166:
	isync
	mfcr 3,128
	rlwinm 3,3,3,1
	bne 0,.L169
	rldicl 3,3,0,63
	blr
	.p2align 4,,15
.L169:
	sth 9,0(4)
	rldicl 3,3,0,63
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_CompareExchangeN_uint16,.-SCOREP_Atomic_CompareExchangeN_uint16
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_AddFetch_uint16
	.type	SCOREP_Atomic_AddFetch_uint16, @function
SCOREP_Atomic_AddFetch_uint16:
	sync
.L171:
	lharx 9,0,3
	add 9,9,4
	sthcx. 9,0,3
	bne- 0,.L171
	isync
	rldicl 3,9,0,48
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_AddFetch_uint16,.-SCOREP_Atomic_AddFetch_uint16
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_SubFetch_uint16
	.type	SCOREP_Atomic_SubFetch_uint16, @function
SCOREP_Atomic_SubFetch_uint16:
	sync
.L174:
	lharx 9,0,3
	subf 9,4,9
	sthcx. 9,0,3
	bne- 0,.L174
	isync
	rldicl 3,9,0,48
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_SubFetch_uint16,.-SCOREP_Atomic_SubFetch_uint16
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_AndFetch_uint16
	.type	SCOREP_Atomic_AndFetch_uint16, @function
SCOREP_Atomic_AndFetch_uint16:
	sync
.L177:
	lharx 9,0,3
	and 9,9,4
	sthcx. 9,0,3
	bne- 0,.L177
	isync
	rldicl 3,9,0,48
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_AndFetch_uint16,.-SCOREP_Atomic_AndFetch_uint16
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_XorFetch_uint16
	.type	SCOREP_Atomic_XorFetch_uint16, @function
SCOREP_Atomic_XorFetch_uint16:
	sync
.L180:
	lharx 9,0,3
	xor 9,9,4
	sthcx. 9,0,3
	bne- 0,.L180
	isync
	rldicl 3,9,0,48
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_XorFetch_uint16,.-SCOREP_Atomic_XorFetch_uint16
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_OrFetch_uint16
	.type	SCOREP_Atomic_OrFetch_uint16, @function
SCOREP_Atomic_OrFetch_uint16:
	sync
.L183:
	lharx 9,0,3
	or 9,9,4
	sthcx. 9,0,3
	bne- 0,.L183
	isync
	rldicl 3,9,0,48
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_OrFetch_uint16,.-SCOREP_Atomic_OrFetch_uint16
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_NandFetch_uint16
	.type	SCOREP_Atomic_NandFetch_uint16, @function
SCOREP_Atomic_NandFetch_uint16:
	sync
.L186:
	lharx 9,0,3
	nand 9,9,4
	sthcx. 9,0,3
	bne- 0,.L186
	isync
	rldicl 3,9,0,48
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_NandFetch_uint16,.-SCOREP_Atomic_NandFetch_uint16
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchAdd_uint16
	.type	SCOREP_Atomic_FetchAdd_uint16, @function
SCOREP_Atomic_FetchAdd_uint16:
	sync
.L189:
	lharx 9,0,3
	add 10,9,4
	sthcx. 10,0,3
	bne- 0,.L189
	isync
	rldicl 3,9,0,48
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchAdd_uint16,.-SCOREP_Atomic_FetchAdd_uint16
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchSub_uint16
	.type	SCOREP_Atomic_FetchSub_uint16, @function
SCOREP_Atomic_FetchSub_uint16:
	sync
.L192:
	lharx 9,0,3
	subf 10,4,9
	sthcx. 10,0,3
	bne- 0,.L192
	isync
	rldicl 3,9,0,48
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchSub_uint16,.-SCOREP_Atomic_FetchSub_uint16
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchAnd_uint16
	.type	SCOREP_Atomic_FetchAnd_uint16, @function
SCOREP_Atomic_FetchAnd_uint16:
	sync
.L195:
	lharx 9,0,3
	and 10,9,4
	sthcx. 10,0,3
	bne- 0,.L195
	isync
	rldicl 3,9,0,48
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchAnd_uint16,.-SCOREP_Atomic_FetchAnd_uint16
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchXor_uint16
	.type	SCOREP_Atomic_FetchXor_uint16, @function
SCOREP_Atomic_FetchXor_uint16:
	sync
.L198:
	lharx 9,0,3
	xor 10,9,4
	sthcx. 10,0,3
	bne- 0,.L198
	isync
	rldicl 3,9,0,48
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchXor_uint16,.-SCOREP_Atomic_FetchXor_uint16
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchOr_uint16
	.type	SCOREP_Atomic_FetchOr_uint16, @function
SCOREP_Atomic_FetchOr_uint16:
	sync
.L201:
	lharx 9,0,3
	or 10,9,4
	sthcx. 10,0,3
	bne- 0,.L201
	isync
	rldicl 3,9,0,48
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchOr_uint16,.-SCOREP_Atomic_FetchOr_uint16
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchNand_uint16
	.type	SCOREP_Atomic_FetchNand_uint16, @function
SCOREP_Atomic_FetchNand_uint16:
	sync
.L204:
	lharx 9,0,3
	nand 10,9,4
	sthcx. 10,0,3
	bne- 0,.L204
	isync
	rldicl 3,9,0,48
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchNand_uint16,.-SCOREP_Atomic_FetchNand_uint16
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_LoadN_uint32
	.type	SCOREP_Atomic_LoadN_uint32, @function
SCOREP_Atomic_LoadN_uint32:
	sync
	lwz 3,0(3)
	cmpw 7,3,3
	bne- 7,$+4
	isync
	rldicl 3,3,0,32
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_LoadN_uint32,.-SCOREP_Atomic_LoadN_uint32
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_StoreN_uint32
	.type	SCOREP_Atomic_StoreN_uint32, @function
SCOREP_Atomic_StoreN_uint32:
	sync
	stw 4,0(3)
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_StoreN_uint32,.-SCOREP_Atomic_StoreN_uint32
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_ExchangeN_uint32
	.type	SCOREP_Atomic_ExchangeN_uint32, @function
SCOREP_Atomic_ExchangeN_uint32:
	sync
.L209:
	lwarx 9,0,3
	stwcx. 4,0,3
	bne- 0,.L209
	isync
	rldicl 3,9,0,32
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_ExchangeN_uint32,.-SCOREP_Atomic_ExchangeN_uint32
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_CompareExchangeN_uint32
	.type	SCOREP_Atomic_CompareExchangeN_uint32, @function
SCOREP_Atomic_CompareExchangeN_uint32:
	lwz 10,0(4)
	sync
.L212:
	lwarx 9,0,3
	cmpw 0,9,10
	bne- 0,.L213
	stwcx. 5,0,3
	bne- 0,.L212
.L213:
	isync
	mfcr 3,128
	rlwinm 3,3,3,1
	bne 0,.L216
	rldicl 3,3,0,63
	blr
	.p2align 4,,15
.L216:
	stw 9,0(4)
	rldicl 3,3,0,63
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_CompareExchangeN_uint32,.-SCOREP_Atomic_CompareExchangeN_uint32
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_AddFetch_uint32
	.type	SCOREP_Atomic_AddFetch_uint32, @function
SCOREP_Atomic_AddFetch_uint32:
	sync
.L218:
	lwarx 9,0,3
	add 9,9,4
	stwcx. 9,0,3
	bne- 0,.L218
	isync
	rldicl 3,9,0,32
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_AddFetch_uint32,.-SCOREP_Atomic_AddFetch_uint32
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_SubFetch_uint32
	.type	SCOREP_Atomic_SubFetch_uint32, @function
SCOREP_Atomic_SubFetch_uint32:
	sync
.L221:
	lwarx 9,0,3
	subf 9,4,9
	stwcx. 9,0,3
	bne- 0,.L221
	isync
	rldicl 3,9,0,32
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_SubFetch_uint32,.-SCOREP_Atomic_SubFetch_uint32
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_AndFetch_uint32
	.type	SCOREP_Atomic_AndFetch_uint32, @function
SCOREP_Atomic_AndFetch_uint32:
	sync
.L224:
	lwarx 9,0,3
	and 9,9,4
	stwcx. 9,0,3
	bne- 0,.L224
	isync
	rldicl 3,9,0,32
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_AndFetch_uint32,.-SCOREP_Atomic_AndFetch_uint32
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_XorFetch_uint32
	.type	SCOREP_Atomic_XorFetch_uint32, @function
SCOREP_Atomic_XorFetch_uint32:
	sync
.L227:
	lwarx 9,0,3
	xor 9,9,4
	stwcx. 9,0,3
	bne- 0,.L227
	isync
	rldicl 3,9,0,32
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_XorFetch_uint32,.-SCOREP_Atomic_XorFetch_uint32
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_OrFetch_uint32
	.type	SCOREP_Atomic_OrFetch_uint32, @function
SCOREP_Atomic_OrFetch_uint32:
	sync
.L230:
	lwarx 9,0,3
	or 9,9,4
	stwcx. 9,0,3
	bne- 0,.L230
	isync
	rldicl 3,9,0,32
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_OrFetch_uint32,.-SCOREP_Atomic_OrFetch_uint32
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_NandFetch_uint32
	.type	SCOREP_Atomic_NandFetch_uint32, @function
SCOREP_Atomic_NandFetch_uint32:
	sync
.L233:
	lwarx 9,0,3
	nand 9,9,4
	stwcx. 9,0,3
	bne- 0,.L233
	isync
	rldicl 3,9,0,32
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_NandFetch_uint32,.-SCOREP_Atomic_NandFetch_uint32
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchAdd_uint32
	.type	SCOREP_Atomic_FetchAdd_uint32, @function
SCOREP_Atomic_FetchAdd_uint32:
	sync
.L236:
	lwarx 9,0,3
	add 10,9,4
	stwcx. 10,0,3
	bne- 0,.L236
	isync
	rldicl 3,9,0,32
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchAdd_uint32,.-SCOREP_Atomic_FetchAdd_uint32
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchSub_uint32
	.type	SCOREP_Atomic_FetchSub_uint32, @function
SCOREP_Atomic_FetchSub_uint32:
	sync
.L239:
	lwarx 9,0,3
	subf 10,4,9
	stwcx. 10,0,3
	bne- 0,.L239
	isync
	rldicl 3,9,0,32
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchSub_uint32,.-SCOREP_Atomic_FetchSub_uint32
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchAnd_uint32
	.type	SCOREP_Atomic_FetchAnd_uint32, @function
SCOREP_Atomic_FetchAnd_uint32:
	sync
.L242:
	lwarx 9,0,3
	and 10,9,4
	stwcx. 10,0,3
	bne- 0,.L242
	isync
	rldicl 3,9,0,32
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchAnd_uint32,.-SCOREP_Atomic_FetchAnd_uint32
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchXor_uint32
	.type	SCOREP_Atomic_FetchXor_uint32, @function
SCOREP_Atomic_FetchXor_uint32:
	sync
.L245:
	lwarx 9,0,3
	xor 10,9,4
	stwcx. 10,0,3
	bne- 0,.L245
	isync
	rldicl 3,9,0,32
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchXor_uint32,.-SCOREP_Atomic_FetchXor_uint32
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchOr_uint32
	.type	SCOREP_Atomic_FetchOr_uint32, @function
SCOREP_Atomic_FetchOr_uint32:
	sync
.L248:
	lwarx 9,0,3
	or 10,9,4
	stwcx. 10,0,3
	bne- 0,.L248
	isync
	rldicl 3,9,0,32
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchOr_uint32,.-SCOREP_Atomic_FetchOr_uint32
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchNand_uint32
	.type	SCOREP_Atomic_FetchNand_uint32, @function
SCOREP_Atomic_FetchNand_uint32:
	sync
.L251:
	lwarx 9,0,3
	nand 10,9,4
	stwcx. 10,0,3
	bne- 0,.L251
	isync
	rldicl 3,9,0,32
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchNand_uint32,.-SCOREP_Atomic_FetchNand_uint32
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_LoadN_uint64
	.type	SCOREP_Atomic_LoadN_uint64, @function
SCOREP_Atomic_LoadN_uint64:
	sync
	ld 3,0(3)
	cmpw 7,3,3
	bne- 7,$+4
	isync
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_LoadN_uint64,.-SCOREP_Atomic_LoadN_uint64
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_StoreN_uint64
	.type	SCOREP_Atomic_StoreN_uint64, @function
SCOREP_Atomic_StoreN_uint64:
	sync
	std 4,0(3)
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_StoreN_uint64,.-SCOREP_Atomic_StoreN_uint64
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_ExchangeN_uint64
	.type	SCOREP_Atomic_ExchangeN_uint64, @function
SCOREP_Atomic_ExchangeN_uint64:
	sync
.L256:
	ldarx 9,0,3
	stdcx. 4,0,3
	bne- 0,.L256
	isync
	mr 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_ExchangeN_uint64,.-SCOREP_Atomic_ExchangeN_uint64
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_CompareExchangeN_uint64
	.type	SCOREP_Atomic_CompareExchangeN_uint64, @function
SCOREP_Atomic_CompareExchangeN_uint64:
	ld 10,0(4)
	sync
.L259:
	ldarx 9,0,3
	cmpd 0,9,10
	bne- 0,.L260
	stdcx. 5,0,3
	bne- 0,.L259
.L260:
	isync
	mfcr 3,128
	rlwinm 3,3,3,1
	bne 0,.L263
	rldicl 3,3,0,63
	blr
	.p2align 4,,15
.L263:
	std 9,0(4)
	rldicl 3,3,0,63
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_CompareExchangeN_uint64,.-SCOREP_Atomic_CompareExchangeN_uint64
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_AddFetch_uint64
	.type	SCOREP_Atomic_AddFetch_uint64, @function
SCOREP_Atomic_AddFetch_uint64:
	sync
	mr 9,3
.L265:
	ldarx 3,0,9
	add 3,3,4
	stdcx. 3,0,9
	bne- 0,.L265
	isync
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_AddFetch_uint64,.-SCOREP_Atomic_AddFetch_uint64
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_SubFetch_uint64
	.type	SCOREP_Atomic_SubFetch_uint64, @function
SCOREP_Atomic_SubFetch_uint64:
	sync
	mr 9,3
.L268:
	ldarx 3,0,9
	subf 3,4,3
	stdcx. 3,0,9
	bne- 0,.L268
	isync
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_SubFetch_uint64,.-SCOREP_Atomic_SubFetch_uint64
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_AndFetch_uint64
	.type	SCOREP_Atomic_AndFetch_uint64, @function
SCOREP_Atomic_AndFetch_uint64:
	sync
	mr 9,3
.L271:
	ldarx 3,0,9
	and 3,3,4
	stdcx. 3,0,9
	bne- 0,.L271
	isync
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_AndFetch_uint64,.-SCOREP_Atomic_AndFetch_uint64
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_XorFetch_uint64
	.type	SCOREP_Atomic_XorFetch_uint64, @function
SCOREP_Atomic_XorFetch_uint64:
	sync
	mr 9,3
.L274:
	ldarx 3,0,9
	xor 3,3,4
	stdcx. 3,0,9
	bne- 0,.L274
	isync
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_XorFetch_uint64,.-SCOREP_Atomic_XorFetch_uint64
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_OrFetch_uint64
	.type	SCOREP_Atomic_OrFetch_uint64, @function
SCOREP_Atomic_OrFetch_uint64:
	sync
	mr 9,3
.L277:
	ldarx 3,0,9
	or 3,3,4
	stdcx. 3,0,9
	bne- 0,.L277
	isync
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_OrFetch_uint64,.-SCOREP_Atomic_OrFetch_uint64
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_NandFetch_uint64
	.type	SCOREP_Atomic_NandFetch_uint64, @function
SCOREP_Atomic_NandFetch_uint64:
	sync
	mr 9,3
.L280:
	ldarx 3,0,9
	nand 3,3,4
	stdcx. 3,0,9
	bne- 0,.L280
	isync
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_NandFetch_uint64,.-SCOREP_Atomic_NandFetch_uint64
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchAdd_uint64
	.type	SCOREP_Atomic_FetchAdd_uint64, @function
SCOREP_Atomic_FetchAdd_uint64:
	sync
.L283:
	ldarx 9,0,3
	add 10,9,4
	stdcx. 10,0,3
	bne- 0,.L283
	isync
	mr 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchAdd_uint64,.-SCOREP_Atomic_FetchAdd_uint64
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchSub_uint64
	.type	SCOREP_Atomic_FetchSub_uint64, @function
SCOREP_Atomic_FetchSub_uint64:
	sync
.L286:
	ldarx 9,0,3
	subf 10,4,9
	stdcx. 10,0,3
	bne- 0,.L286
	isync
	mr 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchSub_uint64,.-SCOREP_Atomic_FetchSub_uint64
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchAnd_uint64
	.type	SCOREP_Atomic_FetchAnd_uint64, @function
SCOREP_Atomic_FetchAnd_uint64:
	sync
.L289:
	ldarx 9,0,3
	and 10,9,4
	stdcx. 10,0,3
	bne- 0,.L289
	isync
	mr 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchAnd_uint64,.-SCOREP_Atomic_FetchAnd_uint64
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchXor_uint64
	.type	SCOREP_Atomic_FetchXor_uint64, @function
SCOREP_Atomic_FetchXor_uint64:
	sync
.L292:
	ldarx 9,0,3
	xor 10,9,4
	stdcx. 10,0,3
	bne- 0,.L292
	isync
	mr 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchXor_uint64,.-SCOREP_Atomic_FetchXor_uint64
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchOr_uint64
	.type	SCOREP_Atomic_FetchOr_uint64, @function
SCOREP_Atomic_FetchOr_uint64:
	sync
.L295:
	ldarx 9,0,3
	or 10,9,4
	stdcx. 10,0,3
	bne- 0,.L295
	isync
	mr 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchOr_uint64,.-SCOREP_Atomic_FetchOr_uint64
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchNand_uint64
	.type	SCOREP_Atomic_FetchNand_uint64, @function
SCOREP_Atomic_FetchNand_uint64:
	sync
.L298:
	ldarx 9,0,3
	nand 10,9,4
	stdcx. 10,0,3
	bne- 0,.L298
	isync
	mr 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchNand_uint64,.-SCOREP_Atomic_FetchNand_uint64
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_LoadN_uintptr
	.type	SCOREP_Atomic_LoadN_uintptr, @function
SCOREP_Atomic_LoadN_uintptr:
	sync
	ld 3,0(3)
	cmpw 7,3,3
	bne- 7,$+4
	isync
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_LoadN_uintptr,.-SCOREP_Atomic_LoadN_uintptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_StoreN_uintptr
	.type	SCOREP_Atomic_StoreN_uintptr, @function
SCOREP_Atomic_StoreN_uintptr:
	sync
	std 4,0(3)
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_StoreN_uintptr,.-SCOREP_Atomic_StoreN_uintptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_ExchangeN_uintptr
	.type	SCOREP_Atomic_ExchangeN_uintptr, @function
SCOREP_Atomic_ExchangeN_uintptr:
	sync
.L303:
	ldarx 9,0,3
	stdcx. 4,0,3
	bne- 0,.L303
	isync
	mr 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_ExchangeN_uintptr,.-SCOREP_Atomic_ExchangeN_uintptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_CompareExchangeN_uintptr
	.type	SCOREP_Atomic_CompareExchangeN_uintptr, @function
SCOREP_Atomic_CompareExchangeN_uintptr:
	ld 10,0(4)
	sync
.L306:
	ldarx 9,0,3
	cmpd 0,9,10
	bne- 0,.L307
	stdcx. 5,0,3
	bne- 0,.L306
.L307:
	isync
	mfcr 3,128
	rlwinm 3,3,3,1
	bne 0,.L310
	rldicl 3,3,0,63
	blr
	.p2align 4,,15
.L310:
	std 9,0(4)
	rldicl 3,3,0,63
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_CompareExchangeN_uintptr,.-SCOREP_Atomic_CompareExchangeN_uintptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_AddFetch_uintptr
	.type	SCOREP_Atomic_AddFetch_uintptr, @function
SCOREP_Atomic_AddFetch_uintptr:
	sync
	mr 9,3
.L312:
	ldarx 3,0,9
	add 3,3,4
	stdcx. 3,0,9
	bne- 0,.L312
	isync
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_AddFetch_uintptr,.-SCOREP_Atomic_AddFetch_uintptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_SubFetch_uintptr
	.type	SCOREP_Atomic_SubFetch_uintptr, @function
SCOREP_Atomic_SubFetch_uintptr:
	sync
	mr 9,3
.L315:
	ldarx 3,0,9
	subf 3,4,3
	stdcx. 3,0,9
	bne- 0,.L315
	isync
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_SubFetch_uintptr,.-SCOREP_Atomic_SubFetch_uintptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_AndFetch_uintptr
	.type	SCOREP_Atomic_AndFetch_uintptr, @function
SCOREP_Atomic_AndFetch_uintptr:
	sync
	mr 9,3
.L318:
	ldarx 3,0,9
	and 3,3,4
	stdcx. 3,0,9
	bne- 0,.L318
	isync
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_AndFetch_uintptr,.-SCOREP_Atomic_AndFetch_uintptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_XorFetch_uintptr
	.type	SCOREP_Atomic_XorFetch_uintptr, @function
SCOREP_Atomic_XorFetch_uintptr:
	sync
	mr 9,3
.L321:
	ldarx 3,0,9
	xor 3,3,4
	stdcx. 3,0,9
	bne- 0,.L321
	isync
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_XorFetch_uintptr,.-SCOREP_Atomic_XorFetch_uintptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_OrFetch_uintptr
	.type	SCOREP_Atomic_OrFetch_uintptr, @function
SCOREP_Atomic_OrFetch_uintptr:
	sync
	mr 9,3
.L324:
	ldarx 3,0,9
	or 3,3,4
	stdcx. 3,0,9
	bne- 0,.L324
	isync
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_OrFetch_uintptr,.-SCOREP_Atomic_OrFetch_uintptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_NandFetch_uintptr
	.type	SCOREP_Atomic_NandFetch_uintptr, @function
SCOREP_Atomic_NandFetch_uintptr:
	sync
	mr 9,3
.L327:
	ldarx 3,0,9
	nand 3,3,4
	stdcx. 3,0,9
	bne- 0,.L327
	isync
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_NandFetch_uintptr,.-SCOREP_Atomic_NandFetch_uintptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchAdd_uintptr
	.type	SCOREP_Atomic_FetchAdd_uintptr, @function
SCOREP_Atomic_FetchAdd_uintptr:
	sync
.L330:
	ldarx 9,0,3
	add 10,9,4
	stdcx. 10,0,3
	bne- 0,.L330
	isync
	mr 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchAdd_uintptr,.-SCOREP_Atomic_FetchAdd_uintptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchSub_uintptr
	.type	SCOREP_Atomic_FetchSub_uintptr, @function
SCOREP_Atomic_FetchSub_uintptr:
	sync
.L333:
	ldarx 9,0,3
	subf 10,4,9
	stdcx. 10,0,3
	bne- 0,.L333
	isync
	mr 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchSub_uintptr,.-SCOREP_Atomic_FetchSub_uintptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchAnd_uintptr
	.type	SCOREP_Atomic_FetchAnd_uintptr, @function
SCOREP_Atomic_FetchAnd_uintptr:
	sync
.L336:
	ldarx 9,0,3
	and 10,9,4
	stdcx. 10,0,3
	bne- 0,.L336
	isync
	mr 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchAnd_uintptr,.-SCOREP_Atomic_FetchAnd_uintptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchXor_uintptr
	.type	SCOREP_Atomic_FetchXor_uintptr, @function
SCOREP_Atomic_FetchXor_uintptr:
	sync
.L339:
	ldarx 9,0,3
	xor 10,9,4
	stdcx. 10,0,3
	bne- 0,.L339
	isync
	mr 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchXor_uintptr,.-SCOREP_Atomic_FetchXor_uintptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchOr_uintptr
	.type	SCOREP_Atomic_FetchOr_uintptr, @function
SCOREP_Atomic_FetchOr_uintptr:
	sync
.L342:
	ldarx 9,0,3
	or 10,9,4
	stdcx. 10,0,3
	bne- 0,.L342
	isync
	mr 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchOr_uintptr,.-SCOREP_Atomic_FetchOr_uintptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchNand_uintptr
	.type	SCOREP_Atomic_FetchNand_uintptr, @function
SCOREP_Atomic_FetchNand_uintptr:
	sync
.L345:
	ldarx 9,0,3
	nand 10,9,4
	stdcx. 10,0,3
	bne- 0,.L345
	isync
	mr 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchNand_uintptr,.-SCOREP_Atomic_FetchNand_uintptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_LoadN_int8
	.type	SCOREP_Atomic_LoadN_int8, @function
SCOREP_Atomic_LoadN_int8:
	sync
	lbz 3,0(3)
	cmpw 7,3,3
	bne- 7,$+4
	isync
	extsb 3,3
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_LoadN_int8,.-SCOREP_Atomic_LoadN_int8
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_StoreN_int8
	.type	SCOREP_Atomic_StoreN_int8, @function
SCOREP_Atomic_StoreN_int8:
	sync
	stb 4,0(3)
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_StoreN_int8,.-SCOREP_Atomic_StoreN_int8
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_ExchangeN_int8
	.type	SCOREP_Atomic_ExchangeN_int8, @function
SCOREP_Atomic_ExchangeN_int8:
	sync
.L350:
	lbarx 9,0,3
	stbcx. 4,0,3
	bne- 0,.L350
	isync
	extsb 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_ExchangeN_int8,.-SCOREP_Atomic_ExchangeN_int8
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_CompareExchangeN_int8
	.type	SCOREP_Atomic_CompareExchangeN_int8, @function
SCOREP_Atomic_CompareExchangeN_int8:
	lbz 10,0(4)
	sync
.L353:
	lbarx 9,0,3
	cmpw 0,9,10
	bne- 0,.L354
	stbcx. 5,0,3
	bne- 0,.L353
.L354:
	isync
	mfcr 3,128
	rlwinm 3,3,3,1
	bne 0,.L357
	rldicl 3,3,0,63
	blr
	.p2align 4,,15
.L357:
	stb 9,0(4)
	rldicl 3,3,0,63
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_CompareExchangeN_int8,.-SCOREP_Atomic_CompareExchangeN_int8
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_AddFetch_int8
	.type	SCOREP_Atomic_AddFetch_int8, @function
SCOREP_Atomic_AddFetch_int8:
	sync
	rlwinm 4,4,0,0xff
.L359:
	lbarx 9,0,3
	add 9,9,4
	stbcx. 9,0,3
	bne- 0,.L359
	isync
	extsb 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_AddFetch_int8,.-SCOREP_Atomic_AddFetch_int8
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_SubFetch_int8
	.type	SCOREP_Atomic_SubFetch_int8, @function
SCOREP_Atomic_SubFetch_int8:
	sync
	rlwinm 4,4,0,0xff
.L362:
	lbarx 9,0,3
	subf 9,4,9
	stbcx. 9,0,3
	bne- 0,.L362
	isync
	extsb 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_SubFetch_int8,.-SCOREP_Atomic_SubFetch_int8
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_AndFetch_int8
	.type	SCOREP_Atomic_AndFetch_int8, @function
SCOREP_Atomic_AndFetch_int8:
	sync
	rlwinm 4,4,0,0xff
.L365:
	lbarx 9,0,3
	and 9,9,4
	stbcx. 9,0,3
	bne- 0,.L365
	isync
	extsb 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_AndFetch_int8,.-SCOREP_Atomic_AndFetch_int8
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_XorFetch_int8
	.type	SCOREP_Atomic_XorFetch_int8, @function
SCOREP_Atomic_XorFetch_int8:
	sync
	rlwinm 4,4,0,0xff
.L368:
	lbarx 9,0,3
	xor 9,9,4
	stbcx. 9,0,3
	bne- 0,.L368
	isync
	extsb 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_XorFetch_int8,.-SCOREP_Atomic_XorFetch_int8
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_OrFetch_int8
	.type	SCOREP_Atomic_OrFetch_int8, @function
SCOREP_Atomic_OrFetch_int8:
	sync
	rlwinm 4,4,0,0xff
.L371:
	lbarx 9,0,3
	or 9,9,4
	stbcx. 9,0,3
	bne- 0,.L371
	isync
	extsb 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_OrFetch_int8,.-SCOREP_Atomic_OrFetch_int8
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_NandFetch_int8
	.type	SCOREP_Atomic_NandFetch_int8, @function
SCOREP_Atomic_NandFetch_int8:
	sync
	rlwinm 4,4,0,0xff
.L374:
	lbarx 9,0,3
	nand 9,9,4
	stbcx. 9,0,3
	bne- 0,.L374
	isync
	extsb 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_NandFetch_int8,.-SCOREP_Atomic_NandFetch_int8
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchAdd_int8
	.type	SCOREP_Atomic_FetchAdd_int8, @function
SCOREP_Atomic_FetchAdd_int8:
	sync
	rlwinm 4,4,0,0xff
.L377:
	lbarx 9,0,3
	add 10,9,4
	stbcx. 10,0,3
	bne- 0,.L377
	isync
	extsb 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchAdd_int8,.-SCOREP_Atomic_FetchAdd_int8
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchSub_int8
	.type	SCOREP_Atomic_FetchSub_int8, @function
SCOREP_Atomic_FetchSub_int8:
	sync
	rlwinm 4,4,0,0xff
.L380:
	lbarx 9,0,3
	subf 10,4,9
	stbcx. 10,0,3
	bne- 0,.L380
	isync
	extsb 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchSub_int8,.-SCOREP_Atomic_FetchSub_int8
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchAnd_int8
	.type	SCOREP_Atomic_FetchAnd_int8, @function
SCOREP_Atomic_FetchAnd_int8:
	sync
	rlwinm 4,4,0,0xff
.L383:
	lbarx 9,0,3
	and 10,9,4
	stbcx. 10,0,3
	bne- 0,.L383
	isync
	extsb 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchAnd_int8,.-SCOREP_Atomic_FetchAnd_int8
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchXor_int8
	.type	SCOREP_Atomic_FetchXor_int8, @function
SCOREP_Atomic_FetchXor_int8:
	sync
	rlwinm 4,4,0,0xff
.L386:
	lbarx 9,0,3
	xor 10,9,4
	stbcx. 10,0,3
	bne- 0,.L386
	isync
	extsb 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchXor_int8,.-SCOREP_Atomic_FetchXor_int8
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchOr_int8
	.type	SCOREP_Atomic_FetchOr_int8, @function
SCOREP_Atomic_FetchOr_int8:
	sync
	rlwinm 4,4,0,0xff
.L389:
	lbarx 9,0,3
	or 10,9,4
	stbcx. 10,0,3
	bne- 0,.L389
	isync
	extsb 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchOr_int8,.-SCOREP_Atomic_FetchOr_int8
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchNand_int8
	.type	SCOREP_Atomic_FetchNand_int8, @function
SCOREP_Atomic_FetchNand_int8:
	sync
	rlwinm 4,4,0,0xff
.L392:
	lbarx 9,0,3
	nand 10,9,4
	stbcx. 10,0,3
	bne- 0,.L392
	isync
	extsb 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchNand_int8,.-SCOREP_Atomic_FetchNand_int8
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_LoadN_int16
	.type	SCOREP_Atomic_LoadN_int16, @function
SCOREP_Atomic_LoadN_int16:
	sync
	lhz 3,0(3)
	cmpw 7,3,3
	bne- 7,$+4
	isync
	extsh 3,3
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_LoadN_int16,.-SCOREP_Atomic_LoadN_int16
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_StoreN_int16
	.type	SCOREP_Atomic_StoreN_int16, @function
SCOREP_Atomic_StoreN_int16:
	sync
	sth 4,0(3)
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_StoreN_int16,.-SCOREP_Atomic_StoreN_int16
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_ExchangeN_int16
	.type	SCOREP_Atomic_ExchangeN_int16, @function
SCOREP_Atomic_ExchangeN_int16:
	sync
.L397:
	lharx 9,0,3
	sthcx. 4,0,3
	bne- 0,.L397
	isync
	extsh 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_ExchangeN_int16,.-SCOREP_Atomic_ExchangeN_int16
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_CompareExchangeN_int16
	.type	SCOREP_Atomic_CompareExchangeN_int16, @function
SCOREP_Atomic_CompareExchangeN_int16:
	lhz 10,0(4)
	sync
.L400:
	lharx 9,0,3
	cmpw 0,9,10
	bne- 0,.L401
	sthcx. 5,0,3
	bne- 0,.L400
.L401:
	isync
	mfcr 3,128
	rlwinm 3,3,3,1
	bne 0,.L404
	rldicl 3,3,0,63
	blr
	.p2align 4,,15
.L404:
	sth 9,0(4)
	rldicl 3,3,0,63
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_CompareExchangeN_int16,.-SCOREP_Atomic_CompareExchangeN_int16
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_AddFetch_int16
	.type	SCOREP_Atomic_AddFetch_int16, @function
SCOREP_Atomic_AddFetch_int16:
	sync
	rlwinm 4,4,0,0xffff
.L406:
	lharx 9,0,3
	add 9,9,4
	sthcx. 9,0,3
	bne- 0,.L406
	isync
	extsh 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_AddFetch_int16,.-SCOREP_Atomic_AddFetch_int16
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_SubFetch_int16
	.type	SCOREP_Atomic_SubFetch_int16, @function
SCOREP_Atomic_SubFetch_int16:
	sync
	rlwinm 4,4,0,0xffff
.L409:
	lharx 9,0,3
	subf 9,4,9
	sthcx. 9,0,3
	bne- 0,.L409
	isync
	extsh 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_SubFetch_int16,.-SCOREP_Atomic_SubFetch_int16
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_AndFetch_int16
	.type	SCOREP_Atomic_AndFetch_int16, @function
SCOREP_Atomic_AndFetch_int16:
	sync
	rlwinm 4,4,0,0xffff
.L412:
	lharx 9,0,3
	and 9,9,4
	sthcx. 9,0,3
	bne- 0,.L412
	isync
	extsh 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_AndFetch_int16,.-SCOREP_Atomic_AndFetch_int16
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_XorFetch_int16
	.type	SCOREP_Atomic_XorFetch_int16, @function
SCOREP_Atomic_XorFetch_int16:
	sync
	rlwinm 4,4,0,0xffff
.L415:
	lharx 9,0,3
	xor 9,9,4
	sthcx. 9,0,3
	bne- 0,.L415
	isync
	extsh 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_XorFetch_int16,.-SCOREP_Atomic_XorFetch_int16
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_OrFetch_int16
	.type	SCOREP_Atomic_OrFetch_int16, @function
SCOREP_Atomic_OrFetch_int16:
	sync
	rlwinm 4,4,0,0xffff
.L418:
	lharx 9,0,3
	or 9,9,4
	sthcx. 9,0,3
	bne- 0,.L418
	isync
	extsh 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_OrFetch_int16,.-SCOREP_Atomic_OrFetch_int16
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_NandFetch_int16
	.type	SCOREP_Atomic_NandFetch_int16, @function
SCOREP_Atomic_NandFetch_int16:
	sync
	rlwinm 4,4,0,0xffff
.L421:
	lharx 9,0,3
	nand 9,9,4
	sthcx. 9,0,3
	bne- 0,.L421
	isync
	extsh 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_NandFetch_int16,.-SCOREP_Atomic_NandFetch_int16
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchAdd_int16
	.type	SCOREP_Atomic_FetchAdd_int16, @function
SCOREP_Atomic_FetchAdd_int16:
	sync
	rlwinm 4,4,0,0xffff
.L424:
	lharx 9,0,3
	add 10,9,4
	sthcx. 10,0,3
	bne- 0,.L424
	isync
	extsh 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchAdd_int16,.-SCOREP_Atomic_FetchAdd_int16
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchSub_int16
	.type	SCOREP_Atomic_FetchSub_int16, @function
SCOREP_Atomic_FetchSub_int16:
	sync
	rlwinm 4,4,0,0xffff
.L427:
	lharx 9,0,3
	subf 10,4,9
	sthcx. 10,0,3
	bne- 0,.L427
	isync
	extsh 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchSub_int16,.-SCOREP_Atomic_FetchSub_int16
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchAnd_int16
	.type	SCOREP_Atomic_FetchAnd_int16, @function
SCOREP_Atomic_FetchAnd_int16:
	sync
	rlwinm 4,4,0,0xffff
.L430:
	lharx 9,0,3
	and 10,9,4
	sthcx. 10,0,3
	bne- 0,.L430
	isync
	extsh 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchAnd_int16,.-SCOREP_Atomic_FetchAnd_int16
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchXor_int16
	.type	SCOREP_Atomic_FetchXor_int16, @function
SCOREP_Atomic_FetchXor_int16:
	sync
	rlwinm 4,4,0,0xffff
.L433:
	lharx 9,0,3
	xor 10,9,4
	sthcx. 10,0,3
	bne- 0,.L433
	isync
	extsh 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchXor_int16,.-SCOREP_Atomic_FetchXor_int16
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchOr_int16
	.type	SCOREP_Atomic_FetchOr_int16, @function
SCOREP_Atomic_FetchOr_int16:
	sync
	rlwinm 4,4,0,0xffff
.L436:
	lharx 9,0,3
	or 10,9,4
	sthcx. 10,0,3
	bne- 0,.L436
	isync
	extsh 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchOr_int16,.-SCOREP_Atomic_FetchOr_int16
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchNand_int16
	.type	SCOREP_Atomic_FetchNand_int16, @function
SCOREP_Atomic_FetchNand_int16:
	sync
	rlwinm 4,4,0,0xffff
.L439:
	lharx 9,0,3
	nand 10,9,4
	sthcx. 10,0,3
	bne- 0,.L439
	isync
	extsh 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchNand_int16,.-SCOREP_Atomic_FetchNand_int16
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_LoadN_int32
	.type	SCOREP_Atomic_LoadN_int32, @function
SCOREP_Atomic_LoadN_int32:
	sync
	lwz 3,0(3)
	cmpw 7,3,3
	bne- 7,$+4
	isync
	extsw 3,3
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_LoadN_int32,.-SCOREP_Atomic_LoadN_int32
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_StoreN_int32
	.type	SCOREP_Atomic_StoreN_int32, @function
SCOREP_Atomic_StoreN_int32:
	sync
	stw 4,0(3)
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_StoreN_int32,.-SCOREP_Atomic_StoreN_int32
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_ExchangeN_int32
	.type	SCOREP_Atomic_ExchangeN_int32, @function
SCOREP_Atomic_ExchangeN_int32:
	sync
.L444:
	lwarx 9,0,3
	stwcx. 4,0,3
	bne- 0,.L444
	isync
	extsw 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_ExchangeN_int32,.-SCOREP_Atomic_ExchangeN_int32
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_CompareExchangeN_int32
	.type	SCOREP_Atomic_CompareExchangeN_int32, @function
SCOREP_Atomic_CompareExchangeN_int32:
	lwz 10,0(4)
	sync
.L447:
	lwarx 9,0,3
	cmpw 0,9,10
	bne- 0,.L448
	stwcx. 5,0,3
	bne- 0,.L447
.L448:
	isync
	mfcr 3,128
	rlwinm 3,3,3,1
	bne 0,.L451
	rldicl 3,3,0,63
	blr
	.p2align 4,,15
.L451:
	stw 9,0(4)
	rldicl 3,3,0,63
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_CompareExchangeN_int32,.-SCOREP_Atomic_CompareExchangeN_int32
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_AddFetch_int32
	.type	SCOREP_Atomic_AddFetch_int32, @function
SCOREP_Atomic_AddFetch_int32:
	sync
.L453:
	lwarx 9,0,3
	add 9,9,4
	stwcx. 9,0,3
	bne- 0,.L453
	isync
	extsw 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_AddFetch_int32,.-SCOREP_Atomic_AddFetch_int32
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_SubFetch_int32
	.type	SCOREP_Atomic_SubFetch_int32, @function
SCOREP_Atomic_SubFetch_int32:
	sync
.L456:
	lwarx 9,0,3
	subf 9,4,9
	stwcx. 9,0,3
	bne- 0,.L456
	isync
	extsw 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_SubFetch_int32,.-SCOREP_Atomic_SubFetch_int32
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_AndFetch_int32
	.type	SCOREP_Atomic_AndFetch_int32, @function
SCOREP_Atomic_AndFetch_int32:
	sync
.L459:
	lwarx 9,0,3
	and 9,9,4
	stwcx. 9,0,3
	bne- 0,.L459
	isync
	extsw 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_AndFetch_int32,.-SCOREP_Atomic_AndFetch_int32
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_XorFetch_int32
	.type	SCOREP_Atomic_XorFetch_int32, @function
SCOREP_Atomic_XorFetch_int32:
	sync
.L462:
	lwarx 9,0,3
	xor 9,9,4
	stwcx. 9,0,3
	bne- 0,.L462
	isync
	extsw 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_XorFetch_int32,.-SCOREP_Atomic_XorFetch_int32
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_OrFetch_int32
	.type	SCOREP_Atomic_OrFetch_int32, @function
SCOREP_Atomic_OrFetch_int32:
	sync
.L465:
	lwarx 9,0,3
	or 9,9,4
	stwcx. 9,0,3
	bne- 0,.L465
	isync
	extsw 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_OrFetch_int32,.-SCOREP_Atomic_OrFetch_int32
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_NandFetch_int32
	.type	SCOREP_Atomic_NandFetch_int32, @function
SCOREP_Atomic_NandFetch_int32:
	sync
.L468:
	lwarx 9,0,3
	nand 9,9,4
	stwcx. 9,0,3
	bne- 0,.L468
	isync
	extsw 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_NandFetch_int32,.-SCOREP_Atomic_NandFetch_int32
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchAdd_int32
	.type	SCOREP_Atomic_FetchAdd_int32, @function
SCOREP_Atomic_FetchAdd_int32:
	sync
.L471:
	lwarx 9,0,3
	add 10,9,4
	stwcx. 10,0,3
	bne- 0,.L471
	isync
	extsw 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchAdd_int32,.-SCOREP_Atomic_FetchAdd_int32
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchSub_int32
	.type	SCOREP_Atomic_FetchSub_int32, @function
SCOREP_Atomic_FetchSub_int32:
	sync
.L474:
	lwarx 9,0,3
	subf 10,4,9
	stwcx. 10,0,3
	bne- 0,.L474
	isync
	extsw 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchSub_int32,.-SCOREP_Atomic_FetchSub_int32
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchAnd_int32
	.type	SCOREP_Atomic_FetchAnd_int32, @function
SCOREP_Atomic_FetchAnd_int32:
	sync
.L477:
	lwarx 9,0,3
	and 10,9,4
	stwcx. 10,0,3
	bne- 0,.L477
	isync
	extsw 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchAnd_int32,.-SCOREP_Atomic_FetchAnd_int32
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchXor_int32
	.type	SCOREP_Atomic_FetchXor_int32, @function
SCOREP_Atomic_FetchXor_int32:
	sync
.L480:
	lwarx 9,0,3
	xor 10,9,4
	stwcx. 10,0,3
	bne- 0,.L480
	isync
	extsw 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchXor_int32,.-SCOREP_Atomic_FetchXor_int32
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchOr_int32
	.type	SCOREP_Atomic_FetchOr_int32, @function
SCOREP_Atomic_FetchOr_int32:
	sync
.L483:
	lwarx 9,0,3
	or 10,9,4
	stwcx. 10,0,3
	bne- 0,.L483
	isync
	extsw 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchOr_int32,.-SCOREP_Atomic_FetchOr_int32
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchNand_int32
	.type	SCOREP_Atomic_FetchNand_int32, @function
SCOREP_Atomic_FetchNand_int32:
	sync
.L486:
	lwarx 9,0,3
	nand 10,9,4
	stwcx. 10,0,3
	bne- 0,.L486
	isync
	extsw 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchNand_int32,.-SCOREP_Atomic_FetchNand_int32
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_LoadN_int64
	.type	SCOREP_Atomic_LoadN_int64, @function
SCOREP_Atomic_LoadN_int64:
	sync
	ld 3,0(3)
	cmpw 7,3,3
	bne- 7,$+4
	isync
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_LoadN_int64,.-SCOREP_Atomic_LoadN_int64
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_StoreN_int64
	.type	SCOREP_Atomic_StoreN_int64, @function
SCOREP_Atomic_StoreN_int64:
	sync
	std 4,0(3)
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_StoreN_int64,.-SCOREP_Atomic_StoreN_int64
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_ExchangeN_int64
	.type	SCOREP_Atomic_ExchangeN_int64, @function
SCOREP_Atomic_ExchangeN_int64:
	sync
.L491:
	ldarx 9,0,3
	stdcx. 4,0,3
	bne- 0,.L491
	isync
	mr 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_ExchangeN_int64,.-SCOREP_Atomic_ExchangeN_int64
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_CompareExchangeN_int64
	.type	SCOREP_Atomic_CompareExchangeN_int64, @function
SCOREP_Atomic_CompareExchangeN_int64:
	ld 10,0(4)
	sync
.L494:
	ldarx 9,0,3
	cmpd 0,9,10
	bne- 0,.L495
	stdcx. 5,0,3
	bne- 0,.L494
.L495:
	isync
	mfcr 3,128
	rlwinm 3,3,3,1
	bne 0,.L498
	rldicl 3,3,0,63
	blr
	.p2align 4,,15
.L498:
	std 9,0(4)
	rldicl 3,3,0,63
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_CompareExchangeN_int64,.-SCOREP_Atomic_CompareExchangeN_int64
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_AddFetch_int64
	.type	SCOREP_Atomic_AddFetch_int64, @function
SCOREP_Atomic_AddFetch_int64:
	sync
	mr 9,3
.L500:
	ldarx 3,0,9
	add 3,3,4
	stdcx. 3,0,9
	bne- 0,.L500
	isync
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_AddFetch_int64,.-SCOREP_Atomic_AddFetch_int64
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_SubFetch_int64
	.type	SCOREP_Atomic_SubFetch_int64, @function
SCOREP_Atomic_SubFetch_int64:
	sync
	mr 9,3
.L503:
	ldarx 3,0,9
	subf 3,4,3
	stdcx. 3,0,9
	bne- 0,.L503
	isync
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_SubFetch_int64,.-SCOREP_Atomic_SubFetch_int64
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_AndFetch_int64
	.type	SCOREP_Atomic_AndFetch_int64, @function
SCOREP_Atomic_AndFetch_int64:
	sync
	mr 9,3
.L506:
	ldarx 3,0,9
	and 3,3,4
	stdcx. 3,0,9
	bne- 0,.L506
	isync
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_AndFetch_int64,.-SCOREP_Atomic_AndFetch_int64
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_XorFetch_int64
	.type	SCOREP_Atomic_XorFetch_int64, @function
SCOREP_Atomic_XorFetch_int64:
	sync
	mr 9,3
.L509:
	ldarx 3,0,9
	xor 3,3,4
	stdcx. 3,0,9
	bne- 0,.L509
	isync
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_XorFetch_int64,.-SCOREP_Atomic_XorFetch_int64
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_OrFetch_int64
	.type	SCOREP_Atomic_OrFetch_int64, @function
SCOREP_Atomic_OrFetch_int64:
	sync
	mr 9,3
.L512:
	ldarx 3,0,9
	or 3,3,4
	stdcx. 3,0,9
	bne- 0,.L512
	isync
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_OrFetch_int64,.-SCOREP_Atomic_OrFetch_int64
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_NandFetch_int64
	.type	SCOREP_Atomic_NandFetch_int64, @function
SCOREP_Atomic_NandFetch_int64:
	sync
	mr 9,3
.L515:
	ldarx 3,0,9
	nand 3,3,4
	stdcx. 3,0,9
	bne- 0,.L515
	isync
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_NandFetch_int64,.-SCOREP_Atomic_NandFetch_int64
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchAdd_int64
	.type	SCOREP_Atomic_FetchAdd_int64, @function
SCOREP_Atomic_FetchAdd_int64:
	sync
.L518:
	ldarx 9,0,3
	add 10,9,4
	stdcx. 10,0,3
	bne- 0,.L518
	isync
	mr 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchAdd_int64,.-SCOREP_Atomic_FetchAdd_int64
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchSub_int64
	.type	SCOREP_Atomic_FetchSub_int64, @function
SCOREP_Atomic_FetchSub_int64:
	sync
.L521:
	ldarx 9,0,3
	subf 10,4,9
	stdcx. 10,0,3
	bne- 0,.L521
	isync
	mr 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchSub_int64,.-SCOREP_Atomic_FetchSub_int64
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchAnd_int64
	.type	SCOREP_Atomic_FetchAnd_int64, @function
SCOREP_Atomic_FetchAnd_int64:
	sync
.L524:
	ldarx 9,0,3
	and 10,9,4
	stdcx. 10,0,3
	bne- 0,.L524
	isync
	mr 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchAnd_int64,.-SCOREP_Atomic_FetchAnd_int64
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchXor_int64
	.type	SCOREP_Atomic_FetchXor_int64, @function
SCOREP_Atomic_FetchXor_int64:
	sync
.L527:
	ldarx 9,0,3
	xor 10,9,4
	stdcx. 10,0,3
	bne- 0,.L527
	isync
	mr 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchXor_int64,.-SCOREP_Atomic_FetchXor_int64
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchOr_int64
	.type	SCOREP_Atomic_FetchOr_int64, @function
SCOREP_Atomic_FetchOr_int64:
	sync
.L530:
	ldarx 9,0,3
	or 10,9,4
	stdcx. 10,0,3
	bne- 0,.L530
	isync
	mr 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchOr_int64,.-SCOREP_Atomic_FetchOr_int64
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchNand_int64
	.type	SCOREP_Atomic_FetchNand_int64, @function
SCOREP_Atomic_FetchNand_int64:
	sync
.L533:
	ldarx 9,0,3
	nand 10,9,4
	stdcx. 10,0,3
	bne- 0,.L533
	isync
	mr 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchNand_int64,.-SCOREP_Atomic_FetchNand_int64
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_LoadN_intptr
	.type	SCOREP_Atomic_LoadN_intptr, @function
SCOREP_Atomic_LoadN_intptr:
	sync
	ld 3,0(3)
	cmpw 7,3,3
	bne- 7,$+4
	isync
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_LoadN_intptr,.-SCOREP_Atomic_LoadN_intptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_StoreN_intptr
	.type	SCOREP_Atomic_StoreN_intptr, @function
SCOREP_Atomic_StoreN_intptr:
	sync
	std 4,0(3)
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_StoreN_intptr,.-SCOREP_Atomic_StoreN_intptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_ExchangeN_intptr
	.type	SCOREP_Atomic_ExchangeN_intptr, @function
SCOREP_Atomic_ExchangeN_intptr:
	sync
.L538:
	ldarx 9,0,3
	stdcx. 4,0,3
	bne- 0,.L538
	isync
	mr 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_ExchangeN_intptr,.-SCOREP_Atomic_ExchangeN_intptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_CompareExchangeN_intptr
	.type	SCOREP_Atomic_CompareExchangeN_intptr, @function
SCOREP_Atomic_CompareExchangeN_intptr:
	ld 10,0(4)
	sync
.L541:
	ldarx 9,0,3
	cmpd 0,9,10
	bne- 0,.L542
	stdcx. 5,0,3
	bne- 0,.L541
.L542:
	isync
	mfcr 3,128
	rlwinm 3,3,3,1
	bne 0,.L545
	rldicl 3,3,0,63
	blr
	.p2align 4,,15
.L545:
	std 9,0(4)
	rldicl 3,3,0,63
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_CompareExchangeN_intptr,.-SCOREP_Atomic_CompareExchangeN_intptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_AddFetch_intptr
	.type	SCOREP_Atomic_AddFetch_intptr, @function
SCOREP_Atomic_AddFetch_intptr:
	sync
	mr 9,3
.L547:
	ldarx 3,0,9
	add 3,3,4
	stdcx. 3,0,9
	bne- 0,.L547
	isync
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_AddFetch_intptr,.-SCOREP_Atomic_AddFetch_intptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_SubFetch_intptr
	.type	SCOREP_Atomic_SubFetch_intptr, @function
SCOREP_Atomic_SubFetch_intptr:
	sync
	mr 9,3
.L550:
	ldarx 3,0,9
	subf 3,4,3
	stdcx. 3,0,9
	bne- 0,.L550
	isync
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_SubFetch_intptr,.-SCOREP_Atomic_SubFetch_intptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_AndFetch_intptr
	.type	SCOREP_Atomic_AndFetch_intptr, @function
SCOREP_Atomic_AndFetch_intptr:
	sync
	mr 9,3
.L553:
	ldarx 3,0,9
	and 3,3,4
	stdcx. 3,0,9
	bne- 0,.L553
	isync
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_AndFetch_intptr,.-SCOREP_Atomic_AndFetch_intptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_XorFetch_intptr
	.type	SCOREP_Atomic_XorFetch_intptr, @function
SCOREP_Atomic_XorFetch_intptr:
	sync
	mr 9,3
.L556:
	ldarx 3,0,9
	xor 3,3,4
	stdcx. 3,0,9
	bne- 0,.L556
	isync
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_XorFetch_intptr,.-SCOREP_Atomic_XorFetch_intptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_OrFetch_intptr
	.type	SCOREP_Atomic_OrFetch_intptr, @function
SCOREP_Atomic_OrFetch_intptr:
	sync
	mr 9,3
.L559:
	ldarx 3,0,9
	or 3,3,4
	stdcx. 3,0,9
	bne- 0,.L559
	isync
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_OrFetch_intptr,.-SCOREP_Atomic_OrFetch_intptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_NandFetch_intptr
	.type	SCOREP_Atomic_NandFetch_intptr, @function
SCOREP_Atomic_NandFetch_intptr:
	sync
	mr 9,3
.L562:
	ldarx 3,0,9
	nand 3,3,4
	stdcx. 3,0,9
	bne- 0,.L562
	isync
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_NandFetch_intptr,.-SCOREP_Atomic_NandFetch_intptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchAdd_intptr
	.type	SCOREP_Atomic_FetchAdd_intptr, @function
SCOREP_Atomic_FetchAdd_intptr:
	sync
.L565:
	ldarx 9,0,3
	add 10,9,4
	stdcx. 10,0,3
	bne- 0,.L565
	isync
	mr 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchAdd_intptr,.-SCOREP_Atomic_FetchAdd_intptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchSub_intptr
	.type	SCOREP_Atomic_FetchSub_intptr, @function
SCOREP_Atomic_FetchSub_intptr:
	sync
.L568:
	ldarx 9,0,3
	subf 10,4,9
	stdcx. 10,0,3
	bne- 0,.L568
	isync
	mr 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchSub_intptr,.-SCOREP_Atomic_FetchSub_intptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchAnd_intptr
	.type	SCOREP_Atomic_FetchAnd_intptr, @function
SCOREP_Atomic_FetchAnd_intptr:
	sync
.L571:
	ldarx 9,0,3
	and 10,9,4
	stdcx. 10,0,3
	bne- 0,.L571
	isync
	mr 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchAnd_intptr,.-SCOREP_Atomic_FetchAnd_intptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchXor_intptr
	.type	SCOREP_Atomic_FetchXor_intptr, @function
SCOREP_Atomic_FetchXor_intptr:
	sync
.L574:
	ldarx 9,0,3
	xor 10,9,4
	stdcx. 10,0,3
	bne- 0,.L574
	isync
	mr 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchXor_intptr,.-SCOREP_Atomic_FetchXor_intptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchOr_intptr
	.type	SCOREP_Atomic_FetchOr_intptr, @function
SCOREP_Atomic_FetchOr_intptr:
	sync
.L577:
	ldarx 9,0,3
	or 10,9,4
	stdcx. 10,0,3
	bne- 0,.L577
	isync
	mr 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchOr_intptr,.-SCOREP_Atomic_FetchOr_intptr
	.align 2
	.p2align 4,,15
	.globl SCOREP_Atomic_FetchNand_intptr
	.type	SCOREP_Atomic_FetchNand_intptr, @function
SCOREP_Atomic_FetchNand_intptr:
	sync
.L580:
	ldarx 9,0,3
	nand 10,9,4
	stdcx. 10,0,3
	bne- 0,.L580
	isync
	mr 3,9
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	SCOREP_Atomic_FetchNand_intptr,.-SCOREP_Atomic_FetchNand_intptr
	.ident	"GCC: (GNU) 4.8.5 20150623 (Red Hat 4.8.5-36)"
	.section	.note.GNU-stack,"",@progbits
