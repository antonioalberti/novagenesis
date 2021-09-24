#!/bin/bash

BASE=`cd ../..; pwd`;

if [ $(find $BASE/IO/AAS01/ -type f -not -name '*ini' | wc -l) -gt 0 ] 
 then
	echo "Removendo arquivos do AAA"	
	find $BASE/IO/AAS01/ -type f -not -name '*ini' | xargs rm
	killall AAS
fi

if [ $(find $BASE/IO/APS01/ -type f -not -name '*ini' | wc -l) -gt 0 ] 
 then
	echo "Removendo arquivos do APA01"	
	find $BASE/IO/APS01/ -type f -not -name '*ini' | xargs rm
	killall APS
fi

if [ $(find $BASE/IO/APS02/ -type f -not -name '*ini' | wc -l) -gt 0 ] 
 then
	echo "Removendo arquivos do APA02"	
	find $BASE/IO/APS02/ -type f -not -name '*ini' | xargs rm
	killall APS
fi

if [ $(find $BASE/IO/GIRS/ -type f -not -name '*ini' | wc -l) -gt 0 ] 
 then
	echo "Removendo arquivos do GIRS"	
	find $BASE/IO/GIRS/ -type f -not -name '*ini' | xargs rm
	killall GIRS
fi

if [ $(find $BASE/IO/HTS/ -type f -not -name '*ini' | wc -l) -gt 0 ] 
 then
	echo "Removendo arquivos do HTS"	
	find $BASE/IO/HTS/ -type f -not -name '*ini' | xargs rm
	killall HTS
fi

if [ $(find $BASE/IO/NRNCS/ -type f -not -name '*ini' | wc -l) -gt 0 ] 
 then
	echo "Removendo arquivos do NRNCS"	
	find $BASE/IO/NRNCS/ -type f -not -name '*ini' | xargs rm
	killall NRNCS
fi

if [ $(find $BASE/IO/PGCS/ -type f -not -name '*ini' | wc -l) -gt 0 ] 
 then
	echo "Removendo arquivos do PGCS"	
	find $BASE/IO/PGCS/ -type f -not -name '*ini' | xargs rm
	killall PGCS
fi

if [ $(find $BASE/IO/POXS01/ -type f -not -name '*ini' | wc -l) -gt 0 ] 
 then
	echo "Removendo arquivos do POXA01"	
	find $BASE/IO/POXS01/ -type f -not -name '*ini' | xargs rm
	killall POXA
fi

if [ $(find $BASE/IO/PSS/ -type f -not -name '*ini' | wc -l) -gt 0 ] 
 then
	echo "Removendo arquivos do PSS"	
	find $BASE/IO/PSS/ -type f -not -name '*ini' | xargs rm
	killall PSS
fi

if [ $(find $BASE/IO/RMS/ -type f -not -name '*ini' | wc -l) -gt 0 ] 
 then
	echo "Removendo arquivos do RMA"	
	find $BASE/IO/RMS/ -type f -not -name '*ini' | xargs rm
	killall RMS
fi

if [ $(find $BASE/IO/SSS01/ -type f -not -name '*ini' | wc -l) -gt 0 ] 
 then
	echo "Removendo arquivos do SSA"	
	find $BASE/IO/SSS01/ -type f -not -name '*ini' | xargs rm
	killall SSS
fi

if [ $(find $BASE/IO/Source1/ -type f -not -name '*ini' | wc -l) -gt 0 ] 
 then
	echo "Removendo arquivos do Source1"	
	find $BASE/IO/Source1/ -type f -not -name '*ini' | xargs rm
	killall ContentApp
fi

if [ $(find $BASE/IO/Source2/ -type f -not -name '*ini' | wc -l) -gt 0 ] 
 then
	echo "Removendo arquivos do Source1"	
	find $BASE/IO/Source2/ -type f -not -name '*ini' | xargs rm
	killall ContentApp
fi

if [ $(find $BASE/IO/Source3/ -type f -not -name '*ini' | wc -l) -gt 0 ] 
 then
	echo "Removendo arquivos do Source1"	
	find $BASE/IO/Source3/ -type f -not -name '*ini' | xargs rm
	killall ContentApp
fi

if [ $(find $BASE/IO/Repository1/ -type f -not -name '*ini' | wc -l) -gt 0 ] 
 then
	echo "Removendo arquivos do Server"	
	find $BASE/IO/Repository1/ -type f -not -name '*ini' | xargs rm
	killall ContentApp
fi

if [ $(find $BASE/IO/Repository2/ -type f -not -name '*ini' | wc -l) -gt 0 ] 
 then
	echo "Removendo arquivos do Server"	
	find $BASE/IO/Repository2/ -type f -not -name '*ini' | xargs rm
	killall ContentApp
fi

if [ $(find $BASE/IO/IoTTestApp/ -type f -not -name '*ini' | wc -l) -gt 0 ] 
 then
	echo "Removendo arquivos do IoTTestApp"	
	find $BASE/IO/IoTTestApp/ -type f -not -name '*ini' | xargs rm
	killall IoTTestApp
fi

if [ $(find $BASE/IO/NBTestApp/ -type f -not -name '*ini' | wc -l) -gt 0 ]
 then
	echo "Removendo arquivos do NBSimpleTestApp"	
	find $BASE/IO/NBTestApp/ -type f -not -name '*ini' | xargs rm
	killall NBTestApp
fi

