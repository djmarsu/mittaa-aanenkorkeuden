Toteutusdokumentti
==================
config.h                muka konfiguraatiotiedosto
freqandpitch.c          taajuus->nuotin nimi konvertointi jne
hw.c                    ALSA äänikortin käsittelyjutut
mittaa-aanenkorkeuden.c main() ja luuppi
params.c                komentoriviparametrien varmistus ja usage()
pitchdetect.c           signaalinkäsittelyalgoritmit

Saavutetut aika- ja tilavaativuudet
-----------------------------------
aika: O(4 * n)
tila: O(2 * n)

Työn mahdolliset puutteet ja parannusehdotukset
-----------------------------------------------
Ehkä parempi filtteri? Zero-crossing on itsessään huonohko tapa estimoida 
taajuutta...
