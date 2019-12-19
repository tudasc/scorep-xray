	.arch armv8-a
	.file	"SCOREP_Atomic.inc.c"
	.text
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_TestAndSet
	.type	SCOREP_Atomic_TestAndSet, %function
SCOREP_Atomic_TestAndSet:
	mov	w2, 1
.L2:
	ldaxrb	w1, [x0]
	stlxrb	w3, w2, [x0]
	cbnz	w3, .L2
	mov	w0, w1
	ret
	.size	SCOREP_Atomic_TestAndSet, .-SCOREP_Atomic_TestAndSet
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_clear
	.type	SCOREP_Atomic_clear, %function
SCOREP_Atomic_clear:
	stlrb	wzr, [x0]
	ret
	.size	SCOREP_Atomic_clear, .-SCOREP_Atomic_clear
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_ThreadFence
	.type	SCOREP_Atomic_ThreadFence, %function
SCOREP_Atomic_ThreadFence:
	dmb	ish
	ret
	.size	SCOREP_Atomic_ThreadFence, .-SCOREP_Atomic_ThreadFence
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_LoadN_void_ptr
	.type	SCOREP_Atomic_LoadN_void_ptr, %function
SCOREP_Atomic_LoadN_void_ptr:
	ldar	x0, [x0]
	ret
	.size	SCOREP_Atomic_LoadN_void_ptr, .-SCOREP_Atomic_LoadN_void_ptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_StoreN_void_ptr
	.type	SCOREP_Atomic_StoreN_void_ptr, %function
SCOREP_Atomic_StoreN_void_ptr:
	stlr	x1, [x0]
	ret
	.size	SCOREP_Atomic_StoreN_void_ptr, .-SCOREP_Atomic_StoreN_void_ptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_ExchangeN_void_ptr
	.type	SCOREP_Atomic_ExchangeN_void_ptr, %function
SCOREP_Atomic_ExchangeN_void_ptr:
.L9:
	ldaxr	x2, [x0]
	stlxr	w3, x1, [x0]
	cbnz	w3, .L9
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_ExchangeN_void_ptr, .-SCOREP_Atomic_ExchangeN_void_ptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_CompareExchangeN_void_ptr
	.type	SCOREP_Atomic_CompareExchangeN_void_ptr, %function
SCOREP_Atomic_CompareExchangeN_void_ptr:
	ldr	x3, [x1]
.L12:
	ldaxr	x4, [x0]
	cmp	x4, x3
	bne	.L13
	stlxr	w5, x2, [x0]
	cbnz	w5, .L12
.L13:
	cset	w0, eq
	cbz	w0, .L14
	ret
	.p2align 3
.L14:
	str	x4, [x1]
	ret
	.size	SCOREP_Atomic_CompareExchangeN_void_ptr, .-SCOREP_Atomic_CompareExchangeN_void_ptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_AddFetch_void_ptr
	.type	SCOREP_Atomic_AddFetch_void_ptr, %function
SCOREP_Atomic_AddFetch_void_ptr:
.L16:
	ldaxr	x2, [x0]
	add	x2, x2, x1
	stlxr	w3, x2, [x0]
	cbnz	w3, .L16
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_AddFetch_void_ptr, .-SCOREP_Atomic_AddFetch_void_ptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_SubFetch_void_ptr
	.type	SCOREP_Atomic_SubFetch_void_ptr, %function
SCOREP_Atomic_SubFetch_void_ptr:
.L18:
	ldaxr	x2, [x0]
	sub	x2, x2, x1
	stlxr	w3, x2, [x0]
	cbnz	w3, .L18
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_SubFetch_void_ptr, .-SCOREP_Atomic_SubFetch_void_ptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_AndFetch_void_ptr
	.type	SCOREP_Atomic_AndFetch_void_ptr, %function
SCOREP_Atomic_AndFetch_void_ptr:
.L20:
	ldaxr	x2, [x0]
	and	x2, x2, x1
	stlxr	w3, x2, [x0]
	cbnz	w3, .L20
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_AndFetch_void_ptr, .-SCOREP_Atomic_AndFetch_void_ptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_XorFetch_void_ptr
	.type	SCOREP_Atomic_XorFetch_void_ptr, %function
SCOREP_Atomic_XorFetch_void_ptr:
.L22:
	ldaxr	x2, [x0]
	eor	x2, x2, x1
	stlxr	w3, x2, [x0]
	cbnz	w3, .L22
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_XorFetch_void_ptr, .-SCOREP_Atomic_XorFetch_void_ptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_OrFetch_void_ptr
	.type	SCOREP_Atomic_OrFetch_void_ptr, %function
SCOREP_Atomic_OrFetch_void_ptr:
.L24:
	ldaxr	x2, [x0]
	orr	x2, x2, x1
	stlxr	w3, x2, [x0]
	cbnz	w3, .L24
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_OrFetch_void_ptr, .-SCOREP_Atomic_OrFetch_void_ptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_NandFetch_void_ptr
	.type	SCOREP_Atomic_NandFetch_void_ptr, %function
SCOREP_Atomic_NandFetch_void_ptr:
.L26:
	ldaxr	x2, [x0]
	and	x2, x2, x1
	mvn	x2, x2
	stlxr	w3, x2, [x0]
	cbnz	w3, .L26
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_NandFetch_void_ptr, .-SCOREP_Atomic_NandFetch_void_ptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchAdd_void_ptr
	.type	SCOREP_Atomic_FetchAdd_void_ptr, %function
SCOREP_Atomic_FetchAdd_void_ptr:
.L28:
	ldaxr	x2, [x0]
	add	x3, x2, x1
	stlxr	w4, x3, [x0]
	cbnz	w4, .L28
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_FetchAdd_void_ptr, .-SCOREP_Atomic_FetchAdd_void_ptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchSub_void_ptr
	.type	SCOREP_Atomic_FetchSub_void_ptr, %function
SCOREP_Atomic_FetchSub_void_ptr:
.L30:
	ldaxr	x2, [x0]
	sub	x3, x2, x1
	stlxr	w4, x3, [x0]
	cbnz	w4, .L30
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_FetchSub_void_ptr, .-SCOREP_Atomic_FetchSub_void_ptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchAnd_void_ptr
	.type	SCOREP_Atomic_FetchAnd_void_ptr, %function
SCOREP_Atomic_FetchAnd_void_ptr:
.L32:
	ldaxr	x2, [x0]
	and	x3, x2, x1
	stlxr	w4, x3, [x0]
	cbnz	w4, .L32
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_FetchAnd_void_ptr, .-SCOREP_Atomic_FetchAnd_void_ptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchXor_void_ptr
	.type	SCOREP_Atomic_FetchXor_void_ptr, %function
SCOREP_Atomic_FetchXor_void_ptr:
.L34:
	ldaxr	x2, [x0]
	eor	x3, x2, x1
	stlxr	w4, x3, [x0]
	cbnz	w4, .L34
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_FetchXor_void_ptr, .-SCOREP_Atomic_FetchXor_void_ptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchOr_void_ptr
	.type	SCOREP_Atomic_FetchOr_void_ptr, %function
SCOREP_Atomic_FetchOr_void_ptr:
.L36:
	ldaxr	x2, [x0]
	orr	x3, x2, x1
	stlxr	w4, x3, [x0]
	cbnz	w4, .L36
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_FetchOr_void_ptr, .-SCOREP_Atomic_FetchOr_void_ptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchNand_void_ptr
	.type	SCOREP_Atomic_FetchNand_void_ptr, %function
SCOREP_Atomic_FetchNand_void_ptr:
.L38:
	ldaxr	x2, [x0]
	and	x3, x2, x1
	mvn	x3, x3
	stlxr	w4, x3, [x0]
	cbnz	w4, .L38
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_FetchNand_void_ptr, .-SCOREP_Atomic_FetchNand_void_ptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_LoadN_bool
	.type	SCOREP_Atomic_LoadN_bool, %function
SCOREP_Atomic_LoadN_bool:
	ldarb	w0, [x0]
	tst	w0, 255
	cset	w0, ne
	ret
	.size	SCOREP_Atomic_LoadN_bool, .-SCOREP_Atomic_LoadN_bool
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_StoreN_bool
	.type	SCOREP_Atomic_StoreN_bool, %function
SCOREP_Atomic_StoreN_bool:
	uxtb	w1, w1
	stlrb	w1, [x0]
	ret
	.size	SCOREP_Atomic_StoreN_bool, .-SCOREP_Atomic_StoreN_bool
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_ExchangeN_bool
	.type	SCOREP_Atomic_ExchangeN_bool, %function
SCOREP_Atomic_ExchangeN_bool:
	uxtb	w1, w1
.L42:
	ldaxrb	w2, [x0]
	stlxrb	w3, w1, [x0]
	cbnz	w3, .L42
	tst	w2, 255
	cset	w0, ne
	ret
	.size	SCOREP_Atomic_ExchangeN_bool, .-SCOREP_Atomic_ExchangeN_bool
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_CompareExchangeN_bool
	.type	SCOREP_Atomic_CompareExchangeN_bool, %function
SCOREP_Atomic_CompareExchangeN_bool:
	ldrb	w3, [x1]
	uxtb	w2, w2
.L45:
	ldaxrb	w4, [x0]
	cmp	w4, w3
	bne	.L46
	stlxrb	w5, w2, [x0]
	cbnz	w5, .L45
.L46:
	cset	w0, eq
	cbz	w0, .L47
	ret
	.p2align 3
.L47:
	strb	w4, [x1]
	ret
	.size	SCOREP_Atomic_CompareExchangeN_bool, .-SCOREP_Atomic_CompareExchangeN_bool
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_LoadN_char
	.type	SCOREP_Atomic_LoadN_char, %function
SCOREP_Atomic_LoadN_char:
	ldarb	w0, [x0]
	ret
	.size	SCOREP_Atomic_LoadN_char, .-SCOREP_Atomic_LoadN_char
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_StoreN_char
	.type	SCOREP_Atomic_StoreN_char, %function
SCOREP_Atomic_StoreN_char:
	uxtb	w1, w1
	stlrb	w1, [x0]
	ret
	.size	SCOREP_Atomic_StoreN_char, .-SCOREP_Atomic_StoreN_char
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_ExchangeN_char
	.type	SCOREP_Atomic_ExchangeN_char, %function
