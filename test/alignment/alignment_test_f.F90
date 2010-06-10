program MAIN
	integer( kind=8 ) foo
	integer( kind=8 ) bar
	integer( kind=8 ) foobar
	integer( kind=8 ) baz

	common /foo/ foo
	common /bar/ bar
	common /foobar/ foobar
	common /baz/ baz

	write (*,*) "foo    = ", foo
	write (*,*) "bar    = ", bar
	write (*,*) "foobar = ", foobar
	write (*,*) "baz    = ", baz
end program MAIN
