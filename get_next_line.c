/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: david <davguerr@student.42madrid.com>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/02 11:31:52 by david             #+#    #+#             */
/*   Updated: 2026/10/04 20:11:30 by david            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static char	*read_to_str(int fd, char *str)
{
	char	*buf;
	ssize_t	nbytes;

	buf = malloc (sizeof(char) * ((size_t)BUFFER_SIZE + 1));
	if (!buf)
		return (free(str), NULL);
	nbytes = 1;
	while (!ft_strchr(str, '\n') && nbytes > 0)
	{
		nbytes = read(fd, buf, BUFFER_SIZE);
		if (nbytes < 0)
			return (free(buf), free(str), NULL);
		buf[nbytes] = '\0';
		if (nbytes > 0)
			str = ft_strjoin(str, buf);
		if (!str)
			return (free(str), free(buf), NULL);
	}
	return (free(buf), str);
}

static char	*str_to_line(char *str)
{
	char	*line;
	size_t	i;
	size_t	j;

	if (!str || str[0] == '\0')
		return (NULL);
	i = 0;
	while (str[i] && str[i] != '\n')
	{
		i++;
	}
	if (str[i] == '\n')
		i++;
	line = malloc (sizeof(char) * (i + 1));
	if (!line)
		return (NULL);
	j = 0;
	while (j < i)
	{
		line[j] = str [j];
		j++;
	}
	line [j] = '\0';
	return (line);
}

static char	*clean_str(char *str)
{
	char	*new_str;
	size_t	i;

	if (!str)
		return (NULL);
	i = 0;
	while (str[i] && str[i] != '\n')
		i++;
	if (!str[i])
		return (free(str), NULL);
	i++;
	if (!str[i])
		return (free(str), NULL);
	new_str = ft_strjoin(NULL, &str[i]);
	if (!new_str)
		return (free(str), NULL);
	return (free (str), new_str);
}

char	*get_next_line(int fd)
{
	static char	*str;
	char		*line;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	str = read_to_str(fd, str);
	if (!str)
		return (str = NULL, NULL);
	line = str_to_line(str);
	if (!line)
		return (free(str), str = NULL, NULL);
	str = clean_str(str);
	return (line);
}
