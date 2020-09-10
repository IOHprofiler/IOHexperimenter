FROM python:3.6

RUN mkdir /app
WORKDIR /app
COPY . /app

RUN python3 setup.py bdist_wheel
RUN python3 setup.py install
