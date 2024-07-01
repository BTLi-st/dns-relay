#!/bin/bash

iptables -t nat -A PREROUTING -p udp -d 192.168.1.100 --dport 53 -j DNAT --to-destination 127.0.0.1:12345
iptables -t nat -A POSTROUTING -p udp -s 127.0.0.1 --sport 12345 -j SNAT --to-source 192.168.1.100:53