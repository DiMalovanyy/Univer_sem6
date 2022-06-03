#!/bin/bash

EXEC=mpiexec
declare -a executables=(lab_1 lab_2 lab_3)
#declare -a input_data_sizes=(10 20)
declare -a input_data_sizes=(10 100 1000 2000 5000 6000 8000)
declare -a num_threads=(1 5 10)


printf "Executable\tNum Threads\t"
for data_size in "${input_data_sizes[@]}"
do
	printf "%i\t\t" "${data_size}"
done
printf "\n"
echo "-------------------------------------------------------------------------------------------------------------------------------------------"


declare -a random_arrays
for data_size_index in "${!input_data_sizes[@]}"; do
	declare -a random_array
	count=1
	data_size=${input_data_sizes[$data_size_index]}
	while [[ "$count" -le ${data_size} ]]; do
		random_array[$count]=$(( $RANDOM % 10000 ))
		let "count += 1"
	done
	random_arrays[$data_size_index]=${random_array[*]}
done


declare -a results

for executable in "${executables[@]}"
do
	if [ "${executable}" == "lab_1" ]; then
		printf "%s\t1\t\t" "NonParallel"
		for data_size_index in "${!input_data_sizes[@]}"; do

			random_array=()
			random_array=${random_arrays[$data_size_index]}

			#echo "${random_array[*]}"
			#echo "-----"
			#result=$(./lab_1 "${random_array[*]}" "${input_data_sizes[$data_size_index]}")
			time="$( TIMEFORMAT='%lU';time ( ./lab_1 "${random_array[*]}" "${input_data_sizes[$data_size_index]}" ) 2>&1 1>/dev/null )"
			error_code=$?

			if [ $error_code -ne 0 ]; then
				printf "Command %s failed on %s data size" "${executable}" "${input_data_sizes[$data_size_index]}"
				exit 1
			fi

			results[$data_size_index]=${result}
			printf "${time}\t"
		done
		printf "\n"
	elif [ "${executable}" == "lab_2" ]; then
		for num_thread in "${num_threads[@]}"; do
			printf "%s\t\t%i\t\t" "MPI" "${num_thread}"
			for data_size_index in "${!input_data_sizes[@]}"; do
				random_array=()
				random_array=${random_arrays[$data_size_index]}
				#result=$(mpiexec -n ${num_thread} ./lab_2 "${random_array[*]}" "${input_data_sizes[$data_size_index]}")
				time="$( TIMEFORMAT='%lU';time ( mpiexec -n ${num_thread} ./lab_2 "${random_array[*]}" "${input_data_sizes[$data_size_index]}") 2>&1 1>/dev/null )"

				if [ $error_code -ne 0 ]; then
					printf "Command %s failed on %s data size" "${executable}" "${input_data_sizes[$data_size_index]}"
					exit 1
				fi

				# if [ ${result} -ne "${results[$data_size_index]}" ]; then
				# 	printf "Incorrect\t"
				#	continue
				# fi

				printf "${time}\t"
			done
			printf "\n"
		done
	elif [ "${executable}" == "lab_3" ]; then
		for num_thread in "${num_threads[@]}"; do
			printf "%s\t\t%i\t\t" "OpenMP" "${num_thread}"
			for data_size_index in "${!input_data_sizes[@]}"; do
				random_array=()
				random_array=${random_arrays[$data_size_index]}
				#result=$(./lab_3 "${random_array[*]}" "${input_data_sizes[$data_size_index]}" "${num_thread}")
				time="$( TIMEFORMAT='%lU';time (./lab_3 "${random_array[*]}" "${input_data_sizes[$data_size_index]}" "${num_thread}") 2>&1 1>/dev/null )"

				if [ $error_code -ne 0 ]; then
					printf "Command %s failed on %s data size" "${executable}" "${input_data_sizes[$data_size_index]}"
					exit 1
				fi

				# if [ ${result} -ne "${results[$data_size_index]}" ]; then
				# 	printf "Incorrect\t"
				#	continue
				# fi

				printf "${time}\t"
			done
			printf "\n"
		done

	fi
done


