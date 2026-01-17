FOR /L %%N IN (2,2,42) DO icx.exe /nologo /O3 /QxHost /DN=%%Nu /DNDEBUG /Ferowset_%%N.exe rowset.cpp /link /RELEASE
IF EXIST timeit.dat DEL /F timeit.dat
FOR /L %%N IN (2,2,42) DO "C:\Program Files (x86)\Windows Resource Kits\Tools\timeit.exe" rowset_%%N.exe > rowset_%%N.out 2> rowset_%%N.err
