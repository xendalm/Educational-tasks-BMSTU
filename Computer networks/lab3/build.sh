#!/bin/bash
export GOPATH=`pwd`
go get github.com/mgutz/logxi/v1
go get github.com/skorobogatov/input
export LOGXI=*
export LOGXI_FORMAT=pretty,happy
go install ./src/test