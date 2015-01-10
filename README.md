Torrentor!
=============

Torrentor is a torrent client for the Haiku operating system.

Requirements
-------
To build Torrentor you need some libs which are optional packages.

* OpenSSL   -- ```$ pkgman install lib:libssl```
* libevent  -- ```$ pkgman install lib:libevent```
* curl      -- ```$ pkgman install lib:libcurl```


Build
-------
Be sure you're using gcc4, you can set this by running ```$ setarch x86```
To build torrentor, browse to the root source directory and run ```$ jam```
