FROM ubuntu:latest

# ベースイメージの作成時に一度だけapt-get updateを実行
RUN apt-get update && apt-get -y install vim build-essential git

# 環境変数を設定
ENV GIT_USER=gardener12449
ENV GIT_EMAIL=s2312449@u.tsukuba.ac.jp

WORKDIR /app/9cc

RUN git clone https://github.com/gardener12449/9cc.git && \
    cd 9cc && git config user.name "$GIT_USER" && git config user.email "$GIT_EMAIL"

# コンテナ起動後にbashが起動し、以降のコマンドを実行できるようにする
CMD ["bash"]
