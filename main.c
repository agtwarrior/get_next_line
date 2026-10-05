/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: david <davguerr@student.42madrid.com>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/05 17:18:53 by david             #+#    #+#             */
/*   Updated: 2026/10/05 17:18:54 by david            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <stdio.h>
#include "get_next_line.h"

/* Waits until Enter is pressed. Returns 0 if stdin is closed (Ctrl+D). */
static int	wait_enter(void)
{
	char	c;

	c = '\0';
	while (c != '\n')
	{
		if (read(0, &c, 1) <= 0)
			return (0);
	}
	return (1);
}

/* Shows the line without its final '\n': the Enter key adds it on screen. */
static void	show_line(char *line)
{
	size_t	len;

	len = 0;
	while (line[len] && line[len] != '\n')
		len++;
	write(1, line, len);
}

int	main(int argc, char **argv)
{
	char	*line;
	int		fd;

	if (argc != 2)
		return (printf("Usage: %s <file>\n", argv[0]), 1);
	fd = open(argv[1], O_RDONLY);
	if (fd < 0)
		return (printf("Error: cannot open %s\n", argv[1]), 1);
	line = get_next_line(fd);
	while (line)
	{
		show_line(line);
		free(line);
		if (!wait_enter())
			break ;
		line = get_next_line(fd);
	}
	close(fd);
	printf("-- end --\n");
	return (0);
}