SCOREP_Atomic_ExchangeN_char:
	uxtb	w1, w1
.L51:
	ldaxrb	w2, [x0]
	stlxrb	w3, w1, [x0]
	cbnz	w3, .L51
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_ExchangeN_char, .-SCOREP_Atomic_ExchangeN_char
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_CompareExchangeN_char
	.type	SCOREP_Atomic_CompareExchangeN_char, %function
SCOREP_Atomic_CompareExchangeN_char:
	ldrb	w3, [x1]
	uxtb	w2, w2
.L54:
	ldaxrb	w4, [x0]
	cmp	w4, w3
	bne	.L55
	stlxrb	w5, w2, [x0]
	cbnz	w5, .L54
.L55:
	cset	w0, eq
	cbz	w0, .L56
	ret
	.p2align 3
.L56:
	strb	w4, [x1]
	ret
	.size	SCOREP_Atomic_CompareExchangeN_char, .-SCOREP_Atomic_CompareExchangeN_char
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_AddFetch_char
	.type	SCOREP_Atomic_AddFetch_char, %function
SCOREP_Atomic_AddFetch_char:
	uxtb	w1, w1
.L58:
	ldaxrb	w2, [x0]
	add	w2, w2, w1
	stlxrb	w3, w2, [x0]
	cbnz	w3, .L58
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_AddFetch_char, .-SCOREP_Atomic_AddFetch_char
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_SubFetch_char
	.type	SCOREP_Atomic_SubFetch_char, %function
SCOREP_Atomic_SubFetch_char:
	uxtb	w1, w1
.L60:
	ldaxrb	w2, [x0]
	sub	w2, w2, w1
	stlxrb	w3, w2, [x0]
	cbnz	w3, .L60
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_SubFetch_char, .-SCOREP_Atomic_SubFetch_char
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_AndFetch_char
	.type	SCOREP_Atomic_AndFetch_char, %function
SCOREP_Atomic_AndFetch_char:
	uxtb	w1, w1
.L62:
	ldaxrb	w2, [x0]
	and	w2, w2, w1
	stlxrb	w3, w2, [x0]
	cbnz	w3, .L62
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_AndFetch_char, .-SCOREP_Atomic_AndFetch_char
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_XorFetch_char
	.type	SCOREP_Atomic_XorFetch_char, %function
SCOREP_Atomic_XorFetch_char:
	uxtb	w1, w1
.L64:
	ldaxrb	w2, [x0]
	eor	w2, w2, w1
	stlxrb	w3, w2, [x0]
	cbnz	w3, .L64
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_XorFetch_char, .-SCOREP_Atomic_XorFetch_char
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_OrFetch_char
	.type	SCOREP_Atomic_OrFetch_char, %function
SCOREP_Atomic_OrFetch_char:
	uxtb	w1, w1
.L66:
	ldaxrb	w2, [x0]
	orr	w2, w2, w1
	stlxrb	w3, w2, [x0]
	cbnz	w3, .L66
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_OrFetch_char, .-SCOREP_Atomic_OrFetch_char
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_NandFetch_char
	.type	SCOREP_Atomic_NandFetch_char, %function
SCOREP_Atomic_NandFetch_char:
	uxtb	w1, w1
.L68:
	ldaxrb	w2, [x0]
	and	w2, w2, w1
	mvn	w2, w2
	stlxrb	w3, w2, [x0]
	cbnz	w3, .L68
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_NandFetch_char, .-SCOREP_Atomic_NandFetch_char
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchAdd_char
	.type	SCOREP_Atomic_FetchAdd_char, %function
SCOREP_Atomic_FetchAdd_char:
	uxtb	w1, w1
.L70:
	ldaxrb	w2, [x0]
	add	w3, w2, w1
	stlxrb	w4, w3, [x0]
	cbnz	w4, .L70
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_FetchAdd_char, .-SCOREP_Atomic_FetchAdd_char
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchSub_char
	.type	SCOREP_Atomic_FetchSub_char, %function
SCOREP_Atomic_FetchSub_char:
	uxtb	w1, w1
.L72:
	ldaxrb	w2, [x0]
	sub	w3, w2, w1
	stlxrb	w4, w3, [x0]
	cbnz	w4, .L72
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_FetchSub_char, .-SCOREP_Atomic_FetchSub_char
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchAnd_char
	.type	SCOREP_Atomic_FetchAnd_char, %function
SCOREP_Atomic_FetchAnd_char:
	uxtb	w1, w1
.L74:
	ldaxrb	w2, [x0]
	and	w3, w2, w1
	stlxrb	w4, w3, [x0]
	cbnz	w4, .L74
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_FetchAnd_char, .-SCOREP_Atomic_FetchAnd_char
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchXor_char
	.type	SCOREP_Atomic_FetchXor_char, %function
SCOREP_Atomic_FetchXor_char:
	uxtb	w1, w1
.L76:
	ldaxrb	w2, [x0]
	eor	w3, w2, w1
	stlxrb	w4, w3, [x0]
	cbnz	w4, .L76
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_FetchXor_char, .-SCOREP_Atomic_FetchXor_char
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchOr_char
	.type	SCOREP_Atomic_FetchOr_char, %function
SCOREP_Atomic_FetchOr_char:
	uxtb	w1, w1
.L78:
	ldaxrb	w2, [x0]
	orr	w3, w2, w1
	stlxrb	w4, w3, [x0]
	cbnz	w4, .L78
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_FetchOr_char, .-SCOREP_Atomic_FetchOr_char
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchNand_char
	.type	SCOREP_Atomic_FetchNand_char, %function
SCOREP_Atomic_FetchNand_char:
	uxtb	w1, w1
.L80:
	ldaxrb	w2, [x0]
	and	w3, w2, w1
	mvn	w3, w3
	stlxrb	w4, w3, [x0]
	cbnz	w4, .L80
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_FetchNand_char, .-SCOREP_Atomic_FetchNand_char
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_LoadN_uint8
	.type	SCOREP_Atomic_LoadN_uint8, %function
SCOREP_Atomic_LoadN_uint8:
	ldarb	w0, [x0]
	ret
	.size	SCOREP_Atomic_LoadN_uint8, .-SCOREP_Atomic_LoadN_uint8
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_StoreN_uint8
	.type	SCOREP_Atomic_StoreN_uint8, %function
SCOREP_Atomic_StoreN_uint8:
	uxtb	w1, w1
	stlrb	w1, [x0]
	ret
	.size	SCOREP_Atomic_StoreN_uint8, .-SCOREP_Atomic_StoreN_uint8
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_ExchangeN_uint8
	.type	SCOREP_Atomic_ExchangeN_uint8, %function
SCOREP_Atomic_ExchangeN_uint8:
	uxtb	w1, w1
.L84:
	ldaxrb	w2, [x0]
	stlxrb	w3, w1, [x0]
	cbnz	w3, .L84
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_ExchangeN_uint8, .-SCOREP_Atomic_ExchangeN_uint8
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_CompareExchangeN_uint8
	.type	SCOREP_Atomic_CompareExchangeN_uint8, %function
SCOREP_Atomic_CompareExchangeN_uint8:
	ldrb	w3, [x1]
	uxtb	w2, w2
.L87:
	ldaxrb	w4, [x0]
	cmp	w4, w3
	bne	.L88
	stlxrb	w5, w2, [x0]
	cbnz	w5, .L87
.L88:
	cset	w0, eq
	cbz	w0, .L89
	ret
	.p2align 3
.L89:
	strb	w4, [x1]
	ret
	.size	SCOREP_Atomic_CompareExchangeN_uint8, .-SCOREP_Atomic_CompareExchangeN_uint8
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_AddFetch_uint8
	.type	SCOREP_Atomic_AddFetch_uint8, %function
SCOREP_Atomic_AddFetch_uint8:
	uxtb	w1, w1
.L91:
	ldaxrb	w2, [x0]
	add	w2, w2, w1
	stlxrb	w3, w2, [x0]
	cbnz	w3, .L91
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_AddFetch_uint8, .-SCOREP_Atomic_AddFetch_uint8
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_SubFetch_uint8
	.type	SCOREP_Atomic_SubFetch_uint8, %function
SCOREP_Atomic_SubFetch_uint8:
	uxtb	w1, w1
.L93:
	ldaxrb	w2, [x0]
	sub	w2, w2, w1
	stlxrb	w3, w2, [x0]
	cbnz	w3, .L93
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_SubFetch_uint8, .-SCOREP_Atomic_SubFetch_uint8
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_AndFetch_uint8
	.type	SCOREP_Atomic_AndFetch_uint8, %function
SCOREP_Atomic_AndFetch_uint8:
	uxtb	w1, w1
.L95:
	ldaxrb	w2, [x0]
	and	w2, w2, w1
	stlxrb	w3, w2, [x0]
	cbnz	w3, .L95
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_AndFetch_uint8, .-SCOREP_Atomic_AndFetch_uint8
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_XorFetch_uint8
	.type	SCOREP_Atomic_XorFetch_uint8, %function
SCOREP_Atomic_XorFetch_uint8:
	uxtb	w1, w1
.L97:
	ldaxrb	w2, [x0]
	eor	w2, w2, w1
	stlxrb	w3, w2, [x0]
	cbnz	w3, .L97
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_XorFetch_uint8, .-SCOREP_Atomic_XorFetch_uint8
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_OrFetch_uint8
	.type	SCOREP_Atomic_OrFetch_uint8, %function
SCOREP_Atomic_OrFetch_uint8:
	uxtb	w1, w1
.L99:
	ldaxrb	w2, [x0]
	orr	w2, w2, w1
	stlxrb	w3, w2, [x0]
	cbnz	w3, .L99
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_OrFetch_uint8, .-SCOREP_Atomic_OrFetch_uint8
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_NandFetch_uint8
	.type	SCOREP_Atomic_NandFetch_uint8, %function
SCOREP_Atomic_NandFetch_uint8:
	uxtb	w1, w1
.L101:
	ldaxrb	w2, [x0]
	and	w2, w2, w1
	mvn	w2, w2
	stlxrb	w3, w2, [x0]
	cbnz	w3, .L101
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_NandFetch_uint8, .-SCOREP_Atomic_NandFetch_uint8
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchAdd_uint8
	.type	SCOREP_Atomic_FetchAdd_uint8, %function
