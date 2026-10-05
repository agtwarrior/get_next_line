*This project has been created as part of the 42 curriculum by davguerr.*

# get_next_line

## Description

`get_next_line` is a C function that returns one line read from a file
descriptor. Calling it repeatedly (in a loop, for example) reads the whole
content of a file, or of standard input, one line at a time and without loading
it all into memory.

```c
char	*get_next_line(int fd);
```

| | |
|---|---|
| **Parameter** | `fd`: the file descriptor to read from |
| **Return value** | The line that was read, or `NULL` if there is nothing left to read or an error occurs |
| **External functions** | `read`, `malloc`, `free` |

The returned line includes the trailing `\n`, except when the end of the file
is reached and the file does not end with a newline. Each line is allocated
with `malloc`, so the caller is responsible for releasing it with `free`.

The goal of the project is to learn how **static variables** work in C: since
`read` can return more bytes than one line takes, whatever is left over has to
be kept somewhere between one call and the next.

### Files

| File | Content |
|---|---|
| `get_next_line.c` | `get_next_line` and its three static helper functions |
| `get_next_line_utils.c` | Support functions: `ft_strchr`, `ft_strjoin`, `ft_strlen`, `ft_memcpy` |
| `get_next_line.h` | Prototypes and the default value of `BUFFER_SIZE` |

This repository contains only the mandatory part, so the function handles a
single file descriptor at a time.

## Instructions

### Compilation

The project is a function, not a program: it has no `Makefile` and no `main`.
To use it, compile its two source files together with the program that calls
it:

```sh
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 get_next_line.c get_next_line_utils.c main.c
```

`BUFFER_SIZE` sets how many bytes are requested from `read` on each call. The
`-D BUFFER_SIZE=n` flag is optional: without it, the default value defined in
`get_next_line.h` is used, which is `42`.

```sh
cc -Wall -Wextra -Werror get_next_line.c get_next_line_utils.c main.c
```

If `BUFFER_SIZE` is less than or equal to 0, or if the descriptor is negative,
the function returns `NULL`.

### Usage example

```c
#include <fcntl.h>
#include <stdio.h>
#include "get_next_line.h"

int	main(int argc, char **argv)
{
	char	*line;
	int		fd;

	fd = 0;
	if (argc > 1)
		fd = open(argv[1], O_RDONLY);
	if (fd < 0)
		return (1);
	line = get_next_line(fd);
	while (line)
	{
		printf("%s", line);
		free(line);
		line = get_next_line(fd);
	}
	if (fd > 0)
		close(fd);
	return (0);
}
```

```sh
./a.out file.txt           # reads from a file
echo "hello" | ./a.out     # reads from standard input
```

## Algorithm

### Explanation

The function keeps, in a single static variable called `str`, everything that
has already been read from the descriptor but not yet returned. Each call runs
three steps, each in its own function:

1. **Read (`read_to_str`).** A temporary buffer of `BUFFER_SIZE + 1` bytes is
   allocated and `read` is called in a loop. What is read gets appended to
   `str` with `ft_strjoin`, which builds the new string and frees the old one.
   The loop stops as soon as `str` contains a `\n` or `read` returns 0 (end of
   file). If `str` already held a `\n` from the previous call, nothing is read.

2. **Extract (`str_to_line`).** `str` is scanned up to the first `\n` and that
   stretch, newline included, is copied into a new string: this is the line
   that gets returned. If there is no `\n`, the end of the file has been
   reached and everything left is copied. If `str` is empty, `NULL` is
   returned.

3. **Clean (`clean_str`).** Whatever comes after the `\n` is copied into a new
   string, the old one is freed, and the result becomes the new value of
   `str`, ready for the next call. If nothing follows the `\n`, `str` is freed
   and set to `NULL`.

For example, with `BUFFER_SIZE=5` and a file containing `"hello\nworld\n"`:

| Call | `read` results | `str` after reading | Returned line | `str` at the end |
|---|---|---|---|---|
| 1 | `"hello"`, `"\nworl"` | `"hello\nworl"` | `"hello\n"` | `"worl"` |
| 2 | `"d\n"` | `"world\n"` | `"world\n"` | `NULL` |
| 3 | 0 bytes | `NULL` | `NULL` | `NULL` |

On any error (`read` returns -1 or a `malloc` fails) all allocated memory is
freed, `str` goes back to `NULL` and the function returns `NULL`. Once the end
of the file is reached, the function holds no allocated memory either.

### Justification

- **It reads as little as possible.** The read loop checks for a `\n` before
  each `read`, so the file is never read in full and split afterwards. This is
  what makes it work on standard input, where data arrives a bit at a time.
- **It works with any `BUFFER_SIZE`.** Because what is read accumulates in
  `str`, it does not matter whether the line is longer than the buffer
  (`BUFFER_SIZE=1`) or the buffer holds many lines (`BUFFER_SIZE=9999`).
- **The buffer lives on the heap.** A local array of `BUFFER_SIZE` bytes would
  overflow the stack with values such as `10000000`. With `malloc`, if there is
  not enough memory the function returns `NULL` instead of crashing with a
  segmentation fault.
- **A single static variable.** All the state kept between calls is one
  pointer, which makes it easy to reason about who owns each block of memory:
  `ft_strjoin` and `clean_str` always free the old string when replacing it.
- **Three separate steps.** Reading, extracting and cleaning are short,
  independent functions that are easy to explain and to modify, and that
  comply with the Norm.

### Limitations

- On every iteration of the loop, `str` is scanned again for a `\n` and copied
  in full when appending. With very long lines and a very small `BUFFER_SIZE`
  the cost grows quadratically. Simpler code was preferred over that
  optimisation.
- Only one descriptor is handled at a time. If calls alternate between
  different descriptors, the leftovers of one get mixed with the other's.
- If the function stops being called before the end of the file, whatever is
  still stored in `str` is not freed.
- Content is treated as `\0`-terminated strings, so behaviour on binary files
  is undefined, as the subject allows.

## Resources

### References

- [`read(2)`](https://man7.org/linux/man-pages/man2/read.2.html): manual page
  of the `read` system call.
- [`open(2)`](https://man7.org/linux/man-pages/man2/open.2.html): file
  descriptors and how they are obtained.
- [`malloc(3)`](https://man7.org/linux/man-pages/man3/malloc.3.html): dynamic
  memory management with `malloc` and `free`.
- [Static variables in C (GeeksforGeeks)](https://www.geeksforgeeks.org/c/static-variables-in-c/):
  what a static variable is and how long it lives.
- [Storage-class specifiers (cppreference)](https://en.cppreference.com/w/c/language/storage_duration):
  storage duration and linkage in C.
- [File descriptor (Wikipedia)](https://en.wikipedia.org/wiki/File_descriptor):
  what a file descriptor is.
- [Valgrind Quick Start](https://valgrind.org/docs/manual/quick-start.html):
  how to check for memory leaks.

### AI usage

An AI assistant (Claude) was used to write this `README.md` from the finished
code and the project subject.
