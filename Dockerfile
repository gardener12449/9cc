# Dockerfile
FROM ubuntu:latest

WORKDIR /app

RUN apt-get update && apt-get install -y git

RUN git clone https://github.com/gardener12449/9cc.git

# コンテナが終了しないようにbashを起動
CMD ["bash"]
