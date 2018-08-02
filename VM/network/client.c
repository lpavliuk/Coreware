/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   corewar.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkiselev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/11 13:45:19 by tkiselev          #+#    #+#             */
/*   Updated: 2018/07/11 13:45:23 by tkiselev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../corewar.h"

static void	connect_to_server(int socket_fd, char *ip)
{
	struct sockaddr_in	address;
	struct timeval		timeout;
	fd_set				set;

	fcntl(socket_fd, F_SETFL, O_NONBLOCK);
	address.sin_family = AF_INET;
	address.sin_port = htons(PORT);
	if (!inet_aton(ip, &address.sin_addr))
		ft_error("Error: connect_to_server()");
	connect(socket_fd, (struct sockaddr *)&address, sizeof(address));
	FD_ZERO(&set);
    FD_SET(socket_fd, &set);
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;
	if (select(socket_fd + 1, &set, NULL, NULL, &timeout) != 1)	/* Checks whether we can read from this server socket or not. */
		ft_error("Error: connect");
}

static void				read_init_info(int socket_fd)
{
	char			flag_sec;
	unsigned char	str;

	flag_sec = 0;
	str = 0;
	while (1)
	{
		if (read(socket_fd, &str, sizeof(unsigned char)) > 0)
		{
			if (!flag_sec)
			{
				sleep(1);
				system("clear");
				printf("Time to start the game: %d\n", str);
				flag_sec = 1;
				if (str == 0)
					break ;
			}
			else
			{
				printf("Number of connected players: %d\n", str);
				flag_sec = 0;
			}
		}
	}
}

/*
** name | comment | size | exec
*/

static void				serialize(t_bot *bot, unsigned char *str)
{
	unsigned char size[4];

	size[0] = ((unsigned char *)&bot->size)[0];
	size[1] = ((unsigned char *)&bot->size)[1];
	size[2] = ((unsigned char *)&bot->size)[2];
	size[3] = ((unsigned char *)&bot->size)[3];
	ft_memcpy(str, bot->name, PROG_NAME_LENGTH);
	ft_memcpy(str + PROG_NAME_LENGTH, bot->comment, COMMENT_LENGTH);
	ft_memcpy(str + PROG_NAME_LENGTH + COMMENT_LENGTH, size, 4);
	ft_memcpy(str + PROG_NAME_LENGTH + COMMENT_LENGTH + 4, bot->exec, bot->size);
}

void					client(void)
{
	int				len;
	int				socket_fd;
	unsigned char	str[PROG_NAME_LENGTH + COMMENT_LENGTH + g_vm->bot->size + 4];

	len = PROG_NAME_LENGTH + COMMENT_LENGTH + g_vm->bot->size + 4;
	(g_vm->count_players != 1) ? ft_error("Error: client()") : 0;
	ft_bzero(str, len);
	serialize(g_vm->bot, str);
	socket_fd = create_socket();
	connect_to_server(socket_fd, g_vm->ip);
	send(socket_fd, "Ready to fight!", 15, 0);
	read_init_info(socket_fd);
	send(socket_fd, str, len, 0);
	
	while (1);
	close(socket_fd);
}
