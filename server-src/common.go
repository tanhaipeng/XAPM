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
)

func initConf() *config.Config {
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
