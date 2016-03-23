#!/bin/bash
touch /var/ethx.txt
ifconfig -a | grep eth | awk '{print $1}' > /var/ethx.txt
chmod 777 /var/ethx.txt
