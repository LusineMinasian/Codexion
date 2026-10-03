#include "codexion.h"
#include <errno.h>
#include <limits.h>

static int ft_is_digits(const char *s)
{
    if (s == NULL || *s == '\0')
        return (0);
    while (*s)
    {
        if (*s < '0' || *s > '9')
            return (0);
        s++;
    }
    return (1);
}

int ft_strtol_check(const char *s, long *result)
{
    long    val;
    char    *end;
    int     base;

    base = 10;
    errno = 0;
    val = strtol(s, &end, base);
    if (errno == 0)
    {
        *result = val;
        return (1);
    }
    else
    {
        return (0);
    }
}

int	parse_args(int argc, char **argv, t_sim *sim)
{
	long	val;

	if (argc != 9)
	{
		fprintf(stderr, "codexion: expected 8 arguments\n");
		return (0);
	}
	if (!ft_is_digits(argv[1]) || !ft_strtol_check(argv[1], &val) || val < 1)
	{
		fprintf(stderr, "codexion: invalid number_of_coders\n");
		return (0);
	}
	sim->number_of_coders = (int)val;
	if (!ft_is_digits(argv[2]) || !ft_strtol_check(argv[2], &val) || val < 1)
	{
		fprintf(stderr, "codexion: invalid time_to_burnout\n");
		return (0);
	}
	sim->time_to_burnout = val;
	if (!ft_is_digits(argv[3]) || !ft_strtol_check(argv[3], &val) || val < 1)
	{
		fprintf(stderr, "codexion: invalid time_to_compile\n");
		return (0);
	}
	sim->time_to_compile = val;
	if (!ft_is_digits(argv[4]) || !ft_strtol_check(argv[4], &val))
	{
		fprintf(stderr, "codexion: invalid time_to_debug\n");
		return (0);
	}
	sim->time_to_debug = val;
	if (!ft_is_digits(argv[5]) || !ft_strtol_check(argv[5], &val))
	{
		fprintf(stderr, "codexion: invalid time_to_refactor\n");
		return (0);
	}
	sim->time_to_refactor = val;
	if (!ft_is_digits(argv[6]) || !ft_strtol_check(argv[6], &val))
	{
		fprintf(stderr, "codexion: invalid number_of_compiles_required\n");
		return (0);
	}
	sim->number_of_compiles_required = (int)val;
	if (!ft_is_digits(argv[7]) || !ft_strtol_check(argv[7], &val))
	{
		fprintf(stderr, "codexion: invalid dongle_cooldown\n");
		return (0);
	}
	sim->dongle_cooldown = val;
	if (strcmp(argv[8], "fifo") == 0)
		sim->scheduler = SCHED_FIFO;
	else if (strcmp(argv[8], "edf") == 0)
		sim->scheduler = SCHEDULER_EDF;
	else
	{
		fprintf(stderr, "codexion: scheduler must be 'fifo' or 'edf'\n");
		return (0);
	}
	return (1);
}