SCOREP_Atomic_FetchAdd_uint8:
	uxtb	w1, w1
.L103:
	ldaxrb	w2, [x0]
	add	w3, w2, w1
	stlxrb	w4, w3, [x0]
	cbnz	w4, .L103
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_FetchAdd_uint8, .-SCOREP_Atomic_FetchAdd_uint8
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchSub_uint8
	.type	SCOREP_Atomic_FetchSub_uint8, %function
SCOREP_Atomic_FetchSub_uint8:
	uxtb	w1, w1
.L105:
	ldaxrb	w2, [x0]
	sub	w3, w2, w1
	stlxrb	w4, w3, [x0]
	cbnz	w4, .L105
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_FetchSub_uint8, .-SCOREP_Atomic_FetchSub_uint8
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchAnd_uint8
	.type	SCOREP_Atomic_FetchAnd_uint8, %function
SCOREP_Atomic_FetchAnd_uint8:
	uxtb	w1, w1
.L107:
	ldaxrb	w2, [x0]
	and	w3, w2, w1
	stlxrb	w4, w3, [x0]
	cbnz	w4, .L107
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_FetchAnd_uint8, .-SCOREP_Atomic_FetchAnd_uint8
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchXor_uint8
	.type	SCOREP_Atomic_FetchXor_uint8, %function
SCOREP_Atomic_FetchXor_uint8:
	uxtb	w1, w1
.L109:
	ldaxrb	w2, [x0]
	eor	w3, w2, w1
	stlxrb	w4, w3, [x0]
	cbnz	w4, .L109
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_FetchXor_uint8, .-SCOREP_Atomic_FetchXor_uint8
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchOr_uint8
	.type	SCOREP_Atomic_FetchOr_uint8, %function
SCOREP_Atomic_FetchOr_uint8:
	uxtb	w1, w1
.L111:
	ldaxrb	w2, [x0]
	orr	w3, w2, w1
	stlxrb	w4, w3, [x0]
	cbnz	w4, .L111
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_FetchOr_uint8, .-SCOREP_Atomic_FetchOr_uint8
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchNand_uint8
	.type	SCOREP_Atomic_FetchNand_uint8, %function
SCOREP_Atomic_FetchNand_uint8:
	uxtb	w1, w1
.L113:
	ldaxrb	w2, [x0]
	and	w3, w2, w1
	mvn	w3, w3
	stlxrb	w4, w3, [x0]
	cbnz	w4, .L113
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_FetchNand_uint8, .-SCOREP_Atomic_FetchNand_uint8
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_LoadN_uint16
	.type	SCOREP_Atomic_LoadN_uint16, %function
SCOREP_Atomic_LoadN_uint16:
	ldarh	w0, [x0]
	ret
	.size	SCOREP_Atomic_LoadN_uint16, .-SCOREP_Atomic_LoadN_uint16
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_StoreN_uint16
	.type	SCOREP_Atomic_StoreN_uint16, %function
SCOREP_Atomic_StoreN_uint16:
	uxth	w1, w1
	stlrh	w1, [x0]
	ret
	.size	SCOREP_Atomic_StoreN_uint16, .-SCOREP_Atomic_StoreN_uint16
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_ExchangeN_uint16
	.type	SCOREP_Atomic_ExchangeN_uint16, %function
SCOREP_Atomic_ExchangeN_uint16:
	uxth	w1, w1
.L117:
	ldaxrh	w2, [x0]
	stlxrh	w3, w1, [x0]
	cbnz	w3, .L117
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_ExchangeN_uint16, .-SCOREP_Atomic_ExchangeN_uint16
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_CompareExchangeN_uint16
	.type	SCOREP_Atomic_CompareExchangeN_uint16, %function
SCOREP_Atomic_CompareExchangeN_uint16:
	ldrh	w3, [x1]
	uxth	w2, w2
.L120:
	ldaxrh	w4, [x0]
	cmp	w4, w3
	bne	.L121
	stlxrh	w5, w2, [x0]
	cbnz	w5, .L120
.L121:
	cset	w0, eq
	cbz	w0, .L122
	ret
	.p2align 3
.L122:
	strh	w4, [x1]
	ret
	.size	SCOREP_Atomic_CompareExchangeN_uint16, .-SCOREP_Atomic_CompareExchangeN_uint16
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_AddFetch_uint16
	.type	SCOREP_Atomic_AddFetch_uint16, %function
SCOREP_Atomic_AddFetch_uint16:
	uxth	w1, w1
.L124:
	ldaxrh	w2, [x0]
	add	w2, w2, w1
	stlxrh	w3, w2, [x0]
	cbnz	w3, .L124
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_AddFetch_uint16, .-SCOREP_Atomic_AddFetch_uint16
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_SubFetch_uint16
	.type	SCOREP_Atomic_SubFetch_uint16, %function
SCOREP_Atomic_SubFetch_uint16:
	uxth	w1, w1
.L126:
	ldaxrh	w2, [x0]
	sub	w2, w2, w1
	stlxrh	w3, w2, [x0]
	cbnz	w3, .L126
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_SubFetch_uint16, .-SCOREP_Atomic_SubFetch_uint16
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_AndFetch_uint16
	.type	SCOREP_Atomic_AndFetch_uint16, %function
SCOREP_Atomic_AndFetch_uint16:
	uxth	w1, w1
.L128:
	ldaxrh	w2, [x0]
	and	w2, w2, w1
	stlxrh	w3, w2, [x0]
	cbnz	w3, .L128
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_AndFetch_uint16, .-SCOREP_Atomic_AndFetch_uint16
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_XorFetch_uint16
	.type	SCOREP_Atomic_XorFetch_uint16, %function
SCOREP_Atomic_XorFetch_uint16:
	uxth	w1, w1
.L130:
	ldaxrh	w2, [x0]
	eor	w2, w2, w1
	stlxrh	w3, w2, [x0]
	cbnz	w3, .L130
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_XorFetch_uint16, .-SCOREP_Atomic_XorFetch_uint16
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_OrFetch_uint16
	.type	SCOREP_Atomic_OrFetch_uint16, %function
SCOREP_Atomic_OrFetch_uint16:
	uxth	w1, w1
.L132:
	ldaxrh	w2, [x0]
	orr	w2, w2, w1
	stlxrh	w3, w2, [x0]
	cbnz	w3, .L132
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_OrFetch_uint16, .-SCOREP_Atomic_OrFetch_uint16
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_NandFetch_uint16
	.type	SCOREP_Atomic_NandFetch_uint16, %function
SCOREP_Atomic_NandFetch_uint16:
	uxth	w1, w1
.L134:
	ldaxrh	w2, [x0]
	and	w2, w2, w1
	mvn	w2, w2
	stlxrh	w3, w2, [x0]
	cbnz	w3, .L134
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_NandFetch_uint16, .-SCOREP_Atomic_NandFetch_uint16
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchAdd_uint16
	.type	SCOREP_Atomic_FetchAdd_uint16, %function
SCOREP_Atomic_FetchAdd_uint16:
	uxth	w1, w1
.L136:
	ldaxrh	w2, [x0]
	add	w3, w2, w1
	stlxrh	w4, w3, [x0]
	cbnz	w4, .L136
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_FetchAdd_uint16, .-SCOREP_Atomic_FetchAdd_uint16
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchSub_uint16
	.type	SCOREP_Atomic_FetchSub_uint16, %function
SCOREP_Atomic_FetchSub_uint16:
	uxth	w1, w1
.L138:
	ldaxrh	w2, [x0]
	sub	w3, w2, w1
	stlxrh	w4, w3, [x0]
	cbnz	w4, .L138
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_FetchSub_uint16, .-SCOREP_Atomic_FetchSub_uint16
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchAnd_uint16
	.type	SCOREP_Atomic_FetchAnd_uint16, %function
SCOREP_Atomic_FetchAnd_uint16:
	uxth	w1, w1
.L140:
	ldaxrh	w2, [x0]
	and	w3, w2, w1
	stlxrh	w4, w3, [x0]
	cbnz	w4, .L140
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_FetchAnd_uint16, .-SCOREP_Atomic_FetchAnd_uint16
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchXor_uint16
	.type	SCOREP_Atomic_FetchXor_uint16, %function
SCOREP_Atomic_FetchXor_uint16:
	uxth	w1, w1
.L142:
	ldaxrh	w2, [x0]
	eor	w3, w2, w1
	stlxrh	w4, w3, [x0]
	cbnz	w4, .L142
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_FetchXor_uint16, .-SCOREP_Atomic_FetchXor_uint16
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchOr_uint16
	.type	SCOREP_Atomic_FetchOr_uint16, %function
SCOREP_Atomic_FetchOr_uint16:
	uxth	w1, w1
.L144:
	ldaxrh	w2, [x0]
	orr	w3, w2, w1
	stlxrh	w4, w3, [x0]
	cbnz	w4, .L144
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_FetchOr_uint16, .-SCOREP_Atomic_FetchOr_uint16
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchNand_uint16
	.type	SCOREP_Atomic_FetchNand_uint16, %function
SCOREP_Atomic_FetchNand_uint16:
	uxth	w1, w1
.L146:
	ldaxrh	w2, [x0]
	and	w3, w2, w1
	mvn	w3, w3
	stlxrh	w4, w3, [x0]
	cbnz	w4, .L146
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_FetchNand_uint16, .-SCOREP_Atomic_FetchNand_uint16
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_LoadN_uint32
	.type	SCOREP_Atomic_LoadN_uint32, %function
SCOREP_Atomic_LoadN_uint32:
	ldar	w0, [x0]
	ret
	.size	SCOREP_Atomic_LoadN_uint32, .-SCOREP_Atomic_LoadN_uint32
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_StoreN_uint32
	.type	SCOREP_Atomic_StoreN_uint32, %function
SCOREP_Atomic_StoreN_uint32:
	stlr	w1, [x0]
	ret
	.size	SCOREP_Atomic_StoreN_uint32, .-SCOREP_Atomic_StoreN_uint32
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_ExchangeN_uint32
	.type	SCOREP_Atomic_ExchangeN_uint32, %function
