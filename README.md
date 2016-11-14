Phalcon Framework
=================

Phalcon is a web framework implemented as a C extension offering high performance and lower resource consumption.

Get Started
-----------

Phalcon is written in C with platform independence in mind. As a result, Phalcon is available on Microsoft Windows, GNU/Linux, and Mac OS X. You can either download a binary package for the system of your choice or build it from sources.

### Linux/Unix/Mac

On a Unix-based platform you can easily compile and install the extension from sources.

#### Requirements
Prerequisite packages are:

* PHP 5.4.x/5.5.x development resources
* GCC compiler (Linux/Solaris) or Xcode (Mac)

Ubuntu:

```bash
sudo apt-get install php5-dev libpcre3-dev gcc make
```

Suse:

```bash
sudo zypper install php5-devel gcc make
```

CentOS/Fedora/RHEL

```bash
sudo yum install php-devel pcre-devel gcc make
```

Compilation
-----------

Follow these instructions to generate a binary extension for your platform:

```bash
git clone git://github.com/dreamsxin/cphalcon.git
cd cphalcon/ext
phpize
./configure
make -j4 && sudo make install
```

Add the extension to your php.ini:

```bash
extension=phalcon.so
```

Finally, restart the webserver.

External Links
--------------

* [Documentation](http://docs.phalconphp.com/)
* [Support](http://phalconphp.com/support)
* [Blog](http://blog.phalconphp.com)
* [Twitter](http://twitter.com/phalconphp)
* [中文帮助](https://github.com/dreamsxin/cphalcon7/wiki)
* [捐贈名單（Donation）](https://github.com/dreamsxin/cphalcon7/blob/master/DONATE.md)
* PHP5 系列 [Phalcon7](https://github.com/dreamsxin/cphalcon)
* Zephir 系列 [Phalcon](https://github.com/phalcon/cphalcon)

Current Build Status
--------------------

Phalcon Framework is built under the Travis CI service. Every commit pushed to this repository will queue a build into the continuous integration service and will run all PHPUnit tests to ensure that everything is going well and the project is stable. The current build status is:

[![Build Status](https://secure.travis-ci.org/dreamsxin/cphalcon.png?branch=master)](http://travis-ci.org/dreamsxin/cphalcon)

Meet the Incubator
-----------
Our community is developing amazing extra features for Phalcon every day via [Incubator](https://github.com/phalcon/incubator). There are resources to enhance your experience with the framework and that enlarge the main features.

Just give it a try and help us improve Phalcon even more!


License
-------
Phalcon is open source software licensed under the New BSD License. See the docs/LICENSE.txt file for more information.
