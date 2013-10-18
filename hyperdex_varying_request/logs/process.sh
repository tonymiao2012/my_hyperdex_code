for req in 64 65 80 96 128
do  
	echo $req
	grep 'Get' ./get*_np_${1}_req_${req}.log | awk -F' ' '{print $NF}';
done

