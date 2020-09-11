FROM python:3.6

RUN apt update -y
RUN apt install -y swig


RUN mkdir /app
WORKDIR /app
COPY . /app


RUN python3 setup.py bdist_wheel
RUN python3 setup.py install
RUN python3 -m unittest -b
