#!/bin/bash


# Function to run a program in a new terminal
run_in_terminal()
{
    program="$1"
    gnome-terminal -- bash -c "./$program; exec bash"
}

# Start the out_prk_sys_srv_run program
echo "Starting out_prk_sys_srv_run..."
run_in_terminal "out_prk_sys_srv_run"

# Function to detect data activity in the FIFO from out_giis
detect_data_activity()
{
    fifo_file="$1"
    tail -f "$fifo_file" | while IFS= read -r line; do
        if echo "$line" | grep -q "data"; then
            break
        fi
    done
}

# Start out_insert_data_from_giis_shm when data activity from out_giis is detected
echo "Waiting for data from out_giis..."
detect_data_activity "giis/ipc_to_db"
detect_data_activity "giis/ipc_transfer_giis"

echo "Detected data from out_giis. Starting out_insert_data_from_giis_shm..."
run_in_terminal "out_insert_data_from_giis_shm"

echo "Control script finished."

