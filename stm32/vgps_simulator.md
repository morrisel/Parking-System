
### stm32 Directory
	This directory contains the README.md file and the ext_data directory.
	The ext_data directory holds temporary files used for simulating a GPS sensor during the development phase.
	Some of these files will be deleted or modified during the final project assembly.
	
###	Virtual GPS (out_mvgps)
#####	Purpose:
	This virtual GPS simulates the dynamic and static behavior of a GPS sensor.
	It generates random coordinates (x, y, z) and periodically fixes them for a specified duration (static mode).
	
###	Example output when running the ./out_mvgps program:
```bash
	./out_mvgps
	Data from GPS: x 91.37 y 72.45 z 0.70
	Data from GPS: x 11.55 y 81.65 z 34.44
	Data from GPS: x 47.41 y 0.31 z 55.67
	Data from GPS: x 41.73 y 65.84 z 50.68
	Data from GPS: x 10.59 y 30.40 z 28.88
	Data from GPS: x 65.19 y 26.89 z 65.26
	Static time: 2 seconds (x 65.19 y 26.89 z 65.26)
	Static time: 4 seconds (x 65.19 y 26.89 z 65.26)
	Static time: 6 seconds (x 65.19 y 26.89 z 65.26)
	Static time: 8 seconds (x 65.19 y 26.89 z 65.26)
	Static time: 10 seconds (x 65.19 y 26.89 z 65.26)
	Static time: 12 seconds (x 65.19 y 26.89 z 65.26)
	Static time: 14 seconds (x 65.19 y 26.89 z 65.26)
	Static time: 16 seconds (x 65.19 y 26.89 z 65.26)
	Static time: 18 seconds (x 65.19 y 26.89 z 65.26)
	Static time: 20 seconds (x 65.19 y 26.89 z 65.26)
	Static time: 22 seconds (x 65.19 y 26.89 z 65.26)
	Child process exited with status 0
```	
	
###	Generated Files
	all_datacar_locations.data: Stores all generated coordinates.
	datacar_locations.tmp: Contains the latest coordinates, updated each time ./out_mvgps is run.
---

###	Next Steps
	Data from the datacar_locations.tmp file will be transmitted to the BBG microcomputer
	via the GPIO interface using the I2C protocol in the following development stages.
	
###	Relevant Data for Tests
	At this stage of development, test data regarding the device's location 
	can be found in the ~/stm32/ext_data/gps_data/ directory.
