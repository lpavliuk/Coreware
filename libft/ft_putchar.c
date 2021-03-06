/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putchar.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opavliuk <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/21 12:44:20 by opavliuk          #+#    #+#             */
/*   Updated: 2018/04/26 13:20:56 by opavliuk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_putchar(int c)
{
	unsigned char	code[4];

	if (c <= 127)
		write(1, &c, 1);
	else if (c <= 2047)
	{
		code[0] = 192 | ((c >> 6) & 31);
		code[1] = 128 | (c & 63);
		write(1, code, 2);
	}
	else if (c <= 65535)
	{
		code[0] = 224 | ((c >> 12) & 15);
		code[1] = 128 | ((c >> 6) & 63);
		code[2] = 128 | (c & 63);
		write(1, code, 3);
	}
	else if (c <= 1114111)
	{
		code[0] = 240 | ((c >> 18) & 7);
		code[1] = 128 | ((c >> 12) & 63);
		code[2] = 128 | ((c >> 6) & 63);
		code[3] = 128 | (c & 63);
		write(1, code, 4);
	}
}
