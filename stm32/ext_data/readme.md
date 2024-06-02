
I'm here ---> "ext_data/"

Running process:

     1. vgps/gps_creator.c
	 		create files in vgps/mcu/stm32/nucleo/f746zg/

     2. gps_data_transfer.c
	 		create tmp dir if its not exist and copy gps' files into tmp dir


     gcc -c gps_creator.c -o gps_creator.o
     gcc -c gps_data_transfer.c -o gps_data_transfer.o
     gcc gps_creator.o gps_data_transfer.o -o your_program_name






     
     gcc -shared -o libgps_creator.so vgps/gps_creator.c -fPIC
     gcc -shared -o libgps_data_transfer.so gps_data_transfer.c -fPIC
     gcc vgps.c -o out_vgps -ldl
     gcc vgps.c vgps/gps_creator.c gps_data_transfer.c -o out_vgps -ldl





	Пример команд для компиляции
	----------------------------
	Компиляция исходного кода в объектные файлы:
	
	gcc -c -fPIC gps_creator.c -o gps_creator.o
	gcc -c -fPIC gps_functions.c -o gps_functions.o


	Создание общего объекта (.so файла):

	gcc -shared -o libgps.so gps_creator.o gps_functions.o



	Если компиляция программы была успешной и появляются проблемы при запуске с привязкой окружения, то
	есть несколько способов решения этой проблемы:

	Установка переменной окружения LD_LIBRARY_PATH:
	Вы можете установить переменную окружения LD_LIBRARY_PATH, чтобы указать системе, 
	где искать динамические библиотеки.
	Например, если ваша библиотека libgps.so находится в текущем каталоге,
	выполните следующую команду перед запуском main_program:
	
	export LD_LIBRARY_PATH=.
	Затем попробуйте снова запустить main_program.
	
	Копирование библиотеки в каталог с программой:
	Вы также можете скопировать библиотеку libgps.so в каталог с вашей программой.
	Например, если ваша программа и библиотека находятся в одном каталоге vgps, выполните следующую команду:
	
	cp libgps.so vgps/
	Затем перейдите в каталог vgps и попробуйте снова запустить main_program.
	
	Изменение переменной окружения LD_LIBRARY_PATH во время выполнения:
	Если вы хотите избежать постоянной установки переменной окружения LD_LIBRARY_PATH,
	вы можете установить ее только на время выполнения программы. Например:
	
	LD_LIBRARY_PATH=. ./main_program
	Выберите тот метод, который наиболее удобен для вашей ситуации.
