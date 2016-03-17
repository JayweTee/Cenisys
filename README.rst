Cenisys
=======

.. image:: https://gitlab.com/itxtech/cenisys/badges/master/build.svg
   :alt: GitLab CI
   :target: https://gitlab.com/itxtech/cenisys/builds

Canonical source
----------------

The source of Cenisys is maintained on GitLab.com_. Currently Cenisys is mirrored on GitHub for convenience.
You should make contributions on GitLab if possible.

.. _GitLab.com: https://gitlab.com/itxtech/cenisys

Ultra fast Minecraft server written in C++
------------------------------------------

Cenisys is written in C++ and has the following feature:

- Asynchronous I/O with Boost.Asio
- Utilizing the latest C++ standard to provide easy-to-use API

Requirements
------------

You will need the following libraries for build:

- Boost 1.61
- yaml-cpp

To generate documentation, you will need additional tools:

- Sphinx
- breathe
- doxygen

The required C++ standard is C++14.

License
-------

Cenisys is licensed under GPLv3. Plugins must be licensed under a compatible license.

Documentations
--------------

Documentations are bundled with release. You can also browse the git version on `GitLab Pages`_.

.. _GitLab Pages: https://itxtech.gitlab.io/cenisys

Translations
------------

Join our translation project on Zanata_! Your help is appreciated.

.. _Zanata: https://translate.zanata.org/project/view/cenisys

Special Thanks
--------------

- Thanks to PocketMine team: Protocol is based on PocketMine-MP source
- Thanks to Bukkit: API is inspired by Bukkit
- Thanks to Glowstone team: Internal design is based on Glowstone one
