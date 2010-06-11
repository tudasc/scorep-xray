program MAIN
	integer( kind=8 ) foo
	integer( kind=8 ) bar
	integer( kind=8 ) foobar
	integer( kind=8 ) baz
	integer( kind=8 ) foobaz

	common /foo/ foo
	common /bar/ bar
	common /foobar/ foobar
	common /baz/ baz
	common /foobaz/ foobaz

	write (*,*) "foo    = ", foo
	write (*,*) "bar    = ", bar
	write (*,*) "foobar = ", foobar
	write (*,*) "baz    = ", baz
	write (*,*) "foobaz = ", foobaz

	foo = foo - 1
	bar = bar + 1
	foobar = foobar - 1
	baz = baz - 43
	foobaz = foobaz + 43

	call check()
end program MAIN