SCOREP_Atomic_ExchangeN_uint32:
.L150:
	ldaxr	w2, [x0]
	stlxr	w3, w1, [x0]
	cbnz	w3, .L150
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_ExchangeN_uint32, .-SCOREP_Atomic_ExchangeN_uint32
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_CompareExchangeN_uint32
	.type	SCOREP_Atomic_CompareExchangeN_uint32, %function
SCOREP_Atomic_CompareExchangeN_uint32:
	ldr	w3, [x1]
.L153:
	ldaxr	w4, [x0]
	cmp	w4, w3
	bne	.L154
	stlxr	w5, w2, [x0]
	cbnz	w5, .L153
.L154:
	cset	w0, eq
	cbz	w0, .L155
	ret
	.p2align 3
.L155:
	str	w4, [x1]
	ret
	.size	SCOREP_Atomic_CompareExchangeN_uint32, .-SCOREP_Atomic_CompareExchangeN_uint32
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_AddFetch_uint32
	.type	SCOREP_Atomic_AddFetch_uint32, %function
SCOREP_Atomic_AddFetch_uint32:
.L157:
	ldaxr	w2, [x0]
	add	w2, w2, w1
	stlxr	w3, w2, [x0]
	cbnz	w3, .L157
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_AddFetch_uint32, .-SCOREP_Atomic_AddFetch_uint32
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_SubFetch_uint32
	.type	SCOREP_Atomic_SubFetch_uint32, %function
SCOREP_Atomic_SubFetch_uint32:
.L159:
	ldaxr	w2, [x0]
	sub	w2, w2, w1
	stlxr	w3, w2, [x0]
	cbnz	w3, .L159
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_SubFetch_uint32, .-SCOREP_Atomic_SubFetch_uint32
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_AndFetch_uint32
	.type	SCOREP_Atomic_AndFetch_uint32, %function
SCOREP_Atomic_AndFetch_uint32:
.L161:
	ldaxr	w2, [x0]
	and	w2, w2, w1
	stlxr	w3, w2, [x0]
	cbnz	w3, .L161
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_AndFetch_uint32, .-SCOREP_Atomic_AndFetch_uint32
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_XorFetch_uint32
	.type	SCOREP_Atomic_XorFetch_uint32, %function
SCOREP_Atomic_XorFetch_uint32:
.L163:
	ldaxr	w2, [x0]
	eor	w2, w2, w1
	stlxr	w3, w2, [x0]
	cbnz	w3, .L163
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_XorFetch_uint32, .-SCOREP_Atomic_XorFetch_uint32
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_OrFetch_uint32
	.type	SCOREP_Atomic_OrFetch_uint32, %function
SCOREP_Atomic_OrFetch_uint32:
.L165:
	ldaxr	w2, [x0]
	orr	w2, w2, w1
	stlxr	w3, w2, [x0]
	cbnz	w3, .L165
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_OrFetch_uint32, .-SCOREP_Atomic_OrFetch_uint32
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_NandFetch_uint32
	.type	SCOREP_Atomic_NandFetch_uint32, %function
SCOREP_Atomic_NandFetch_uint32:
.L167:
	ldaxr	w2, [x0]
	and	w2, w2, w1
	mvn	w2, w2
	stlxr	w3, w2, [x0]
	cbnz	w3, .L167
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_NandFetch_uint32, .-SCOREP_Atomic_NandFetch_uint32
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchAdd_uint32
	.type	SCOREP_Atomic_FetchAdd_uint32, %function
SCOREP_Atomic_FetchAdd_uint32:
.L169:
	ldaxr	w2, [x0]
	add	w3, w2, w1
	stlxr	w4, w3, [x0]
	cbnz	w4, .L169
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_FetchAdd_uint32, .-SCOREP_Atomic_FetchAdd_uint32
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchSub_uint32
	.type	SCOREP_Atomic_FetchSub_uint32, %function
SCOREP_Atomic_FetchSub_uint32:
.L171:
	ldaxr	w2, [x0]
	sub	w3, w2, w1
	stlxr	w4, w3, [x0]
	cbnz	w4, .L171
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_FetchSub_uint32, .-SCOREP_Atomic_FetchSub_uint32
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchAnd_uint32
	.type	SCOREP_Atomic_FetchAnd_uint32, %function
SCOREP_Atomic_FetchAnd_uint32:
.L173:
	ldaxr	w2, [x0]
	and	w3, w2, w1
	stlxr	w4, w3, [x0]
	cbnz	w4, .L173
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_FetchAnd_uint32, .-SCOREP_Atomic_FetchAnd_uint32
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchXor_uint32
	.type	SCOREP_Atomic_FetchXor_uint32, %function
SCOREP_Atomic_FetchXor_uint32:
.L175:
	ldaxr	w2, [x0]
	eor	w3, w2, w1
	stlxr	w4, w3, [x0]
	cbnz	w4, .L175
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_FetchXor_uint32, .-SCOREP_Atomic_FetchXor_uint32
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchOr_uint32
	.type	SCOREP_Atomic_FetchOr_uint32, %function
SCOREP_Atomic_FetchOr_uint32:
.L177:
	ldaxr	w2, [x0]
	orr	w3, w2, w1
	stlxr	w4, w3, [x0]
	cbnz	w4, .L177
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_FetchOr_uint32, .-SCOREP_Atomic_FetchOr_uint32
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchNand_uint32
	.type	SCOREP_Atomic_FetchNand_uint32, %function
SCOREP_Atomic_FetchNand_uint32:
.L179:
	ldaxr	w2, [x0]
	and	w3, w2, w1
	mvn	w3, w3
	stlxr	w4, w3, [x0]
	cbnz	w4, .L179
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_FetchNand_uint32, .-SCOREP_Atomic_FetchNand_uint32
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_LoadN_uint64
	.type	SCOREP_Atomic_LoadN_uint64, %function
SCOREP_Atomic_LoadN_uint64:
	ldar	x0, [x0]
	ret
	.size	SCOREP_Atomic_LoadN_uint64, .-SCOREP_Atomic_LoadN_uint64
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_StoreN_uint64
	.type	SCOREP_Atomic_StoreN_uint64, %function
SCOREP_Atomic_StoreN_uint64:
	stlr	x1, [x0]
	ret
	.size	SCOREP_Atomic_StoreN_uint64, .-SCOREP_Atomic_StoreN_uint64
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_ExchangeN_uint64
	.type	SCOREP_Atomic_ExchangeN_uint64, %function
SCOREP_Atomic_ExchangeN_uint64:
.L183:
	ldaxr	x2, [x0]
	stlxr	w3, x1, [x0]
	cbnz	w3, .L183
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_ExchangeN_uint64, .-SCOREP_Atomic_ExchangeN_uint64
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_CompareExchangeN_uint64
	.type	SCOREP_Atomic_CompareExchangeN_uint64, %function
SCOREP_Atomic_CompareExchangeN_uint64:
	ldr	x3, [x1]
.L186:
	ldaxr	x4, [x0]
	cmp	x4, x3
	bne	.L187
	stlxr	w5, x2, [x0]
	cbnz	w5, .L186
.L187:
	cset	w0, eq
	cbz	w0, .L188
	ret
	.p2align 3
.L188:
	str	x4, [x1]
	ret
	.size	SCOREP_Atomic_CompareExchangeN_uint64, .-SCOREP_Atomic_CompareExchangeN_uint64
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_AddFetch_uint64
	.type	SCOREP_Atomic_AddFetch_uint64, %function
SCOREP_Atomic_AddFetch_uint64:
.L190:
	ldaxr	x2, [x0]
	add	x2, x2, x1
	stlxr	w3, x2, [x0]
	cbnz	w3, .L190
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_AddFetch_uint64, .-SCOREP_Atomic_AddFetch_uint64
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_SubFetch_uint64
	.type	SCOREP_Atomic_SubFetch_uint64, %function
SCOREP_Atomic_SubFetch_uint64:
.L192:
	ldaxr	x2, [x0]
	sub	x2, x2, x1
	stlxr	w3, x2, [x0]
	cbnz	w3, .L192
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_SubFetch_uint64, .-SCOREP_Atomic_SubFetch_uint64
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_AndFetch_uint64
	.type	SCOREP_Atomic_AndFetch_uint64, %function
SCOREP_Atomic_AndFetch_uint64:
.L194:
	ldaxr	x2, [x0]
	and	x2, x2, x1
	stlxr	w3, x2, [x0]
	cbnz	w3, .L194
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_AndFetch_uint64, .-SCOREP_Atomic_AndFetch_uint64
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_XorFetch_uint64
	.type	SCOREP_Atomic_XorFetch_uint64, %function
SCOREP_Atomic_XorFetch_uint64:
.L196:
	ldaxr	x2, [x0]
	eor	x2, x2, x1
	stlxr	w3, x2, [x0]
	cbnz	w3, .L196
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_XorFetch_uint64, .-SCOREP_Atomic_XorFetch_uint64
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_OrFetch_uint64
	.type	SCOREP_Atomic_OrFetch_uint64, %function
SCOREP_Atomic_OrFetch_uint64:
.L198:
	ldaxr	x2, [x0]
	orr	x2, x2, x1
	stlxr	w3, x2, [x0]
	cbnz	w3, .L198
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_OrFetch_uint64, .-SCOREP_Atomic_OrFetch_uint64
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_NandFetch_uint64
	.type	SCOREP_Atomic_NandFetch_uint64, %function
SCOREP_Atomic_NandFetch_uint64:
.L200:
	ldaxr	x2, [x0]
	and	x2, x2, x1
	mvn	x2, x2
	stlxr	w3, x2, [x0]
	cbnz	w3, .L200
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_NandFetch_uint64, .-SCOREP_Atomic_NandFetch_uint64
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchAdd_uint64
	.type	SCOREP_Atomic_FetchAdd_uint64, %function
SCOREP_Atomic_FetchAdd_uint64:
.L202:
	ldaxr	x2, [x0]
	add	x3, x2, x1
	stlxr	w4, x3, [x0]
	cbnz	w4, .L202
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_FetchAdd_uint64, .-SCOREP_Atomic_FetchAdd_uint64
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchSub_uint64
	.type	SCOREP_Atomic_FetchSub_uint64, %function
