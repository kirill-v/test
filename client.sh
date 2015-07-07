#!/bin/bash

if [[ $1 == "-h" || $1 == "--help" || $# == 0 ]]; then
	echo "Usage: $0 command"
	echo "Supported commands:"
	printf "\t set-led-state on/off\n"
	printf "\t set-led-color red/green/blue\n"
	printf "\t set-led-rate [0-5]\n"
	printf "\t get-led-state\n"
	printf "\t get-led-color\n"
	printf "\t get-led-rate\n"
	printf "\t stop-server\n"
	exit 0
fi

request_pipe=/tmp/led_request
response_pipe=/tmp/led_response

(
flock -x 999
echo "Executing command: $@"
echo  "$@" > ${request_pipe}
read line < ${response_pipe}
echo "Result: $line"
) 999>/tmp/led
