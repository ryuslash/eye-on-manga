CFLAGS=`pkg-config hildon-1 sqlite3 --cflags --libs`

all:
	$(CC) -Wall -Wextra -pedantic \
		collections.c \
		interface.c \
		c-main-window.c \
		c-new-item-dialog.c \
		data.c \
		$(CFLAGS) \
		-o collections

.PHONY: check-syntax
check-syntax:
	/scratchbox/login $(CC) -pedantic -Wall -Wextra -fsyntax-only \
		`readlink -f $(CHK_SOURCES)` \
		`/scratchbox/login pkg-config hildon-1 sqlite3 --cflags --libs`