SCOREP_Atomic_FetchSub_uint64:
.L204:
	ldaxr	x2, [x0]
	sub	x3, x2, x1
	stlxr	w4, x3, [x0]
	cbnz	w4, .L204
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_FetchSub_uint64, .-SCOREP_Atomic_FetchSub_uint64
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchAnd_uint64
	.type	SCOREP_Atomic_FetchAnd_uint64, %function
SCOREP_Atomic_FetchAnd_uint64:
.L206:
	ldaxr	x2, [x0]
	and	x3, x2, x1
	stlxr	w4, x3, [x0]
	cbnz	w4, .L206
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_FetchAnd_uint64, .-SCOREP_Atomic_FetchAnd_uint64
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchXor_uint64
	.type	SCOREP_Atomic_FetchXor_uint64, %function
SCOREP_Atomic_FetchXor_uint64:
.L208:
	ldaxr	x2, [x0]
	eor	x3, x2, x1
	stlxr	w4, x3, [x0]
	cbnz	w4, .L208
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_FetchXor_uint64, .-SCOREP_Atomic_FetchXor_uint64
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchOr_uint64
	.type	SCOREP_Atomic_FetchOr_uint64, %function
SCOREP_Atomic_FetchOr_uint64:
.L210:
	ldaxr	x2, [x0]
	orr	x3, x2, x1
	stlxr	w4, x3, [x0]
	cbnz	w4, .L210
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_FetchOr_uint64, .-SCOREP_Atomic_FetchOr_uint64
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchNand_uint64
	.type	SCOREP_Atomic_FetchNand_uint64, %function
SCOREP_Atomic_FetchNand_uint64:
.L212:
	ldaxr	x2, [x0]
	and	x3, x2, x1
	mvn	x3, x3
	stlxr	w4, x3, [x0]
	cbnz	w4, .L212
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_FetchNand_uint64, .-SCOREP_Atomic_FetchNand_uint64
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_LoadN_uintptr
	.type	SCOREP_Atomic_LoadN_uintptr, %function
SCOREP_Atomic_LoadN_uintptr:
	ldar	x0, [x0]
	ret
	.size	SCOREP_Atomic_LoadN_uintptr, .-SCOREP_Atomic_LoadN_uintptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_StoreN_uintptr
	.type	SCOREP_Atomic_StoreN_uintptr, %function
SCOREP_Atomic_StoreN_uintptr:
	stlr	x1, [x0]
	ret
	.size	SCOREP_Atomic_StoreN_uintptr, .-SCOREP_Atomic_StoreN_uintptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_ExchangeN_uintptr
	.type	SCOREP_Atomic_ExchangeN_uintptr, %function
SCOREP_Atomic_ExchangeN_uintptr:
.L216:
	ldaxr	x2, [x0]
	stlxr	w3, x1, [x0]
	cbnz	w3, .L216
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_ExchangeN_uintptr, .-SCOREP_Atomic_ExchangeN_uintptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_CompareExchangeN_uintptr
	.type	SCOREP_Atomic_CompareExchangeN_uintptr, %function
SCOREP_Atomic_CompareExchangeN_uintptr:
	ldr	x3, [x1]
.L219:
	ldaxr	x4, [x0]
	cmp	x4, x3
	bne	.L220
	stlxr	w5, x2, [x0]
	cbnz	w5, .L219
.L220:
	cset	w0, eq
	cbz	w0, .L221
	ret
	.p2align 3
.L221:
	str	x4, [x1]
	ret
	.size	SCOREP_Atomic_CompareExchangeN_uintptr, .-SCOREP_Atomic_CompareExchangeN_uintptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_AddFetch_uintptr
	.type	SCOREP_Atomic_AddFetch_uintptr, %function
SCOREP_Atomic_AddFetch_uintptr:
.L223:
	ldaxr	x2, [x0]
	add	x2, x2, x1
	stlxr	w3, x2, [x0]
	cbnz	w3, .L223
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_AddFetch_uintptr, .-SCOREP_Atomic_AddFetch_uintptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_SubFetch_uintptr
	.type	SCOREP_Atomic_SubFetch_uintptr, %function
SCOREP_Atomic_SubFetch_uintptr:
.L225:
	ldaxr	x2, [x0]
	sub	x2, x2, x1
	stlxr	w3, x2, [x0]
	cbnz	w3, .L225
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_SubFetch_uintptr, .-SCOREP_Atomic_SubFetch_uintptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_AndFetch_uintptr
	.type	SCOREP_Atomic_AndFetch_uintptr, %function
SCOREP_Atomic_AndFetch_uintptr:
.L227:
	ldaxr	x2, [x0]
	and	x2, x2, x1
	stlxr	w3, x2, [x0]
	cbnz	w3, .L227
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_AndFetch_uintptr, .-SCOREP_Atomic_AndFetch_uintptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_XorFetch_uintptr
	.type	SCOREP_Atomic_XorFetch_uintptr, %function
SCOREP_Atomic_XorFetch_uintptr:
.L229:
	ldaxr	x2, [x0]
	eor	x2, x2, x1
	stlxr	w3, x2, [x0]
	cbnz	w3, .L229
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_XorFetch_uintptr, .-SCOREP_Atomic_XorFetch_uintptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_OrFetch_uintptr
	.type	SCOREP_Atomic_OrFetch_uintptr, %function
SCOREP_Atomic_OrFetch_uintptr:
.L231:
	ldaxr	x2, [x0]
	orr	x2, x2, x1
	stlxr	w3, x2, [x0]
	cbnz	w3, .L231
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_OrFetch_uintptr, .-SCOREP_Atomic_OrFetch_uintptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_NandFetch_uintptr
	.type	SCOREP_Atomic_NandFetch_uintptr, %function
SCOREP_Atomic_NandFetch_uintptr:
.L233:
	ldaxr	x2, [x0]
	and	x2, x2, x1
	mvn	x2, x2
	stlxr	w3, x2, [x0]
	cbnz	w3, .L233
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_NandFetch_uintptr, .-SCOREP_Atomic_NandFetch_uintptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchAdd_uintptr
	.type	SCOREP_Atomic_FetchAdd_uintptr, %function
SCOREP_Atomic_FetchAdd_uintptr:
.L235:
	ldaxr	x2, [x0]
	add	x3, x2, x1
	stlxr	w4, x3, [x0]
	cbnz	w4, .L235
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_FetchAdd_uintptr, .-SCOREP_Atomic_FetchAdd_uintptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchSub_uintptr
	.type	SCOREP_Atomic_FetchSub_uintptr, %function
SCOREP_Atomic_FetchSub_uintptr:
.L237:
	ldaxr	x2, [x0]
	sub	x3, x2, x1
	stlxr	w4, x3, [x0]
	cbnz	w4, .L237
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_FetchSub_uintptr, .-SCOREP_Atomic_FetchSub_uintptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchAnd_uintptr
	.type	SCOREP_Atomic_FetchAnd_uintptr, %function
SCOREP_Atomic_FetchAnd_uintptr:
.L239:
	ldaxr	x2, [x0]
	and	x3, x2, x1
	stlxr	w4, x3, [x0]
	cbnz	w4, .L239
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_FetchAnd_uintptr, .-SCOREP_Atomic_FetchAnd_uintptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchXor_uintptr
	.type	SCOREP_Atomic_FetchXor_uintptr, %function
SCOREP_Atomic_FetchXor_uintptr:
.L241:
	ldaxr	x2, [x0]
	eor	x3, x2, x1
	stlxr	w4, x3, [x0]
	cbnz	w4, .L241
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_FetchXor_uintptr, .-SCOREP_Atomic_FetchXor_uintptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchOr_uintptr
	.type	SCOREP_Atomic_FetchOr_uintptr, %function
SCOREP_Atomic_FetchOr_uintptr:
.L243:
	ldaxr	x2, [x0]
	orr	x3, x2, x1
	stlxr	w4, x3, [x0]
	cbnz	w4, .L243
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_FetchOr_uintptr, .-SCOREP_Atomic_FetchOr_uintptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchNand_uintptr
	.type	SCOREP_Atomic_FetchNand_uintptr, %function
SCOREP_Atomic_FetchNand_uintptr:
.L245:
	ldaxr	x2, [x0]
	and	x3, x2, x1
	mvn	x3, x3
	stlxr	w4, x3, [x0]
	cbnz	w4, .L245
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_FetchNand_uintptr, .-SCOREP_Atomic_FetchNand_uintptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_LoadN_int8
	.type	SCOREP_Atomic_LoadN_int8, %function
SCOREP_Atomic_LoadN_int8:
	ldarb	w0, [x0]
	ret
	.size	SCOREP_Atomic_LoadN_int8, .-SCOREP_Atomic_LoadN_int8
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_StoreN_int8
	.type	SCOREP_Atomic_StoreN_int8, %function
SCOREP_Atomic_StoreN_int8:
	sxtb	w1, w1
	stlrb	w1, [x0]
	ret
	.size	SCOREP_Atomic_StoreN_int8, .-SCOREP_Atomic_StoreN_int8
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_ExchangeN_int8
	.type	SCOREP_Atomic_ExchangeN_int8, %function
SCOREP_Atomic_ExchangeN_int8:
	sxtb	w1, w1
.L249:
	ldaxrb	w2, [x0]
	stlxrb	w3, w1, [x0]
	cbnz	w3, .L249
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_ExchangeN_int8, .-SCOREP_Atomic_ExchangeN_int8
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_CompareExchangeN_int8
	.type	SCOREP_Atomic_CompareExchangeN_int8, %function
SCOREP_Atomic_CompareExchangeN_int8:
	ldrb	w3, [x1]
	sxtb	w2, w2
.L252:
	ldaxrb	w4, [x0]
	cmp	w4, w3
	bne	.L253
	stlxrb	w5, w2, [x0]
	cbnz	w5, .L252
.L253:
	cset	w0, eq
	cbz	w0, .L254
	ret
	.p2align 3
.L254:
	strb	w4, [x1]
	ret
	.size	SCOREP_Atomic_CompareExchangeN_int8, .-SCOREP_Atomic_CompareExchangeN_int8
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_AddFetch_int8
	.type	SCOREP_Atomic_AddFetch_int8, %function
SCOREP_Atomic_AddFetch_int8:
	sxtb	w1, w1
