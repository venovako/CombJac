#!/bin/bash
for I in *.pdf
do
	qpdf $I --linearize --decode-level=all --recompress-flate --compression-level=9 --object-streams=generate --remove-unreferenced-resources=yes --replace-input
done
unset I
