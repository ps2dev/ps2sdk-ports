FROM fjtrujy/ps2dev:gskit-latest

COPY . /src/ps2sdk-ports

RUN \
  apk add --no-cache --virtual .build-deps gcc musl-dev git && \
  cd /src/ps2sdk-ports && \
  make && \
  apk del .build-deps && \
  rm -rf \
    /src/* \
    /tmp/*

WORKDIR /src
