#!/bin/sh

# Copyright (c) 2013, Ben Noordhuis <info@bnoordhuis.nl>
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

if test -z "$LIBTOOLIZE" -a "`uname`" = "Darwin"; then
	if command -v "libtoolize" >/dev/null; then
		LIBTOOLIZE=libtoolize
	elif command -v "glibtoolize" >/dev/null; then
		LIBTOOLIZE=glibtoolize
	else
		echo "autogen.sh: line $LINENO: command glibtoolize or libtoolize not found"
		exit 1
	fi
fi

set -ex
${LIBTOOLIZE:-libtoolize}
${ACLOCAL:-aclocal -I m4}
${AUTOCONF:-autoconf}
${AUTOMAKE:-automake} --add-missing
