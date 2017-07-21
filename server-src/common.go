/*
 * Copyright 2017 XAPM Author. All Rights Reserved.
 * XAPM TCP Server Drivered by Golang
 * 2017/7/21, by Tan Haipeng, create
 */

package main

import "github.com/go-ozzo/ozzo-config"
import (
	"github.com/go-ozzo/ozzo-log"
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
	if fileDir != "" {
		os.Mkdir(fileDir, 0777)
	}
	logger = log.NewLogger()
	t1 := log.NewConsoleTarget()
	t2 := log.NewFileTarget()
	t2.FileName = logPath
	t2.MaxLevel = log.LevelError
	logger.Targets = append(logger.Targets, t1, t2)
	return logger
}
