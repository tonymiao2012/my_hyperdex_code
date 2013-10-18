#!/bin/bash
#source $object/scripts/orangefs/drop-cache.sh &> /dev/null
#mpiexec -np 4 -f ./client_nodes ./pfs_test >> ./run.log
SCRIPT_DIR=/export/home/yyin/common/research/papers_writing/object_vs_stripe/scripts/hyperdex

ss=64
#for np in 8 32 128 512
for np in 128
do
	#for req in 64 65 80 96 128
	for req in 64 65 80 96 128
	do
		for index in `seq 1 5`
		do
			echo '######'
			echo ${np}, ${req}, ${index}
			#source $object/scripts/orangefs/drop-cache.sh &> /dev/null
			mpiexec -np ${np} -f ./client_nodes ./kvs_put ${req} 2>&1 | tee -a ./logs/put_ss_${ss}_np_${np}_req_${req}.log
			sleep 10
			source $object/scripts/orangefs/drop-cache.sh &> /dev/null
			sleep 10
			mpiexec -np ${np} -f ./client_nodes ./kvs_get ${req} 2>&1 | tee -a ./logs/get_ss_${ss}_np_${np}_req_${req}.log
		done
	done
	sleep 20
	#cd $SCRIPT_DIR
	#source hyperdex-stop.sh
	#sleep 5
	#source hyperdex-clear.sh
	#sleep 5
	#source hyperdex-start.sh
	#sleep 5
	#source hyperdex-check.sh
	#sleep 5
	#cd -
done

