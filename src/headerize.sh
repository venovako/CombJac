#!/bin/bash
echo '/* auto-generated */' > rowset.h
echo '! auto-generated' > rowset.f90
echo '#ifndef ROWSET_H' >> rowset.h
echo '#define ROWSET_H' >> rowset.h
echo 'module rowset' >> rowset.f90
echo 'use, intrinsic :: iso_fortran_env, only: int8' >> rowset.f90
echo 'implicit none' >> rowset.f90
for ((N=2;N<=42;N+=2))
do
	echo '#include "rowset_'$N'.hdr"' >> rowset.h
	echo "include 'rowset_"$N".ftn'" >> rowset.f90
done
unset N
echo '#endif /* !ROWSET_H */' >> rowset.h
echo 'contains' >> rowset.f90
echo 'end module rowset' >> rowset.f90