.L256:
	ldaxrb	w2, [x0]
	add	w2, w2, w1
	stlxrb	w3, w2, [x0]
	cbnz	w3, .L256
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_AddFetch_int8, .-SCOREP_Atomic_AddFetch_int8
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_SubFetch_int8
	.type	SCOREP_Atomic_SubFetch_int8, %function
SCOREP_Atomic_SubFetch_int8:
	sxtb	w1, w1
.L258:
	ldaxrb	w2, [x0]
	sub	w2, w2, w1
	stlxrb	w3, w2, [x0]
	cbnz	w3, .L258
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_SubFetch_int8, .-SCOREP_Atomic_SubFetch_int8
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_AndFetch_int8
	.type	SCOREP_Atomic_AndFetch_int8, %function
SCOREP_Atomic_AndFetch_int8:
	sxtb	w1, w1
.L260:
	ldaxrb	w2, [x0]
	and	w2, w2, w1
	stlxrb	w3, w2, [x0]
	cbnz	w3, .L260
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_AndFetch_int8, .-SCOREP_Atomic_AndFetch_int8
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_XorFetch_int8
	.type	SCOREP_Atomic_XorFetch_int8, %function
SCOREP_Atomic_XorFetch_int8:
	sxtb	w1, w1
.L262:
	ldaxrb	w2, [x0]
	eor	w2, w2, w1
	stlxrb	w3, w2, [x0]
	cbnz	w3, .L262
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_XorFetch_int8, .-SCOREP_Atomic_XorFetch_int8
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_OrFetch_int8
	.type	SCOREP_Atomic_OrFetch_int8, %function
SCOREP_Atomic_OrFetch_int8:
	sxtb	w1, w1
.L264:
	ldaxrb	w2, [x0]
	orr	w2, w2, w1
	stlxrb	w3, w2, [x0]
	cbnz	w3, .L264
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_OrFetch_int8, .-SCOREP_Atomic_OrFetch_int8
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_NandFetch_int8
	.type	SCOREP_Atomic_NandFetch_int8, %function
SCOREP_Atomic_NandFetch_int8:
	sxtb	w1, w1
.L266:
	ldaxrb	w2, [x0]
	and	w2, w2, w1
	mvn	w2, w2
	stlxrb	w3, w2, [x0]
	cbnz	w3, .L266
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_NandFetch_int8, .-SCOREP_Atomic_NandFetch_int8
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchAdd_int8
	.type	SCOREP_Atomic_FetchAdd_int8, %function
SCOREP_Atomic_FetchAdd_int8:
	sxtb	w1, w1
.L268:
	ldaxrb	w2, [x0]
	add	w3, w2, w1
	stlxrb	w4, w3, [x0]
	cbnz	w4, .L268
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_FetchAdd_int8, .-SCOREP_Atomic_FetchAdd_int8
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchSub_int8
	.type	SCOREP_Atomic_FetchSub_int8, %function
SCOREP_Atomic_FetchSub_int8:
	sxtb	w1, w1
.L270:
	ldaxrb	w2, [x0]
	sub	w3, w2, w1
	stlxrb	w4, w3, [x0]
	cbnz	w4, .L270
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_FetchSub_int8, .-SCOREP_Atomic_FetchSub_int8
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchAnd_int8
	.type	SCOREP_Atomic_FetchAnd_int8, %function
SCOREP_Atomic_FetchAnd_int8:
	sxtb	w1, w1
.L272:
	ldaxrb	w2, [x0]
	and	w3, w2, w1
	stlxrb	w4, w3, [x0]
	cbnz	w4, .L272
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_FetchAnd_int8, .-SCOREP_Atomic_FetchAnd_int8
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchXor_int8
	.type	SCOREP_Atomic_FetchXor_int8, %function
SCOREP_Atomic_FetchXor_int8:
	sxtb	w1, w1
.L274:
	ldaxrb	w2, [x0]
	eor	w3, w2, w1
	stlxrb	w4, w3, [x0]
	cbnz	w4, .L274
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_FetchXor_int8, .-SCOREP_Atomic_FetchXor_int8
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchOr_int8
	.type	SCOREP_Atomic_FetchOr_int8, %function
SCOREP_Atomic_FetchOr_int8:
	sxtb	w1, w1
.L276:
	ldaxrb	w2, [x0]
	orr	w3, w2, w1
	stlxrb	w4, w3, [x0]
	cbnz	w4, .L276
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_FetchOr_int8, .-SCOREP_Atomic_FetchOr_int8
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchNand_int8
	.type	SCOREP_Atomic_FetchNand_int8, %function
SCOREP_Atomic_FetchNand_int8:
	sxtb	w1, w1
.L278:
	ldaxrb	w2, [x0]
	and	w3, w2, w1
	mvn	w3, w3
	stlxrb	w4, w3, [x0]
	cbnz	w4, .L278
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_FetchNand_int8, .-SCOREP_Atomic_FetchNand_int8
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_LoadN_int16
	.type	SCOREP_Atomic_LoadN_int16, %function
SCOREP_Atomic_LoadN_int16:
	ldarh	w0, [x0]
	ret
	.size	SCOREP_Atomic_LoadN_int16, .-SCOREP_Atomic_LoadN_int16
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_StoreN_int16
	.type	SCOREP_Atomic_StoreN_int16, %function
SCOREP_Atomic_StoreN_int16:
	sxth	w1, w1
	stlrh	w1, [x0]
	ret
	.size	SCOREP_Atomic_StoreN_int16, .-SCOREP_Atomic_StoreN_int16
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_ExchangeN_int16
	.type	SCOREP_Atomic_ExchangeN_int16, %function
SCOREP_Atomic_ExchangeN_int16:
	sxth	w1, w1
.L282:
	ldaxrh	w2, [x0]
	stlxrh	w3, w1, [x0]
	cbnz	w3, .L282
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_ExchangeN_int16, .-SCOREP_Atomic_ExchangeN_int16
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_CompareExchangeN_int16
	.type	SCOREP_Atomic_CompareExchangeN_int16, %function
SCOREP_Atomic_CompareExchangeN_int16:
	ldrh	w3, [x1]
	sxth	w2, w2
.L285:
	ldaxrh	w4, [x0]
	cmp	w4, w3
	bne	.L286
	stlxrh	w5, w2, [x0]
	cbnz	w5, .L285
.L286:
	cset	w0, eq
	cbz	w0, .L287
	ret
	.p2align 3
.L287:
	strh	w4, [x1]
	ret
	.size	SCOREP_Atomic_CompareExchangeN_int16, .-SCOREP_Atomic_CompareExchangeN_int16
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_AddFetch_int16
	.type	SCOREP_Atomic_AddFetch_int16, %function
SCOREP_Atomic_AddFetch_int16:
	sxth	w1, w1
.L289:
	ldaxrh	w2, [x0]
	add	w2, w2, w1
	stlxrh	w3, w2, [x0]
	cbnz	w3, .L289
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_AddFetch_int16, .-SCOREP_Atomic_AddFetch_int16
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_SubFetch_int16
	.type	SCOREP_Atomic_SubFetch_int16, %function
SCOREP_Atomic_SubFetch_int16:
	sxth	w1, w1
.L291:
	ldaxrh	w2, [x0]
	sub	w2, w2, w1
	stlxrh	w3, w2, [x0]
	cbnz	w3, .L291
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_SubFetch_int16, .-SCOREP_Atomic_SubFetch_int16
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_AndFetch_int16
	.type	SCOREP_Atomic_AndFetch_int16, %function
SCOREP_Atomic_AndFetch_int16:
	sxth	w1, w1
.L293:
	ldaxrh	w2, [x0]
	and	w2, w2, w1
	stlxrh	w3, w2, [x0]
	cbnz	w3, .L293
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_AndFetch_int16, .-SCOREP_Atomic_AndFetch_int16
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_XorFetch_int16
	.type	SCOREP_Atomic_XorFetch_int16, %function
SCOREP_Atomic_XorFetch_int16:
	sxth	w1, w1
.L295:
	ldaxrh	w2, [x0]
	eor	w2, w2, w1
	stlxrh	w3, w2, [x0]
	cbnz	w3, .L295
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_XorFetch_int16, .-SCOREP_Atomic_XorFetch_int16
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_OrFetch_int16
	.type	SCOREP_Atomic_OrFetch_int16, %function
SCOREP_Atomic_OrFetch_int16:
	sxth	w1, w1
.L297:
	ldaxrh	w2, [x0]
	orr	w2, w2, w1
	stlxrh	w3, w2, [x0]
	cbnz	w3, .L297
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_OrFetch_int16, .-SCOREP_Atomic_OrFetch_int16
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_NandFetch_int16
	.type	SCOREP_Atomic_NandFetch_int16, %function
SCOREP_Atomic_NandFetch_int16:
	sxth	w1, w1
.L299:
	ldaxrh	w2, [x0]
	and	w2, w2, w1
	mvn	w2, w2
	stlxrh	w3, w2, [x0]
	cbnz	w3, .L299
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_NandFetch_int16, .-SCOREP_Atomic_NandFetch_int16
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchAdd_int16
	.type	SCOREP_Atomic_FetchAdd_int16, %function
SCOREP_Atomic_FetchAdd_int16:
	sxth	w1, w1
.L301:
	ldaxrh	w2, [x0]
	add	w3, w2, w1
	stlxrh	w4, w3, [x0]
	cbnz	w4, .L301
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_FetchAdd_int16, .-SCOREP_Atomic_FetchAdd_int16
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchSub_int16
	.type	SCOREP_Atomic_FetchSub_int16, %function
SCOREP_Atomic_FetchSub_int16:
	sxth	w1, w1
.L303:
	ldaxrh	w2, [x0]
	sub	w3, w2, w1
	stlxrh	w4, w3, [x0]
	cbnz	w4, .L303
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_FetchSub_int16, .-SCOREP_Atomic_FetchSub_int16
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchAnd_int16
	.type	SCOREP_Atomic_FetchAnd_int16, %function
SCOREP_Atomic_FetchAnd_int16:
	sxth	w1, w1
.L305:
	ldaxrh	w2, [x0]
	and	w3, w2, w1
	stlxrh	w4, w3, [x0]
	cbnz	w4, .L305
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_FetchAnd_int16, .-SCOREP_Atomic_FetchAnd_int16
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchXor_int16
	.type	SCOREP_Atomic_FetchXor_int16, %function
