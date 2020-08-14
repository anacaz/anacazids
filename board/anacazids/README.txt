NOTES:
-----

1) The menus for the register tests are made from the Makefile(s) in the
subordinate directories.  Each Makefile has targets that execute the 'mktest'
command.  This is where the menus are created.

2) The underlying tests are the 'reg-[rw | ro | wo | rc | sc]' tests that are
compiled with the target.  This is the test that gets called from the RUN
menu item created by the menu builder in menu.c when the tls/ids headers
are parsed.

3) Do not make the 'reg-name=value' longer than 31 characters.  mktest will
flag any arguments that exceed 31 characters as an error.
