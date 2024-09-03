debian@beaglebone:~/build/src$ ip a
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN group default qlen 1
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
    inet 127.0.0.1/8 scope host lo
       valid_lft forever preferred_lft forever
    inet6 ::1/128 scope host 
       valid_lft forever preferred_lft forever
2: can0: <NOARP,ECHO> mtu 16 qdisc noop state DOWN group default qlen 10
    link/can 
3: can1: <NOARP,ECHO> mtu 16 qdisc noop state DOWN group default qlen 10
    link/can 
4: eth0: <NO-CARRIER,BROADCAST,MULTICAST,DYNAMIC,UP> mtu 1500 qdisc mq state DOWN group default qlen 1000
    link/ether e0:62:34:66:fe:ac brd ff:ff:ff:ff:ff:ff
5: usb0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UP group default qlen 1000
    link/ether e0:62:34:66:fe:ae brd ff:ff:ff:ff:ff:ff
    inet 192.168.7.2/30 brd 192.168.7.3 scope global usb0
       valid_lft forever preferred_lft forever
    inet6 fe80::e262:34ff:fe66:feae/64 scope link 
       valid_lft forever preferred_lft forever
6: usb1: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UP group default qlen 1000
    link/ether e0:62:34:66:fe:b1 brd ff:ff:ff:ff:ff:ff
    inet 192.168.6.2/30 brd 192.168.6.3 scope global usb1
       valid_lft forever preferred_lft forever
    inet6 fe80::e262:34ff:fe66:feb1/64 scope link 
       valid_lft forever preferred_lft forever
debian@beaglebone:~/build/src$ 



debian@beaglebone:~/build/src$ uname -a
Linux beaglebone 4.9.78-ti-r94 #1 SMP PREEMPT Fri Jan 26 21:26:24 UTC 2018 armv7l GNU/Linux
debian@beaglebone:~/build/src$ uname -m
armv7l
debian@beaglebone:~/build/src$



