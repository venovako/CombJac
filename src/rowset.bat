FOR /L %%N IN (2,2,42) DO IF EXIST rowset_%%N.exe DEL /F rowset_%%N.exe
FOR /L %%N IN (2,2,10) DO icx.exe /nologo /O3 /QxHost /DN=%%Nu /DNDEBUG /DGRAPHVIZ_PREFIX="""""" /DASYMPTOTE_PREFIX="""""" /DPDFTK_PREFIX="""""" /Ferowset_%%N.exe rowset.cpp /link /RELEASE
FOR /L %%N IN (12,2,42) DO icx.exe /nologo /O3 /QxHost /DN=%%Nu /DNDEBUG /DASYMPTOTE_PREFIX="""""" /DPDFTK_PREFIX="""""" /Ferowset_%%N.exe rowset.cpp /link /RELEASE
REM IF EXIST timeit.dat DEL /F timeit.dat
REM FOR /L %%N IN (2,2,42) DO "C:\Program Files (x86)\Windows Resource Kits\Tools\timeit.exe" rowset_%%N.exe > rowset_%%N.out 2> rowset_%%N.er
FOR /L %%N IN (2,2,42) DO rowset_%%N.exe > rowset_%%N.out 2> rowset_%%N.err
REM rowset_2.exe > rowset_2.out 2> rowset_2.err
REM rowset_4.exe > rowset_4.out 2> rowset_4.err
REM rowset_6.exe > rowset_6.out 2> rowset_6.err
REM rowset_8.exe > rowset_8.out 2> rowset_8.err
REM rowset_10.exe > rowset_10.out 2> rowset_10.err
REM rowset_12.exe > rowset_12.out 2> rowset_12.err
REM rowset_14.exe > rowset_14.out 2> rowset_14.err
REM rowset_16.exe > rowset_16.out 2> rowset_16.err
REM rowset_18.exe > rowset_18.out 2> rowset_18.err
REM rowset_20.exe > rowset_20.out 2> rowset_20.err
REM rowset_22.exe > rowset_22.out 2> rowset_22.err
REM rowset_24.exe > rowset_24.out 2> rowset_24.err
REM rowset_26.exe > rowset_26.out 2> rowset_26.err
REM rowset_28.exe > rowset_28.out 2> rowset_28.err
REM rowset_30.exe > rowset_30.out 2> rowset_30.err
REM rowset_32.exe > rowset_32.out 2> rowset_32.err
REM rowset_34.exe > rowset_34.out 2> rowset_34.err
REM rowset_36.exe > rowset_36.out 2> rowset_36.err
REM rowset_38.exe > rowset_38.out 2> rowset_38.err
REM rowset_40.exe > rowset_40.out 2> rowset_40.err
REM rowset_42.exe > rowset_42.out 2> rowset_42.err
