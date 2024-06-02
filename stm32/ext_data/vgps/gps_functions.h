/** ---------------------------------------------------------- gps_functions.h --- *
 *                                                                                 *
 *  This header file declares functions for GPS simulation, including              *
 *  generating random coordinates, checking for movement, and simultating          *
 *  GPS data logging.                                                              *
 *                                                                                 *
 * ------------------------------------------------------------------------------ **/
#ifndef GPS_FUNCTIONS_H /* Include guard */
#define GPS_FUNCTIONS_H

#include "gps_creator.h"

/* Function declarations */
Coordinates generateRandomCoordinates();

bool isMoving(
		Coordinates        previousPosition,
		Coordinates        currentPosition);

void simulateGPS(
		Coordinates        *currentPosition, 
		bool               *isTimerOn, 
		int                *staticTimer,
		FILE               *allDataFile,
		FILE               *tempDataFile );

#endif /* GPS_FUNCTIONS_H */

