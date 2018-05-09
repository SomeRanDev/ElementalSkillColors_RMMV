# ElementalSkillColors_RMMV
An RPG Maker MV plugin developed purely with C++ with the use of node-gyp.

---

How to compile
---

 - Step (0):

Make sure you have NodeJS/NPM, Visual Studio, Python 2.7 installed onto your system. 

 - Step (1) (in a terminal/console window thingy):

`cd [directory of project] && npm install`

 - Step (2):

`"./node_modules/.bin/node-gyp" configure --target=v1.2.0 --arch=ia32`

 - Step (3):

`"./node_modules/.bin/node-gyp" build`

---

Notes
---

If using RPG Maker MV v1.6.0 or above, the version of NodeJS will be different. You can check a project's version of NodeJS by evaluating `process.versions.node` somewhere within the game (for example, the NWJS console).

Once you know the desired NodeJS version, replace "1.2.0" within Step (2) with your own NodeJS version.
