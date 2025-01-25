#!/bin/bash

sudo docker rm test && sudo docker run -it --name test -v $SSH_AUTH_SOCK:/tmp/ssh-agent.sock -e SSH_AUTH_SOCK=/tmp/ssh-agent.sock test


