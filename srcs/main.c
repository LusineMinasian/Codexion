#include "codexion.h"

int	main(int argc, char **argv)
{
    t_sim   sim;

    if (!parse_args(argc, argv, &sim))
        return (1);
    printf("number_of_coders = %d\n", sim.number_of_coders);
	printf("time_to_burnout = %ld\n", sim.time_to_burnout);
	printf("time_to_compile = %ld\n", sim.time_to_compile);
	printf("time_to_debug = %ld\n", sim.time_to_debug);
	printf("time_to_refactor = %ld\n", sim.time_to_refactor);
	printf("number_of_compiles_required = %d\n", sim.number_of_compiles_required);
	printf("dongle_cooldown = %ld\n", sim.dongle_cooldown);
	if (sim.scheduler == SCHED_FIFO)
		printf("scheduler = fifo\n");
	else
		printf("scheduler = edf\n");
	return (0);
}
