all: out_tcp_client out_ipc_sender out_server

out_tcp_client: tcp_client.c
	gcc tcp_client.c -o out_tcp_client

out_ipc_sender: ipc_sender.c
	gcc ipc_sender.c -o out_ipc_sender

out_server: server.c
	gcc server.c -o out_server -lpthread

#out_reader: reader.c
#	gcc reader.c -o out_reader

clean:
	rm -f out_tcp_client out_ipc_sender out_server

