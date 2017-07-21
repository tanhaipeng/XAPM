/*
 * Copyright 2017 XAPM Author. All Rights Reserved.
 * XAPM TCP Server Drivered by Golang
 * 2017/7/21, by Tan Haipeng, create
 */

package main

import "fmt"
import (
	"github.com/go-ozzo/ozzo-log"
	"github.com/go-ozzo/ozzo-config"
)

var confPath string
var conf *config.Config
var logger *log.Logger

func init() {
	confPath = "config.json"
	conf = initConf()
	logger = initLogger(conf.GetString("Logpath", "xapmsrv.log"))
}

func main() {
	fmt.Println("XAPM SERVER START")
	logger.Debug("XAPM SERVER START")
	defer logger.Close()
}
