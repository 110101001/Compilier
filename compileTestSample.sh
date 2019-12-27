for file in `ls ~/compiler/Lab/Test/*.cmm`
do
	./parser $file `echo $file | sed 's/cmm/s/g'`
done
