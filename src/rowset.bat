FOR /L %%N IN (2,2,8) DO icx.exe /nologo /O3 /QxHost /DN=%%Nu /DNDEBUG /DGRAPHVIZ_PREFIX="""""" /DASYMPTOTE_PREFIX="""""" /DPDFTK_PREFIX="""""" /Ferowset_%%N.exe rowset.cpp /link /RELEASE
FOR /L %%N IN (10,2,42) DO icx.exe /nologo /O3 /QxHost /DN=%%Nu /DNDEBUG /DASYMPTOTE_PREFIX="""""" /DPDFTK_PREFIX="""""" /Ferowset_%%N.exe rowset.cpp /link /RELEASE
IF EXIST timeit.dat DEL /F timeit.dat
REM FOR /L %%N IN (2,2,42) DO "C:\Program Files (x86)\Windows Resource Kits\Tools\timeit.exe" rowset_%%N.exe > rowset_%%N.out 2> rowset_%%N.err
rowset_2.exe > rowset_2.out 2> rowset_2.err
rowset_6.exe > rowset_6.out 2> rowset_6.err
rowset_10.exe > rowset_10.out 2> rowset_10.err
rowset_14.exe > rowset_14.out 2> rowset_14.err
rowset_18.exe > rowset_18.out 2> rowset_18.err
rowset_22.exe > rowset_22.out 2> rowset_22.err
rowset_26.exe > rowset_26.out 2> rowset_26.err
REM SLOW
REM rowset_30.exe > rowset_30.out 2> rowset_30.err
rowset_34.exe > rowset_34.out 2> rowset_34.err
rowset_38.exe > rowset_38.out 2> rowset_38.err
rowset_42.exe > rowset_42.out 2> rowset_42.err
REM EXTRA
rowset_4.exe > rowset_4.out 2> rowset_4.err
rowset_8.exe > rowset_8.out 2> rowset_8.err
rowset_12.exe > rowset_12.out 2> rowset_12.err
rowset_16.exe > rowset_16.out 2> rowset_16.err
rowset_20.exe > rowset_20.out 2> rowset_20.err
rowset_24.exe > rowset_24.out 2> rowset_24.err
rowset_28.exe > rowset_28.out 2> rowset_28.err
rowset_32.exe > rowset_32.out 2> rowset_32.err
rowset_36.exe > rowset_36.out 2> rowset_36.err
