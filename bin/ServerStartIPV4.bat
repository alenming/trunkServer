start CheckServer.exe -g 1001 -ipv 0 -sg 1 -si 1 -ip 127.0.0.1 -p 5556
start ServiceServer.exe -g 1001 -ipv 0 -sg 2 -si 1 -ip 127.0.0.1 -p 5557
start BattleServer.exe -g 1001 -ipv 0 -sg 3 -si 1 -ip 127.0.0.1 -p 5558
start MatchServer.exe -g 1001 -ipv 0 -sg 4 -si 1 -ip 127.0.0.1 -p 5559
start UnionServer.exe -g 1001 -ipv 0 -sg 5 -si 1 -ip 127.0.0.1 -p 5560
start CenterServer.exe -g 1001 -ipv 0 -sg 6 -si 1 -ip 127.0.0.1 -p 5561
start ChatServer.exe -g 1001 -ipv 0 -sg 7 -si 1 -ip 127.0.0.1 -p 5562 -checksg 1 -checkp 5556
start SessionServer.exe -g 1001 -ipv 0 -sg 0 -si 1 -ip 0 -p 5555 -connsg 1 -connsc 1 -connsg 2 -connsc 1 -connsg 3 -connsc 1 -connsg 4 -connsc 1 -connsg 5 -connsc 1 -connsg 6 -connsc 1 -chatsg 7 -chatsc 1
