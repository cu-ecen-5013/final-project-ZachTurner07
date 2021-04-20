#! /bin/sh

case "$1" in
    start)
        echo "Starting serial UART server"
        start-stop-daemon -S -n uartserver -a /usr/bin/uartserver -- -d
        ;;
    stop)
        echo "Stopping serial UART server"
        start-stop-daemon -K -n uartserver
        ;;
    *)
        echo "Usage: $0 {start|stop}"
    exit 1
esac

exit 0