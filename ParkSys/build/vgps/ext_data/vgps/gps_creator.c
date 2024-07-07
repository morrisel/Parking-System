/** ------------------------------------------------------------ gps_creator.c --- *
 *                                                                                 *
 *  Entry point for the GPS simulation program, handling initialization,           *
 *  file operations, and simulation control.                                       *
 *                                                                                 *
 *  Basic compilation and linking processing:                                      *
 *  gcc gps_creator.c gps_functions.c -o out_gps_creator                           *
 *                                                                                 *
 * ------------------------------------------------------------------------------ **/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#include "gps_creator.h"
#include "gps_functions.h"

int main()
{
	Coordinates prevpos;
	Coordinates currpos;

	bool  istimeron = true;
	int   stattimer = 0;
	int   dyntimer  = 0;

	/* Open file for appending all data */
	FILE *alldf = fopen("mcu/stm32/nucleo/f746zg/all_datacar_locations.data", "a");
	if (alldf == NULL)
	{
		perror("Error opening all data file");
		return EXIT_FAILURE;
	}

	/* Open file for writing temporary data */
	FILE *tmpdf = fopen("mcu/stm32/nucleo/f746zg/datacar_locations.tmp", "w");
	if (tmpdf == NULL)
	{
		perror("Error opening temporary data file");
		fclose(alldf);
		return EXIT_FAILURE;
	}

	/* Initialize random number generator */
	srand(time(NULL));

	/* Generate a random number of GPS outputs */
	int rndit = rand() % 10 + 3; /* From 3 to 12 outputs */
	int itcnt = 0;

	/* Get initial position */
	prevpos = generateRandomCoordinates();

	/* Generate a random maximum time for static mode (e.g., from 10 to 60 seconds) */
	int maxst = rand() % 51 + 10;

	while (dyntimer < maxst)
	{
		if (istimeron && itcnt < rndit)
		{
			simulateGPS(&currpos, &istimeron, &stattimer, alldf, tmpdf);

			/* Now compare with previous coordinates */
			if (isMoving(prevpos, currpos))
			{
				stattimer = 0; /* Reset static timer */
			}
			else
			{
				(stattimer) += 2;  /* Increase static timer */
			}

			/* 5 seconds without movement */
			if (stattimer >= 5)
			{
				istimeron = false; /* Turn off dynamic timer */
			}

			prevpos = currpos; /* Update previous coordinates */

			itcnt++;
		}
		else
		{
			dyntimer += 2;

			printf("Static time: %d seconds (x %.2lf y %.2lf z %.2lf)\n",
					dyntimer,
					currpos.x,
					currpos.y,
					currpos.z);

			/* Write static time info to all data file */
			fprintf(alldf, "Static time: %d seconds (x %.2lf y %.2lf z %.2lf)\n",
					dyntimer,
					currpos.x,
					currpos.y,
					currpos.z);

			/* Write static time info to temporary data file */
			fprintf(tmpdf, "Static time: %d seconds (x %.2lf y %.2lf z %.2lf)\n",
					dyntimer,
					currpos.x,
					currpos.y,
					currpos.z);
		}

		sleep(1); /* Delay for 1 second */
	}

	/* Close files */
	fclose(alldf);
	fclose(tmpdf);

	return 0;
}
