CFLAGS=`pkg-config hildon-1 sqlite3 --cflags --libs`

all:
	$(CC) -Wall -pedantic \
		eom.c \
		interface.c \
		eom-main-window.c \
		eom-edit-window.c \
		eom-new-item-dialog.c \
		data.c \
		$(CFLAGS) \
		-o eye-on-manga

.PHONY: check-syntax
check-syntax:
	/scratchbox/login $(CC) -pedantic -Wall -Wextra -fsyntax-only \
		`readlink -f $(CHK_SOURCES)` \
		`/scratchbox/login pkg-config hildon-1 sqlite3 --cflags --libs`
