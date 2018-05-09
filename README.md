# ElementalSkillColors_RMMV
An RPG Maker MV plugin developed purely with C++ with the use of node-gyp.

---

How to compile
---

Step (1):

`cd [directory of project] && npm install`

Step (2):

`"./node_modules/.bin/node-gyp" configure --target=v1.2.0 --arch=ia32`

Step (3):

`"./node_modules/.bin/node-gyp" build`
