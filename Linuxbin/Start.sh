echo "ulimit -S -c unlimited > /dev/null"
if [ $# != 0 ]
then
echo "Start $1"
./ServiceServer -$1 2 &
./ServiceServer -$1 6 &
./ServiceServer -$1 7 &
./ServiceServer -$1 8 &
./ServiceServer -$1 9 &
./ServiceServer -$1 10 &
./ServiceServer -$1 11 &
./ServiceServer -$1 12 &
./ServiceServer -$1 13 &
./ServiceServer -$1 14 &
./UnionServer -$1 5 &
./MatchServer -$1 4 &
./CheckServer -$1 3 &
./BattleServer -$1 1 &
./BattleServer -$1 15 &
./RankServer -$1 20 &
./SessionServer -$1 &
else
echo "Start -serverid"
./ServiceServer -serverid 2 &
./ServiceServer -serverid 6 &
./ServiceServer -serverid 7 &
./ServiceServer -serverid 8 &
./ServiceServer -serverid 9 &
./ServiceServer -serverid 10 &
./ServiceServer -serverid 11 &
./ServiceServer -serverid 12 &
./ServiceServer -serverid 13 &
./ServiceServer -serverid 14 &
./UnionServer -serverid 5 &
./MatchServer -serverid 4 &
./CheckServer -serverid 3 &
./BattleServer -serverid 1 &
./BattleServer -serverid 15 &
./RankServer -serverid 20 &
./SessionServer&
fi
