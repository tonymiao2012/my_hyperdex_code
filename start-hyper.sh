cd ~/hyperdex-tmp
hyperdex coordinator -f -l 127.0.0.1 -p 1987 --daemon
sleep 5
hyperdex daemon -f --listen=127.0.0.1 --listen-port=2013 --coordinator=127.0.0.1 --coordinator-port=1987 --data=/home/tony/hd1 --daemon
cd -
