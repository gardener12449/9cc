#!/bin/bash

sudo docker run -it --name 9cc -v $SSH_AUTH_SOCK:/tmp/ssh-agent.sock \
       	        -e SSH_AUTH_SOCK=/tmp/ssh-agent.sock 9cc


