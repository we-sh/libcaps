/* ************************************************************************** */
/*																			  */
/*														  :::	   ::::::::	  */
/*	 caps__initialize.c									:+:		 :+:	:+:	  */
/*													  +:+ +:+		  +:+	  */
/*	 By: abombard <marvin@42.fr>					+#+	 +:+	   +#+		  */
/*												  +#+#+#+#+#+	+#+			  */
/*	 Created: 2016/04/01 17:20:28 by abombard		   #+#	  #+#			  */
/*	 Updated: 2016/04/01 17:25:07 by abombard		  ###	########.fr		  */
/*																			  */
/* ************************************************************************** */

#include "internal_caps.h"
#include "types.h"

#include "libft.h"

#include <stdlib.h>
#include <termcap.h>

static bool	internal_caps__initialize_caps(t_internal_context *caps)
{
	char	*capcodes[] = {
		"nd", "le", "up", "do", "dc", "ce", "cd", "cr", "bc"
	};
	size_t	i;

	caps->caps_size = sizeof(caps->caps) / sizeof(caps->caps[0]);
	i = 0;
	while (i < caps->caps_size)
	{
		caps->caps[i] = tgetstr(capcodes[i], &caps->buffaddr);
		if (i != caps->caps_size - 1 && !caps->caps[i])
		{
			log_error("tgetstr() failed on %s", capcodes[i]);
		}
		else if (i == caps->caps_size - 1)
		{
			caps->caps[i] = "\b";
		}
		i++;
	}
	return (TRUE);
}

static bool	internal_caps__initialize_keycodes(t_internal_context *caps)
{
	char		*keycode;
	size_t		i;

	caps->map_size = sizeof(caps->map) / sizeof(caps->map[0]);
	i = 0;
	while (i < caps->map_size)
	{
		if (caps->map[i].tcapcode)
		{
			keycode = tgetstr(caps->map[i].tcapcode, &caps->buffaddr);
			if (keycode)
			{
				caps->map[i].keycode.bytes = keycode;
				caps->map[i].keycode.size = ft_strlen(keycode);
				//log_debug("Successfully initialized %s", caps->map[i].description);
			}
			//else
			//LOG_WARNING("Could not find %s", caps->map[i].description);
		}
		i++;
	}
	return (TRUE);
}

#include <termios.h>
static bool	internal_caps__tgetent(t_internal_context *caps)
{
	char		*termtype;
	char		*temp;

	termtype = CAPS__TERMTYPE;
	if (!termtype)
	{
		log_fatal("getenv(\"TERM\") failed %s", "");
		return (0);
	}
	caps->termtype = termtype;
	if (!tgetent(caps->termbuffer, termtype))
	{
		log_fatal("tgetent() failed termtype %s", termtype);
		return (0);
	}
	caps->buffaddr = caps->termbuffer;
	/* For termcaps functions */
	temp = tgetstr("pc", &caps->buffaddr);
	PC = temp ? *temp : 0;
	BC = tgetstr("le", &caps->buffaddr);
	UP = tgetstr("up", &caps->buffaddr);
	return (TRUE);
}

bool		caps__initialize(const int fd)
{
	t_internal_context	*caps;

	caps__get_context(&caps);
	caps->fd = fd;
	if (!internal_caps__tgetent(caps))
	{
		log_fatal("interal_caps__tgetent() failed %s", "");
		return (0);
	}
	if (!internal_caps__initialize_keycodes(caps))
	{
		log_fatal("internal_caps__initialize_keycodes() failed %s", "");
		return (0);
	}
	if (!internal_caps__initialize_caps(caps))
	{
		log_fatal("internal_caps__initialize_caps() failed %s", "");
		return (0);
	}
	return (TRUE);
}
