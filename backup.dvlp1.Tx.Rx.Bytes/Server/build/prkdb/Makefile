all: out_create_db_empty out_create_tables


out_create_db_empty: create_db_empty.c
	gcc create_db_empty.c -o out_create_db_empty

out_create_tables:   create_tables.c
	gcc create_tables.c -o out_create_tables


clean:
	rm -f out_create_db_empty out_create_tables

