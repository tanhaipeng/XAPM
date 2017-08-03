# XAPM Project

## server-src

compile:
```
go build -o xapmserver *.go
```

## win-src

compile:
```
1.  cd ..
2.  vi ext/xapm/config.m4
3.  ./buildconf
4.  ./configure --[with|enable]-xapm
5.  make
6.  ./php -f ext/xapm/xapm.php
7.  vi ext/xapm/xapm.c
8.  make
```
**Refer:** http://tanhp.com/index.php/archives/407/

## linux-src