SCOREP_Atomic_FetchXor_int16:
	sxth	w1, w1
.L307:
	ldaxrh	w2, [x0]
	eor	w3, w2, w1
	stlxrh	w4, w3, [x0]
	cbnz	w4, .L307
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_FetchXor_int16, .-SCOREP_Atomic_FetchXor_int16
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchOr_int16
	.type	SCOREP_Atomic_FetchOr_int16, %function
SCOREP_Atomic_FetchOr_int16:
	sxth	w1, w1
.L309:
	ldaxrh	w2, [x0]
	orr	w3, w2, w1
	stlxrh	w4, w3, [x0]
	cbnz	w4, .L309
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_FetchOr_int16, .-SCOREP_Atomic_FetchOr_int16
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchNand_int16
	.type	SCOREP_Atomic_FetchNand_int16, %function
SCOREP_Atomic_FetchNand_int16:
	sxth	w1, w1
.L311:
	ldaxrh	w2, [x0]
	and	w3, w2, w1
	mvn	w3, w3
	stlxrh	w4, w3, [x0]
	cbnz	w4, .L311
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_FetchNand_int16, .-SCOREP_Atomic_FetchNand_int16
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_LoadN_int32
	.type	SCOREP_Atomic_LoadN_int32, %function
SCOREP_Atomic_LoadN_int32:
	ldar	w0, [x0]
	ret
	.size	SCOREP_Atomic_LoadN_int32, .-SCOREP_Atomic_LoadN_int32
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_StoreN_int32
	.type	SCOREP_Atomic_StoreN_int32, %function
SCOREP_Atomic_StoreN_int32:
	stlr	w1, [x0]
	ret
	.size	SCOREP_Atomic_StoreN_int32, .-SCOREP_Atomic_StoreN_int32
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_ExchangeN_int32
	.type	SCOREP_Atomic_ExchangeN_int32, %function
SCOREP_Atomic_ExchangeN_int32:
.L315:
	ldaxr	w2, [x0]
	stlxr	w3, w1, [x0]
	cbnz	w3, .L315
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_ExchangeN_int32, .-SCOREP_Atomic_ExchangeN_int32
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_CompareExchangeN_int32
	.type	SCOREP_Atomic_CompareExchangeN_int32, %function
SCOREP_Atomic_CompareExchangeN_int32:
	ldr	w3, [x1]
.L318:
	ldaxr	w4, [x0]
	cmp	w4, w3
	bne	.L319
	stlxr	w5, w2, [x0]
	cbnz	w5, .L318
.L319:
	cset	w0, eq
	cbz	w0, .L320
	ret
	.p2align 3
.L320:
	str	w4, [x1]
	ret
	.size	SCOREP_Atomic_CompareExchangeN_int32, .-SCOREP_Atomic_CompareExchangeN_int32
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_AddFetch_int32
	.type	SCOREP_Atomic_AddFetch_int32, %function
SCOREP_Atomic_AddFetch_int32:
.L322:
	ldaxr	w2, [x0]
	add	w2, w2, w1
	stlxr	w3, w2, [x0]
	cbnz	w3, .L322
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_AddFetch_int32, .-SCOREP_Atomic_AddFetch_int32
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_SubFetch_int32
	.type	SCOREP_Atomic_SubFetch_int32, %function
SCOREP_Atomic_SubFetch_int32:
.L324:
	ldaxr	w2, [x0]
	sub	w2, w2, w1
	stlxr	w3, w2, [x0]
	cbnz	w3, .L324
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_SubFetch_int32, .-SCOREP_Atomic_SubFetch_int32
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_AndFetch_int32
	.type	SCOREP_Atomic_AndFetch_int32, %function
SCOREP_Atomic_AndFetch_int32:
.L326:
	ldaxr	w2, [x0]
	and	w2, w2, w1
	stlxr	w3, w2, [x0]
	cbnz	w3, .L326
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_AndFetch_int32, .-SCOREP_Atomic_AndFetch_int32
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_XorFetch_int32
	.type	SCOREP_Atomic_XorFetch_int32, %function
SCOREP_Atomic_XorFetch_int32:
.L328:
	ldaxr	w2, [x0]
	eor	w2, w2, w1
	stlxr	w3, w2, [x0]
	cbnz	w3, .L328
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_XorFetch_int32, .-SCOREP_Atomic_XorFetch_int32
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_OrFetch_int32
	.type	SCOREP_Atomic_OrFetch_int32, %function
SCOREP_Atomic_OrFetch_int32:
.L330:
	ldaxr	w2, [x0]
	orr	w2, w2, w1
	stlxr	w3, w2, [x0]
	cbnz	w3, .L330
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_OrFetch_int32, .-SCOREP_Atomic_OrFetch_int32
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_NandFetch_int32
	.type	SCOREP_Atomic_NandFetch_int32, %function
SCOREP_Atomic_NandFetch_int32:
.L332:
	ldaxr	w2, [x0]
	and	w2, w2, w1
	mvn	w2, w2
	stlxr	w3, w2, [x0]
	cbnz	w3, .L332
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_NandFetch_int32, .-SCOREP_Atomic_NandFetch_int32
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchAdd_int32
	.type	SCOREP_Atomic_FetchAdd_int32, %function
SCOREP_Atomic_FetchAdd_int32:
.L334:
	ldaxr	w2, [x0]
	add	w3, w2, w1
	stlxr	w4, w3, [x0]
	cbnz	w4, .L334
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_FetchAdd_int32, .-SCOREP_Atomic_FetchAdd_int32
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchSub_int32
	.type	SCOREP_Atomic_FetchSub_int32, %function
SCOREP_Atomic_FetchSub_int32:
.L336:
	ldaxr	w2, [x0]
	sub	w3, w2, w1
	stlxr	w4, w3, [x0]
	cbnz	w4, .L336
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_FetchSub_int32, .-SCOREP_Atomic_FetchSub_int32
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchAnd_int32
	.type	SCOREP_Atomic_FetchAnd_int32, %function
SCOREP_Atomic_FetchAnd_int32:
.L338:
	ldaxr	w2, [x0]
	and	w3, w2, w1
	stlxr	w4, w3, [x0]
	cbnz	w4, .L338
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_FetchAnd_int32, .-SCOREP_Atomic_FetchAnd_int32
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchXor_int32
	.type	SCOREP_Atomic_FetchXor_int32, %function
SCOREP_Atomic_FetchXor_int32:
.L340:
	ldaxr	w2, [x0]
	eor	w3, w2, w1
	stlxr	w4, w3, [x0]
	cbnz	w4, .L340
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_FetchXor_int32, .-SCOREP_Atomic_FetchXor_int32
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchOr_int32
	.type	SCOREP_Atomic_FetchOr_int32, %function
SCOREP_Atomic_FetchOr_int32:
.L342:
	ldaxr	w2, [x0]
	orr	w3, w2, w1
	stlxr	w4, w3, [x0]
	cbnz	w4, .L342
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_FetchOr_int32, .-SCOREP_Atomic_FetchOr_int32
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchNand_int32
	.type	SCOREP_Atomic_FetchNand_int32, %function
SCOREP_Atomic_FetchNand_int32:
.L344:
	ldaxr	w2, [x0]
	and	w3, w2, w1
	mvn	w3, w3
	stlxr	w4, w3, [x0]
	cbnz	w4, .L344
	mov	w0, w2
	ret
	.size	SCOREP_Atomic_FetchNand_int32, .-SCOREP_Atomic_FetchNand_int32
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_LoadN_int64
	.type	SCOREP_Atomic_LoadN_int64, %function
SCOREP_Atomic_LoadN_int64:
	ldar	x0, [x0]
	ret
	.size	SCOREP_Atomic_LoadN_int64, .-SCOREP_Atomic_LoadN_int64
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_StoreN_int64
	.type	SCOREP_Atomic_StoreN_int64, %function
SCOREP_Atomic_StoreN_int64:
	stlr	x1, [x0]
	ret
	.size	SCOREP_Atomic_StoreN_int64, .-SCOREP_Atomic_StoreN_int64
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_ExchangeN_int64
	.type	SCOREP_Atomic_ExchangeN_int64, %function
SCOREP_Atomic_ExchangeN_int64:
.L348:
	ldaxr	x2, [x0]
	stlxr	w3, x1, [x0]
	cbnz	w3, .L348
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_ExchangeN_int64, .-SCOREP_Atomic_ExchangeN_int64
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_CompareExchangeN_int64
	.type	SCOREP_Atomic_CompareExchangeN_int64, %function
SCOREP_Atomic_CompareExchangeN_int64:
	ldr	x3, [x1]
.L351:
	ldaxr	x4, [x0]
	cmp	x4, x3
	bne	.L352
	stlxr	w5, x2, [x0]
	cbnz	w5, .L351
.L352:
	cset	w0, eq
	cbz	w0, .L353
	ret
	.p2align 3
.L353:
	str	x4, [x1]
	ret
	.size	SCOREP_Atomic_CompareExchangeN_int64, .-SCOREP_Atomic_CompareExchangeN_int64
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_AddFetch_int64
	.type	SCOREP_Atomic_AddFetch_int64, %function
SCOREP_Atomic_AddFetch_int64:
.L355:
	ldaxr	x2, [x0]
	add	x2, x2, x1
	stlxr	w3, x2, [x0]
	cbnz	w3, .L355
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_AddFetch_int64, .-SCOREP_Atomic_AddFetch_int64
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_SubFetch_int64
	.type	SCOREP_Atomic_SubFetch_int64, %function
SCOREP_Atomic_SubFetch_int64:
.L357:
	ldaxr	x2, [x0]
	sub	x2, x2, x1
	stlxr	w3, x2, [x0]
	cbnz	w3, .L357
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_SubFetch_int64, .-SCOREP_Atomic_SubFetch_int64
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_AndFetch_int64
	.type	SCOREP_Atomic_AndFetch_int64, %function
SCOREP_Atomic_AndFetch_int64:
.L359:
	ldaxr	x2, [x0]
	and	x2, x2, x1
	stlxr	w3, x2, [x0]
	cbnz	w3, .L359
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_AndFetch_int64, .-SCOREP_Atomic_AndFetch_int64
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_XorFetch_int64
	.type	SCOREP_Atomic_XorFetch_int64, %function
