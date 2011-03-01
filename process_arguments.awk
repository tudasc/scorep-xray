# We assume that the order of the passed files is
# - platform defautls
# - exported symbols
# - user-specified arguments
#
# iterate over the fields. if a field is of the form "key=value", store it in
# a map<key, value>. subsequent insertions of the same key override the old
# value, e.g. the last insertion wins.
# 
# fields not in "key=value" form are treated as unary arguments and are taken
# into account for the user-provided toplevel arguments only.

{
  if (FILENAME == "user_provided_configure_args") {
    gsub("\'", "")
    if ($0 == "") {
      next
    }
    n = index($0, "=")
    if (n != 0) { # line with at least one "=". Use first "=" as key-value separator
      args_binary[substr($0, 0, n-1)] = substr($0, n+1)
    }    
    else {
      args_unary = "'" $0 "' " args_unary
    }
  }
#   else if (FILENAME == "args_exported") {
#     # HUH, it seems that exported values doesn't make it into configure
#     # if I do a export CC=icc; ./configure then CC always equals gcc.
#     # i.e. we can omit "args_exported"
#     # don't put every exported symbol into the map but override only those
#     # that were inserted by the platform defaults.
#     n = split($0, split_array, "=")
#     if (n == 2) {
#       key = split_array[1]
#       if (key in args_binary) { 
#         args_binary[key]=split_array[2]
#       }
#     }
#   }
  else { # FILENAME == "${ac_scorep_platform}"  
    if (index($0, "#") == 0) { # ! commented line
      n = index($0, "=")
      if (n != 0) { # line with at least one "=". Use first "=" as key-value separator
        args_binary[substr($0, 1, n-1)] = substr($0, n+1)
      }
    }
  }
}

END{
  # concatenate the map's content into a "key=value" pair sequence, add the
  # unary arguments and print it to stdout.
  for (key in args_binary) {
    result = "'" key "=" args_binary[key] "' " result
  }
  print result args_unary
}
