Compilations:

	1. # Compilation of source files into object files

		 gcc -c -fPIC gps_creator.c -o gps_creator.o
		 gcc -c -fPIC gps_functions.c -o gps_functions.o


	2. # Creation of a shared object file (".so" file)

		 gcc -shared -o libgps.so gps_creator.o gps_functions.o


	3. # Setting file permissions

		 chmod 644 libgps.so


	4. # Compilation and linking of the dynamic library

		 gcc gps_creator.c -o out_gps_creator -L. -lgps


	5. # If necessary, in case of problems running the program,
	     the issue may be related to the environment variable setting.
		 
		 We fix it as follows:

		 export LD_LIBRARY_PATH=. --> if we are in the current directory
		 export LD_LIBRARY_PATH=[dir/running/program] --> if we are running from another location

