	T=$1
	Fisin=$2
	Fisout=$3
	export OMP_SCHEDULE="dynamic"
	export OMP_NUM_THREADS=8
	time ./tema1paralel $T $Fisin $Fisout
