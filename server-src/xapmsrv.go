/*
 * Copyright 2017 XAPM Author. All Rights Reserved.
 * XAPM TCP Server Drivered by Golang
 * 2017/7/21, by AllinGoo, create
 */

package main

import (
	"github.com/go-ozzo/ozzo-config"
	"github.com/go-ozzo/ozzo-log"
	"fmt"
)

var conf *config.Config
var logger *log.Logger

func init() {
	conf = initConf("config.json")
	logger = initLogger(conf.GetString("Logpath", "log/xapmsrv.log"))
}

func main() {
	fmt.Println("XAPM SERVER START")
	logger.Notice("XAPM SERVER START")
	defer logger.Close()
}
