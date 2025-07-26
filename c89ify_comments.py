#!/usr/bin/env python3
"""
c89ify_comments.py

Recursively convert all '//' comments in C/C++ source files to C89-style '/* ... */' comments,
while leaving existing '/* ... */' comments and string/char literals intact.
"""

import argparse
import os
import sys

def convert_line_comments(text):
    """
    Convert all // comments in the given text to /* ... */ style,
    skipping anything inside string literals, char literals, or existing block comments.
    """
    out = []
    in_str = False
    in_char = False
    in_block = False
    escape = False

    i = 0
    while i < len(text):
        # detect block-comment start/end
        if not in_str and not in_char and text[i:i+2] == '/*' and not in_block:
            in_block = True
            out.append('/*')
            i += 2
            continue
        if in_block and text[i:i+2] == '*/':
            in_block = False
            out.append('*/')
            i += 2
            continue

        # detect string/char starts/ends
        c = text[i]
        if c == '"' and not in_char and not in_block and not escape:
            in_str = not in_str
            out.append(c)
            i += 1
            continue
        if c == "'" and not in_str and not in_block and not escape:
            in_char = not in_char
            out.append(c)
            i += 1
            continue

        # track backslash escape
        if (in_str or in_char) and c == '\\' and not escape:
            escape = True
            out.append(c)
            i += 1
            continue
        if escape:
            escape = False
            out.append(c)
            i += 1
            continue

        # convert // to /* ... */
        if not in_str and not in_char and not in_block and text[i:i+2] == '//':
            # capture until end-of-line
            comment = text[i+2:]
            # strip trailing newline, we'll re-add
            if comment.endswith('\n'):
                comment_body, nl = comment[:-1], '\n'
            else:
                comment_body, nl = comment, ''
            # form replacement
            out.append('/*' + comment_body.rstrip() + ' */' + nl)
            break

        # otherwise, copy char
        out.append(c)
        i += 1

    return ''.join(out)

def process_file(path, in_place=True):
    with open(path, 'r', encoding='utf-8') as f:
        original = f.read()
    converted = ''.join(convert_line_comments(line) for line in original.splitlines(keepends=True))
    if in_place:
        with open(path, 'w', encoding='utf-8') as f:
            f.write(converted)
        print(f"[OK]  {path}")
    else:
        sys.stdout.write(converted)

def process_path(path, in_place=True, extensions=('.c', '.h', '.cpp', '.hpp')):
    if os.path.isdir(path):
        for root, _, files in os.walk(path):
            for fn in files:
                if fn.lower().endswith(extensions):
                    process_file(os.path.join(root, fn), in_place)
    else:
        process_file(path, in_place)

def main():
    p = argparse.ArgumentParser(
        description="Convert C99-style '//' comments to ANSI-C '/* ... */' in C/C++ source files.")
    p.add_argument("paths", nargs="+",
                   help="file or directory paths to process")
    p.add_argument("--dry-run", action="store_true",
                   help="print converted output to stdout instead of overwriting files")
    args = p.parse_args()

    for path in args.paths:
        if not os.path.exists(path):
            print(f"[ERROR] Path not found: {path}", file=sys.stderr)
            continue
        process_path(path, in_place=not args.dry_run)

if __name__ == "__main__":
    main()
