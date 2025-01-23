#!/bin/bash

sudo docker rm test && sudo docker run -it --name test -v /tmp/ssh-XXXXXXJ5jytx/agent.10321:/tmp/ssh-agent.sock -e SSH_AUTH_SOCK=/tmp/ssh-agent.sock test
