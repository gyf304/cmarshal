FROM ubuntu:20.04 as builder

RUN apt-get update && apt-get install -y make clang libclang-dev

WORKDIR /root
COPY . .
RUN make

FROM ubuntu:20.04

RUN apt-get update && apt-get install --no-install-recommends -y libclang1-10 python3 python3-pip

COPY --from=builder /root/cmarshal /usr/local/bin/cmarshal

COPY ./server /root/server
WORKDIR /root/server
RUN pip3 install -r requirements.txt

ENV PORT=8080
CMD [ "python3", "./server.py" ]
