start CheckServer.exe -g 1001 -ipv 1 -sg 1 -si 1 -ip ::1 -p 5556
start ServiceServer.exe -g 1001 -ipv 1 -sg 2 -si 1 -ip ::1 -p 5557
start BattleServer.exe -g 1001 -ipv 1 -sg 3 -si 1 -ip ::1 -p 5558
start MatchServer.exe -g 1001 -ipv 1 -sg 4 -si 1 -ip ::1 -p 5559
start UnionServer.exe -g 1001 -ipv 1 -sg 5 -si 1 -ip ::1 -p 5560
start RankServer.exe -g 1001 -ipv 1 -sg 6 -si 1 -ip ::1 -p 5561
start SessionServer.exe -g 1001 -ipv 1 -si 1 -ip 0 -p 5555 -sg 1 -sc 1 -sg 2 -sc 1 -sg 3 -sc 1 -sg 4 -sc 1 -sg 5 -sc 1 -sg 6 -sc 1
