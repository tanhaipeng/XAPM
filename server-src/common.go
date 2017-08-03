/*
 * Copyright 2017 XAPM Author. All Rights Reserved.
 * XAPM TCP Server Drivered by Golang
 * 2017/7/21, by AllinGoo, create
 */

package main

import (
	"github.com/go-ozzo/ozzo-log"
	"github.com/go-ozzo/ozzo-config"
	"path/filepath"
	"os"
	"net"
	"fmt"
	"io/ioutil"
)

func initConf(confPath string) *config.Config {
	conf := config.New()
	conf.Load(confPath)
	return conf
}

func initLogger(logPath string) *log.Logger {
	fileDir := filepath.Dir(logPath)
	if fileDir != "" && fileDir != "/" {
		os.Mkdir(fileDir, 0777)
	}
	logger := log.NewLogger()
	t1 := log.NewConsoleTarget()
	t2 := log.NewFileTarget()
	t2.FileName = logPath
	t2.MaxLevel = log.LevelDebug
	logger.Targets = append(logger.Targets, t1, t2)
	logger.Open()
	return logger
}

func startServer(port string) {
	listen, err := net.Listen("tcp", "0.0.0.0:"+port)
	if err != nil {
		fmt.Printf(err.Error())
		logger.Error(err.Error())
		os.Exit(1)
	}
	defer listen.Close()
	fmt.Printf("TCP Server listening on 0.0.0.0:%s\n", port)
	for {
		conn, err := listen.Accept()
		if err != nil {
			logger.Error(err.Error())
		} else {
			go handleRequest(conn)
		}
	}
}

func handleRequest(conn net.Conn) {
	defer conn.Close()
	buf, err := ioutil.ReadAll(conn)
	if err != nil {
		logger.Error(err.Error())
		conn.Write([]byte("fail"))
	} else {
		decodePbData(buf)
		conn.Write([]byte("succ"))
	}
}

func decodePbData(data []byte) bool {
	logger.Info("decode protobuf data")
	return true
}
