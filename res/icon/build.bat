@echo off
magick 16.png 24.png 32.png 48.png 256.png result.ico
magick 48.png -background "#F0F0F0" -alpha remove -strip -type TrueColor BMP3:..\QuickPowerAction.bmp
copy result.ico ..\QuickPowerAction.ico