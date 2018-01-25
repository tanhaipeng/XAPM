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
	"strings"
	"strconv"
	"encoding/json"
	"sort"
)

/* 步骤数据结构 */
type stepType struct {
	Name  string `json:"name"`
	Index int    `json:"index"`
}

/* 自定义排序结构 */
type sortItem map[string]int

type sortType []sortItem

func (s sortType) Len() int {
	return len(s)
}

func (s sortType) Swap(i, j int) {
	s[i], s[j] = s[j], s[i]
}

func (s sortType) Less(i, j int) bool {
	s1, _ := getKey(s[i])
	s2, _ := getKey(s[j])
	if getValue(s1) < getValue(s2) {
		return true
	} else {
		return false
	}
}

func getKey(s map[string]int) (string, int) {
	for k, v := range s {
		return k, v
	}
	return "", 0
}

func getValue(s string) int {
	var step stepType
	json.Unmarshal([]byte(s), &step)
	return step.Index
}

/* 初始化配置文件 */
func initConf(confPath string) *config.Config {
	conf := config.New()
	conf.Load(confPath)
	return conf
}

/* 初始化日志模块 */
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

/* 启动TCP服务器 */
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

/* 处理请求 */
func handleRequest(conn net.Conn) {
	defer conn.Close()
	buf, err := ioutil.ReadAll(conn)
	if err != nil {
		logger.Error(err.Error())
		conn.Write([]byte("fail"))
	} else {
		formatApmData(string(buf))
		conn.Write([]byte("succ"))
	}
}

/* 格式化trace数据 */
func formatApmData(data string) {
	var order []string
	dsplit := strings.Split(data, "\n")
	if len(dsplit) > 2 {
		dsplit = dsplit[1:len(dsplit)-2]
		for _, item := range dsplit {
			info := strings.Split(item, "\t")
			if len(info) == 2 {
				_, err := strconv.ParseFloat(info[1], 64)
				if err == nil {
					order = append(order, info[1])
				} else {
					order = append(order, item)
				}
			}
		}
		var stack []string
		var sorts sortType
		for idx, item := range order {
			cost, err := strconv.ParseFloat(item, 64)
			if err != nil {
				ret, _ := json.Marshal(stepType{Name: item, Index: idx})
				// push
				stack = append(stack, string(ret))
			} else {
				// pop
				top := stack[len(stack)-1]
				stack = stack[:len(stack)-1]
				sorts = append(sorts, sortItem{top: int(cost)})
			}
		}
		sort.Sort(sorts)

		fmt.Println("\n")
		fmt.Println("Trace result:")
		for _, item := range sorts {
			var step stepType
			k, v := getKey(item)
			json.Unmarshal([]byte(k), &step)
			fmt.Println(extractDetail(step.Name), v, "ms")
		}
	}
}

/* 提取详细信息 */
func extractDetail(detail string) string {
	retval := ""
	infoList := strings.Split(detail, "*")
	if len(infoList) >= 2 {
		retval += infoList[0] + "\t" + infoList[1]
		if len(infoList) >= 3 {
			retval += "\t" + infoList[2]
		}
	}
	return retval
}
