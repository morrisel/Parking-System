all:     out_tcp_client                        \
         out_ipc_sender

## Linux Server
## ------------
#out_server: server.c
#	gcc server.c -o out_server -lpthread
#out_listener: listener.c
#	gcc listener.c -o out_listener
#out_giis: giis.c
#	gcc giis.c -o out_giis -lpthread
#out_insert_data_from_giis_shm: insert_data_from_giis_shm.c
#	gcc insert_data_from_giis_shm.c -o out_insert_data_from_giis_shm
#out_update_prices: update_prices.c
#	gcc update_prices.c -o out_update_prices
#out_prk_sys_srv_run: prk_sys_srv_run.c
#	gcc prk_sys_srv_run.c -o out_prk_sys_srv_run
#

# BBG clients
# -----------
out_ipc_sender: ipc_sender.c
	gcc ipc_sender.c -o out_ipc_sender
out_tcp_client: tcp_client.c
	gcc tcp_client.c -o out_tcp_client


CLEAN_FILES = \
	out_tcp_client \
	out_ipc_sender


clean:
	rm -f $(CLEAN_FILES)


