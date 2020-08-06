# K-means clustering extension for PHP

## Installation

```
# install PHP-CPP
RUN git clone https://github.com/CopernicaMarketingSoftware/PHP-CPP.git && \
    cd PHP-CPP && make && make install

# build extension 

RUN git clone https://github.com/karlerss/php-cpp-kmeans.git && \
    cd php-cpp-kmeans && make && make install
```

## Usage

```
$clusters = cpp_kmeans(15, [
    123 => [345,456],
    223 => [867,867],
    ...
])
```