SCOREP_Atomic_XorFetch_int64:
.L361:
	ldaxr	x2, [x0]
	eor	x2, x2, x1
	stlxr	w3, x2, [x0]
	cbnz	w3, .L361
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_XorFetch_int64, .-SCOREP_Atomic_XorFetch_int64
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_OrFetch_int64
	.type	SCOREP_Atomic_OrFetch_int64, %function
SCOREP_Atomic_OrFetch_int64:
.L363:
	ldaxr	x2, [x0]
	orr	x2, x2, x1
	stlxr	w3, x2, [x0]
	cbnz	w3, .L363
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_OrFetch_int64, .-SCOREP_Atomic_OrFetch_int64
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_NandFetch_int64
	.type	SCOREP_Atomic_NandFetch_int64, %function
SCOREP_Atomic_NandFetch_int64:
.L365:
	ldaxr	x2, [x0]
	and	x2, x2, x1
	mvn	x2, x2
	stlxr	w3, x2, [x0]
	cbnz	w3, .L365
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_NandFetch_int64, .-SCOREP_Atomic_NandFetch_int64
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchAdd_int64
	.type	SCOREP_Atomic_FetchAdd_int64, %function
SCOREP_Atomic_FetchAdd_int64:
.L367:
	ldaxr	x2, [x0]
	add	x3, x2, x1
	stlxr	w4, x3, [x0]
	cbnz	w4, .L367
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_FetchAdd_int64, .-SCOREP_Atomic_FetchAdd_int64
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchSub_int64
	.type	SCOREP_Atomic_FetchSub_int64, %function
SCOREP_Atomic_FetchSub_int64:
.L369:
	ldaxr	x2, [x0]
	sub	x3, x2, x1
	stlxr	w4, x3, [x0]
	cbnz	w4, .L369
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_FetchSub_int64, .-SCOREP_Atomic_FetchSub_int64
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchAnd_int64
	.type	SCOREP_Atomic_FetchAnd_int64, %function
SCOREP_Atomic_FetchAnd_int64:
.L371:
	ldaxr	x2, [x0]
	and	x3, x2, x1
	stlxr	w4, x3, [x0]
	cbnz	w4, .L371
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_FetchAnd_int64, .-SCOREP_Atomic_FetchAnd_int64
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchXor_int64
	.type	SCOREP_Atomic_FetchXor_int64, %function
SCOREP_Atomic_FetchXor_int64:
.L373:
	ldaxr	x2, [x0]
	eor	x3, x2, x1
	stlxr	w4, x3, [x0]
	cbnz	w4, .L373
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_FetchXor_int64, .-SCOREP_Atomic_FetchXor_int64
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchOr_int64
	.type	SCOREP_Atomic_FetchOr_int64, %function
SCOREP_Atomic_FetchOr_int64:
.L375:
	ldaxr	x2, [x0]
	orr	x3, x2, x1
	stlxr	w4, x3, [x0]
	cbnz	w4, .L375
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_FetchOr_int64, .-SCOREP_Atomic_FetchOr_int64
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchNand_int64
	.type	SCOREP_Atomic_FetchNand_int64, %function
SCOREP_Atomic_FetchNand_int64:
.L377:
	ldaxr	x2, [x0]
	and	x3, x2, x1
	mvn	x3, x3
	stlxr	w4, x3, [x0]
	cbnz	w4, .L377
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_FetchNand_int64, .-SCOREP_Atomic_FetchNand_int64
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_LoadN_intptr
	.type	SCOREP_Atomic_LoadN_intptr, %function
SCOREP_Atomic_LoadN_intptr:
	ldar	x0, [x0]
	ret
	.size	SCOREP_Atomic_LoadN_intptr, .-SCOREP_Atomic_LoadN_intptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_StoreN_intptr
	.type	SCOREP_Atomic_StoreN_intptr, %function
SCOREP_Atomic_StoreN_intptr:
	stlr	x1, [x0]
	ret
	.size	SCOREP_Atomic_StoreN_intptr, .-SCOREP_Atomic_StoreN_intptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_ExchangeN_intptr
	.type	SCOREP_Atomic_ExchangeN_intptr, %function
SCOREP_Atomic_ExchangeN_intptr:
.L381:
	ldaxr	x2, [x0]
	stlxr	w3, x1, [x0]
	cbnz	w3, .L381
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_ExchangeN_intptr, .-SCOREP_Atomic_ExchangeN_intptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_CompareExchangeN_intptr
	.type	SCOREP_Atomic_CompareExchangeN_intptr, %function
SCOREP_Atomic_CompareExchangeN_intptr:
	ldr	x3, [x1]
.L384:
	ldaxr	x4, [x0]
	cmp	x4, x3
	bne	.L385
	stlxr	w5, x2, [x0]
	cbnz	w5, .L384
.L385:
	cset	w0, eq
	cbz	w0, .L386
	ret
	.p2align 3
.L386:
	str	x4, [x1]
	ret
	.size	SCOREP_Atomic_CompareExchangeN_intptr, .-SCOREP_Atomic_CompareExchangeN_intptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_AddFetch_intptr
	.type	SCOREP_Atomic_AddFetch_intptr, %function
SCOREP_Atomic_AddFetch_intptr:
.L388:
	ldaxr	x2, [x0]
	add	x2, x2, x1
	stlxr	w3, x2, [x0]
	cbnz	w3, .L388
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_AddFetch_intptr, .-SCOREP_Atomic_AddFetch_intptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_SubFetch_intptr
	.type	SCOREP_Atomic_SubFetch_intptr, %function
SCOREP_Atomic_SubFetch_intptr:
.L390:
	ldaxr	x2, [x0]
	sub	x2, x2, x1
	stlxr	w3, x2, [x0]
	cbnz	w3, .L390
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_SubFetch_intptr, .-SCOREP_Atomic_SubFetch_intptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_AndFetch_intptr
	.type	SCOREP_Atomic_AndFetch_intptr, %function
SCOREP_Atomic_AndFetch_intptr:
.L392:
	ldaxr	x2, [x0]
	and	x2, x2, x1
	stlxr	w3, x2, [x0]
	cbnz	w3, .L392
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_AndFetch_intptr, .-SCOREP_Atomic_AndFetch_intptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_XorFetch_intptr
	.type	SCOREP_Atomic_XorFetch_intptr, %function
SCOREP_Atomic_XorFetch_intptr:
.L394:
	ldaxr	x2, [x0]
	eor	x2, x2, x1
	stlxr	w3, x2, [x0]
	cbnz	w3, .L394
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_XorFetch_intptr, .-SCOREP_Atomic_XorFetch_intptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_OrFetch_intptr
	.type	SCOREP_Atomic_OrFetch_intptr, %function
SCOREP_Atomic_OrFetch_intptr:
.L396:
	ldaxr	x2, [x0]
	orr	x2, x2, x1
	stlxr	w3, x2, [x0]
	cbnz	w3, .L396
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_OrFetch_intptr, .-SCOREP_Atomic_OrFetch_intptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_NandFetch_intptr
	.type	SCOREP_Atomic_NandFetch_intptr, %function
SCOREP_Atomic_NandFetch_intptr:
.L398:
	ldaxr	x2, [x0]
	and	x2, x2, x1
	mvn	x2, x2
	stlxr	w3, x2, [x0]
	cbnz	w3, .L398
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_NandFetch_intptr, .-SCOREP_Atomic_NandFetch_intptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchAdd_intptr
	.type	SCOREP_Atomic_FetchAdd_intptr, %function
SCOREP_Atomic_FetchAdd_intptr:
.L400:
	ldaxr	x2, [x0]
	add	x3, x2, x1
	stlxr	w4, x3, [x0]
	cbnz	w4, .L400
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_FetchAdd_intptr, .-SCOREP_Atomic_FetchAdd_intptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchSub_intptr
	.type	SCOREP_Atomic_FetchSub_intptr, %function
SCOREP_Atomic_FetchSub_intptr:
.L402:
	ldaxr	x2, [x0]
	sub	x3, x2, x1
	stlxr	w4, x3, [x0]
	cbnz	w4, .L402
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_FetchSub_intptr, .-SCOREP_Atomic_FetchSub_intptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchAnd_intptr
	.type	SCOREP_Atomic_FetchAnd_intptr, %function
SCOREP_Atomic_FetchAnd_intptr:
.L404:
	ldaxr	x2, [x0]
	and	x3, x2, x1
	stlxr	w4, x3, [x0]
	cbnz	w4, .L404
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_FetchAnd_intptr, .-SCOREP_Atomic_FetchAnd_intptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchXor_intptr
	.type	SCOREP_Atomic_FetchXor_intptr, %function
SCOREP_Atomic_FetchXor_intptr:
.L406:
	ldaxr	x2, [x0]
	eor	x3, x2, x1
	stlxr	w4, x3, [x0]
	cbnz	w4, .L406
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_FetchXor_intptr, .-SCOREP_Atomic_FetchXor_intptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchOr_intptr
	.type	SCOREP_Atomic_FetchOr_intptr, %function
SCOREP_Atomic_FetchOr_intptr:
.L408:
	ldaxr	x2, [x0]
	orr	x3, x2, x1
	stlxr	w4, x3, [x0]
	cbnz	w4, .L408
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_FetchOr_intptr, .-SCOREP_Atomic_FetchOr_intptr
	.align	2
	.p2align 3,,7
	.global	SCOREP_Atomic_FetchNand_intptr
	.type	SCOREP_Atomic_FetchNand_intptr, %function
SCOREP_Atomic_FetchNand_intptr:
.L410:
	ldaxr	x2, [x0]
	and	x3, x2, x1
	mvn	x3, x3
	stlxr	w4, x3, [x0]
	cbnz	w4, .L410
	mov	x0, x2
	ret
	.size	SCOREP_Atomic_FetchNand_intptr, .-SCOREP_Atomic_FetchNand_intptr
	.ident	"GCC: (Ubuntu/Linaro 5.4.0-6ubuntu1~16.04.4) 5.4.0 20160609"
	.section	.note.GNU-stack,"",@progbits
