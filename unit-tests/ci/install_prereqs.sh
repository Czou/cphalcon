#! /bin/sh

DIR=$(readlink -enq $(dirname $0))

sudo apt-get -qq update &

( pecl install apcu-4.0.11 < /dev/null || ( pecl config-set preferred_state beta; pecl install apcu < /dev/null ) && phpenv config-add "$DIR/apcu.ini" ) &

CFLAGS="-O1 -g3 -fno-strict-aliasing" pecl install igbinary < /dev/null &
CFLAGS="-O1 -g3 -fno-strict-aliasing" pecl install imagick < /dev/null &
CFLAGS="-O1 -g3 -fno-strict-aliasing" pecl install yaml < /dev/null &

wait
phpenv config-add "$DIR/memcache.ini"
phpenv config-add "$DIR/memcached.ini"
phpenv config-add "$DIR/mongo.ini"
phpenv config-add "$DIR/redis.ini"
phpenv config-rm xdebug.ini

sudo apt-get -qq install beanstalkd
echo 'DAEMON_OPTS="-l 127.0.0.1 -p 11300"' | sudo tee -a /etc/default/beanstalkd > /dev/null
echo 'START=yes' | sudo tee -a /etc/default/beanstalkd > /dev/null
sudo service beanstalkd restart

sudo apt-get -qq install libpng12-dev
sudo apt-get -qq install libqrencode-dev
sudo apt-get -qq install libzbar-dev
