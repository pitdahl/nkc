<<<<<<< HEAD
@rem Senden einer Datei zum NKC:
@rem Starten des Slaves am NKC (mit 115200 Baud)
@rem rs232d 

python D:\003-transfer\dl.py -p com3 -b 115200 -u %1
@pause

@rem sollen keine weiteren dateien mehr übertragen werden kann der Slave am NKC mit der taste 'x' beeinden werden.
=======
@rem Senden einer Datei zum NKC:
@rem Starten des Slaves am NKC (mit 115200 Baud)
@rem rs232d 

python D:\003-transfer\dl.py -p com3 -b 115200 -u %1
@pause

@rem sollen keine weiteren dateien mehr übertragen werden kann der Slave am NKC mit der taste 'x' beeinden werden.
>>>>>>> d55544c819019099ec79e8a3a517bbcc6d153a1b
@rem Die empfangenen files sollten dann im aktuellen Laufwerk liegen