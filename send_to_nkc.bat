@rem Senden einer Datei zum NKC:
@rem Starten des Slaves am NKC (mit 115200 Baud)
@rem rs232d 

@rem python D:\003-transfer\dl.py -p com3 -b 115200 -u i2c_test.68k
python D:\003-transfer\dl.py -p com%1 -b 115200 -u %2
@pause

@rem sollen keine weiteren dateien mehr übertragen werden kann der Slave am NKC mit der taste 'x' beeinden werden.
@rem Die empfangenen files sollten dann im aktuellen Laufwerk liegen