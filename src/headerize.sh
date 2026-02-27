#!/bin/bash
echo '// auto-generated' > rowset.hpp
echo '! auto-generated' > rowset.f90
echo '#ifndef ROWSET_HPP' >> rowset.hpp
echo '#define ROWSET_HPP' >> rowset.hpp
echo 'module rowset' >> rowset.f90
echo 'use, intrinsic :: iso_fortran_env, only: int8' >> rowset.f90
echo 'implicit none' >> rowset.f90
for ((N=2;N<=42;N+=2))
do
	echo '#include "rowset_'$N'.hpp"' >> rowset.hpp
	echo "include 'rowset_"$N".f90'" >> rowset.f90
done
unset N
echo '#endif // !ROWSET_HPP' >> rowset.hpp
echo 'contains' >> rowset.f90
echo 'end module rowset' >> rowset.f90
