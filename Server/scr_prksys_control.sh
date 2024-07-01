#!/bin/bash


# Функция для запуска программы в новом терминале
run_in_terminal() {
    program="$1"
    gnome-terminal -- bash -c "./$program; exec bash"
}

# Запуск программы out_prk_sys_srv_run
echo "Starting out_prk_sys_srv_run..."
run_in_terminal "out_prk_sys_srv_run"

# Функция для определения активности данных в FIFO от out_giis
detect_data_activity() {
    fifo_file="$1"
    tail -f "$fifo_file" | while IFS= read -r line; do
        if echo "$line" | grep -q "data"; then
            break
        fi
    done
}

# Запуск out_insert_data_from_giis_shm при активности потока данных от out_giis
echo "Waiting for data from out_giis..."
detect_data_activity "giis/ipc_to_db"
detect_data_activity "giis/ipc_transfer_giis"

echo "Detected data from out_giis. Starting out_insert_data_from_giis_shm..."
run_in_terminal "out_insert_data_from_giis_shm"

echo "Control script finished."

