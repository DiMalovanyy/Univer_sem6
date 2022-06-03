#!/bin/bash

declare -a executables=(lab_1 lab_2 lab_3)
declare -a input_data_sizes=(10 100 10000 30000)
declare -a num_threads=(1 5 10 20)


printf "Executable\tNum Threads\t"
for data_size in "${input_data_sizes[@]}"
do
	printf "%i\t" "${data_size}"
done
printf "\n"
echo "----------------------------------------------------------------------"

for executable in "${executables[@]}"
do
	if [ "${executable}" == "lab_1" ]; then
		printf "%s\t\t1\t\t" "${executable}"
		for data_size in "${input_data_sizes[@]}"; do
			printf "2\t"
		done
		printf "\n"
	else
		for num_thread in "${num_threads[@]}"; do
			printf "%s\t\t%i\t\t" "${executable}" "${num_thread}"
			for data_size in "${input_data_sizes[@]}"; do
				printf "2\t"
			done
			printf "\n"
		done
	fi
done


