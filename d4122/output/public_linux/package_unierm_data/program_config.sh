#!/bin/sh

rm -rf $1/unierm_data/fc_server_config
rm -rf $1/unierm_data/local_machine
rm -rf $1/unierm_data/probe_config

cp -R unierm_data $1/
chmod -R 777 $1/unierm_data 

