if [ $# != 0 ] 
then
 echo "express"
 ps aux|grep "Server -$1"|grep -v grep | awk '{printf $2"\t"}' | xargs kill -10
fi
